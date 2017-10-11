#include "efl_gesture_private.h"

#define MY_CLASS EFL_GESTURE_MANAGER_CLASS

typedef struct _Object_Gesture
{
   Eo                             *object;
   const Efl_Event_Description    *type;
   Efl_Gesture                    *gesture;
   Efl_Gesture_Recognizer         *recognizer;
}Object_Gesture;

typedef struct _Efl_Gesture_Manager_Data
{
   // keeps track of all the gesture request for a particular target
   Eina_Hash    *m_gesture_contex;  // (*target, *event_desc)
   // keeps all the event directed to this particular object from touch_begin till touch_end
   Eina_Hash    *m_object_events;  // (*target, *efl_gesture_touch)
   // keeps all the recognizer registered to gesture manager
   Eina_Hash    *m_recognizers; // (*gesture_type, *recognizer)
   // keeps track of all current object gestures.
   Eina_List    *m_object_gestures; //(List of *object_gesture)
   // lazy deletion of gestures
   Eina_List    *m_gestures_to_delete;

   Eina_Hash    *m_config;
} Efl_Gesture_Manager_Data;

static void _cleanup_cached_gestures(Efl_Gesture_Manager_Data *pd,
                                     Eo *target, const Efl_Event_Description *type);
static Efl_Gesture*
_get_state(Efl_Gesture_Manager_Data *pd, Eo *target,
                       Efl_Gesture_Recognizer *recognizer, const Efl_Event_Description *type);

static void
_cleanup_object(Eina_List *list)
{
   Eina_List *l;
   Eo *obj;

   if (!list) return;

   EINA_LIST_FOREACH(list, l, obj)
     efl_del(obj);

   eina_list_free(list);
}

static void
_hash_free_cb(Eo *obj)
{
   efl_del(obj);
}

EOLIAN static Efl_Object *
_efl_gesture_manager_efl_object_constructor(Eo *obj, Efl_Gesture_Manager_Data *pd EINA_UNUSED)
{
   obj = efl_constructor(efl_super(obj, MY_CLASS));

   pd->m_recognizers = eina_hash_pointer_new(EINA_FREE_CB(_hash_free_cb));
   pd->m_gesture_contex = eina_hash_pointer_new(NULL);
   pd->m_object_events = eina_hash_pointer_new(EINA_FREE_CB(_hash_free_cb));
   pd->m_object_gestures = NULL;
   pd->m_gestures_to_delete = NULL;

   pd->m_config = eina_hash_string_superfast_new(EINA_FREE_CB(eina_value_free));

   efl_gesture_manager_register_recognizer(obj, efl_add(EFL_GESTURE_RECOGNIZER_TAP_CLASS, obj));
   return obj;
}


EOLIAN static Eina_Value *
_efl_gesture_manager_config_get(Eo *obj EINA_UNUSED, Efl_Gesture_Manager_Data *pd, const char *name)
{
   return eina_hash_find(pd->m_config, name);
}

EOLIAN static void
_efl_gesture_manager_config_set(Eo *obj EINA_UNUSED, Efl_Gesture_Manager_Data *pd, const char *name, Eina_Value *value)
{
   Eina_Value *v = eina_value_new(eina_value_type_get(value));
   eina_value_copy(value, v);
   eina_hash_add(pd->m_config, name, v);
}

EOLIAN static void
_efl_gesture_manager_efl_object_destructor(Eo *obj, Efl_Gesture_Manager_Data *pd EINA_UNUSED)
{
   eina_hash_free(pd->m_recognizers);
   eina_hash_free(pd->m_gesture_contex);
   eina_hash_free(pd->m_object_events);
   _cleanup_object(pd->m_gestures_to_delete);
   efl_destructor(efl_super(obj, MY_CLASS));
}

void
_efl_gesture_manager_callback_add_hook(Eo *obj, Eo *target, const Efl_Event_Description *type)
{
   Efl_Gesture_Manager_Data *pd = efl_data_scope_get(obj, MY_CLASS);
   // if there is a recognizer registered for that event then add it to the gesture context
   Efl_Gesture_Recognizer *recognizer = eina_hash_find (pd->m_recognizers, &type);
   if (recognizer)
     {
        // add it to the gesture context.
        eina_hash_list_append(pd->m_gesture_contex, &target, type);
     }

}

void
_efl_gesture_manager_callback_del_hook(Eo *obj, Eo *target, const Efl_Event_Description *type)
{
   Efl_Gesture_Manager_Data *pd = efl_data_scope_get(obj, MY_CLASS);
   // if there is a recognizer registered for that event then add it to the gesture context
   Efl_Gesture_Recognizer *recognizer = eina_hash_find (pd->m_recognizers, &type);
   if (recognizer)
     {
        eina_hash_list_remove(pd->m_gesture_contex, &target, type);
        _cleanup_cached_gestures(pd, target, type);
     }
}

void
_efl_gesture_manager_filter_event(Eo *obj, Eo *target, void *event)
{
   Eina_List *l, *gesture_context;
   Efl_Gesture_Manager_Data *pd;
   const Efl_Event_Description *gesture_type;
   Efl_Gesture_Recognizer *recognizer;
   Efl_Gesture *gesture;
   Efl_Gesture_Recognizer_Result recog_result;
   Efl_Gesture_Recognizer_Result recog_state;
   Efl_Gesture_Touch *touch_event;
   Efl_Input_Pointer_Data *pointer_data = efl_data_scope_get(event, EFL_INPUT_POINTER_CLASS);

   pd = efl_data_scope_get(obj, MY_CLASS);
   gesture_context = eina_hash_find(pd->m_gesture_contex, &target);
   if (gesture_context)
     {
        // get the touch event for this particular widget
        touch_event = eina_hash_find(pd->m_object_events, &target);
        if (touch_event)
          {
             efl_gesture_touch_point_record(touch_event, pointer_data->tool, pointer_data->cur.x, pointer_data->cur.y,
                                          pointer_data->timestamp, pointer_data->action);
          }
        else
          {
             touch_event = efl_add(EFL_GESTURE_TOUCH_CLASS, NULL);
             efl_gesture_touch_point_record(touch_event, pointer_data->tool, pointer_data->cur.x, pointer_data->cur.y,
                                          pointer_data->timestamp, pointer_data->action);
             eina_hash_add(pd->m_object_events, &target, touch_event);
          }

        if (efl_gesture_touch_type_get(touch_event) == EFL_GESTURE_TOUCH_UNKNOWN)
          return;

        EINA_LIST_FOREACH(gesture_context, l, gesture_type)
          {
             recognizer = eina_hash_find(pd->m_recognizers, &gesture_type);
             gesture = _get_state(pd, target, recognizer, gesture_type);
             if (!gesture)
               continue;
             recog_result = efl_gesture_recognizer_recognize(recognizer, gesture, target, touch_event);
             recog_state = recog_result & EFL_GESTURE_RESULT_MASK;
             if (recog_state == EFL_GESTURE_TRIGGER)
               {
                  if (efl_gesture_state_get(gesture) == EFL_GESTURE_NONE)
                    efl_gesture_state_set(gesture, EFL_GESTURE_STARTED);
                  else
                    efl_gesture_state_set(gesture, EFL_GESTURE_UPDATED);
               }
             else if (recog_state == EFL_GESTURE_FINISH)
               {
                  efl_gesture_state_set(gesture, EFL_GESTURE_FINISHED);
               }
             else if (recog_state == EFL_GESTURE_MAYBE)
               {
                  continue;
               }
             else if (recog_state == EFL_GESTURE_CANCEL)
               {
                  if (efl_gesture_state_get(gesture) != EFL_GESTURE_NONE)
                    {
                       efl_gesture_state_set(gesture, EFL_GESTURE_CANCELED);
                    }
                  else
                    continue;
               }
             else if (recog_state == EFL_GESTURE_IGNORE)
               {
                  continue;
               }
             efl_event_callback_call(target, gesture_type, gesture);
          }

        if (efl_gesture_touch_type_get(touch_event) == EFL_GESTURE_TOUCH_END)
          {
             EINA_LIST_FOREACH(gesture_context, l, gesture_type)
               _cleanup_cached_gestures(pd, target, gesture_type);

             eina_hash_del(pd->m_object_events, &target, NULL);
             // free gesture_to_delete list
             _cleanup_object(pd->m_gestures_to_delete);
             pd->m_gestures_to_delete = NULL;
          }
     }
}

EOLIAN static const Efl_Event_Description *
_efl_gesture_manager_register_recognizer(Eo *obj EINA_UNUSED, Efl_Gesture_Manager_Data *pd,
                                            Efl_Gesture_Recognizer *recognizer)
{
   Efl_Gesture_Recognizer_Data *rpd;
   Efl_Gesture *dummy = efl_gesture_recognizer_create(recognizer, 0);

   if (!dummy)
     return NULL;

   const Efl_Event_Description *type = efl_gesture_type_get(dummy);

   // Add the recognizer to the m_recognizers
   eina_hash_add(pd->m_recognizers, &type, recognizer);
   // update the manager
   rpd = efl_data_scope_get(recognizer, EFL_GESTURE_RECOGNIZER_CLASS);
   rpd->manager = obj;

   efl_del(dummy);

   return type;
}

EOLIAN static void
_efl_gesture_manager_unregister_recognizer(Eo *obj EINA_UNUSED, Efl_Gesture_Manager_Data *pd,
                                           Efl_Gesture_Recognizer *recognizer)
{
   Eina_List *l, *l_next;
   Object_Gesture *object_gesture;
   const Efl_Event_Description *type;
   Efl_Gesture *dummy;

   if (!recognizer) return;

   // find the type of the recognizer
   dummy = efl_gesture_recognizer_create(recognizer, 0);
   if (!dummy)return;

   type = efl_gesture_type_get(dummy);
   efl_del(dummy);

   // check if its already registered
   recognizer = eina_hash_find(pd->m_recognizers, &type);

   if (!recognizer) return;

   // remove that gesture from the list of object gestures
   EINA_LIST_FOREACH_SAFE(pd->m_object_gestures, l, l_next, object_gesture)
     {
        if (object_gesture->type == type)
          {
             pd->m_gestures_to_delete = eina_list_append(pd->m_gestures_to_delete, object_gesture->gesture);
             free(object_gesture);
             pd->m_object_gestures = eina_list_remove_list(pd->m_object_gestures, l);
          }
     }
   eina_hash_del(pd->m_recognizers, &type, NULL);
}

// EOLIAN static void
// _efl_gesture_manager_ungrab_all(Eo *obj EINA_UNUSED, Efl_Gesture_Manager_Data *pd,
//                                    Eo *target)
// {
//    const Efl_Event_Description *type;
//    Eina_List *l;

//    Eina_List *gesture_type_list = eina_hash_find(pd->m_gesture_contex, &target);

//    if (!gesture_type_list) return;

//    EINA_LIST_FOREACH(gesture_type_list, l, type)
//      {
//         _cleanup_cached_gestures(obj, pd, target, type);
//      }
//    eina_hash_del(pd->m_gesture_contex, &target, NULL);
// }

// get or create a gesture object that will represent the state for a given object, used by the recognizer
Efl_Gesture*
_get_state(Efl_Gesture_Manager_Data *pd,
           Eo *target, Efl_Gesture_Recognizer *recognizer, const Efl_Event_Description *type)
{
   Eina_List *l;
   Object_Gesture *object_gesture;
   Efl_Gesture *gesture;

   // if the widget is being deleted we should be careful not to
   // create a new state.
   if (efl_destructed_is(target))
     return 0;

   //TODO find a optimized way of looking for gesture
   EINA_LIST_FOREACH(pd->m_object_gestures, l, object_gesture)
     {
        if (object_gesture->object == target &&
            object_gesture->recognizer == recognizer &&
            object_gesture->type   == type)
         {
            // the gesture is already processed waiting for cleanup
            if ((efl_gesture_state_get(object_gesture->gesture) == EFL_GESTURE_FINISHED) ||
                (efl_gesture_state_get(object_gesture->gesture) == EFL_GESTURE_CANCELED))
              return NULL;
            return object_gesture->gesture;
         }
     }

   gesture = efl_gesture_recognizer_create(recognizer, target);
   if (!gesture)
     return 0;
   object_gesture = calloc(1, sizeof(Object_Gesture));
   object_gesture->object = target;
   object_gesture->recognizer = recognizer;
   object_gesture->type = type;
   object_gesture->gesture = gesture;

   pd->m_object_gestures = eina_list_append(pd->m_object_gestures, object_gesture);

   return gesture;
}

static void
_cleanup_cached_gestures(Efl_Gesture_Manager_Data *pd,
                         Eo *target, const Efl_Event_Description *type)
{
   Eina_List *l, *l_next;
   Object_Gesture *object_gesture;

   EINA_LIST_FOREACH_SAFE(pd->m_object_gestures, l, l_next, object_gesture)
     {
        if ( (object_gesture->type == type) && (target == object_gesture->object))
          {
             pd->m_gestures_to_delete = eina_list_append(pd->m_gestures_to_delete, object_gesture->gesture);
             free(object_gesture);
             pd->m_object_gestures = eina_list_remove_list(pd->m_object_gestures, l);
          }
     }
}


#include "efl_gesture_manager.eo.c"
