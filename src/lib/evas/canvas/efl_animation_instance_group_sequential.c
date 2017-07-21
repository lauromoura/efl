#include "evas_common_private.h"
#include "evas_private.h"
#include <Ecore.h>

#define MY_CLASS EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_CLASS
#define MY_CLASS_NAME efl_class_name_get(MY_CLASS)

#define EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_CHECK_OR_RETURN(inst, ...) \
   do { \
      if (!inst) { \
         CRI("Efl_Animation_Instance " # inst " is NULL!"); \
         return __VA_ARGS__; \
      } \
      if (efl_animation_instance_is_deleted(inst)) { \
         ERR("Efl_Animation_Instance " # inst " has already been deleted!"); \
         return __VA_ARGS__; \
      } \
   } while (0)

#define EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_DATA_GET(o, pd) \
   Evas_Object_Animation_Instance_Group_Sequential_Data *pd = efl_data_scope_get(o, EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_CLASS)

struct _Evas_Object_Animation_Instance_Group_Sequential_Data
{
   Ecore_Timer    *start_delay_timer;
   double          start_delay_time;

   int             current_index;

   int             remaining_repeat_count;

   Eina_List      *finished_inst_list;

   Eina_Bool       started : 1;
   Eina_Bool       reverse_started : 1;
   Eina_Bool       paused : 1;
   Eina_Bool       is_group_member : 1;
   Eina_Bool       direction_forward : 1;
};

static void _index_animation_start(Eo *eo_obj, int index, Eina_Bool direction_forward);

static void
_pre_animate_cb(void *data, const Efl_Event *event)
{
   Eo *eo_obj = data;

   EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_DATA_GET(eo_obj, pd);

   //pre animate event is supported within class only (protected event)
   efl_event_callback_call(eo_obj, EFL_ANIMATION_INSTANCE_EVENT_PRE_ANIMATE,
                           event->info);

   if (!pd->is_group_member)
     {
        efl_animation_instance_map_reset(event->object);

        if (efl_animation_instance_final_state_keep_get(eo_obj))
          {
             Eina_List *l;
             Efl_Animation_Instance *inst;

             EINA_LIST_FOREACH(pd->finished_inst_list, l, inst)
               {
                  efl_animation_instance_final_state_show(inst);
               }
          }
     }
}

static void
_post_end_cb(void *data, const Efl_Event *event)
{
   Eo *eo_obj = data;

   EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_DATA_GET(eo_obj, pd);

   if (pd->direction_forward)
     {
        //Save finished instances
        if (!eina_list_data_find(pd->finished_inst_list, event->object))
          {
             pd->finished_inst_list
                = eina_list_append(pd->finished_inst_list, event->object);
          }
     }
   else
     {
        /* Remove the finished instance from the finished instance list.
         * In this case, the finished instance is group instance.
         */
        pd->finished_inst_list =
           eina_list_remove(pd->finished_inst_list, event->object);

        /* The last finished instance should be removed from the
         * finished instance list because the last finished
         * instance will be animated from the next animation
         * frame by calling reverse_member_start().
         * However, group animation instances may have more than
         * one member animation instances. So the member
         * animation instances except the last member animation
         * instance should be remained as finished instances.
         */
        Efl_Animation_Instance *last_inst = NULL;

        Eina_List *last_inst_list =
           eina_list_last(pd->finished_inst_list);
        if (last_inst_list)
          last_inst = eina_list_data_get(last_inst_list);

        if (last_inst)
          {
             const char *inst_class = efl_class_name_get(last_inst);
             const char *group_class = "Efl.Animation.Instance.Group";
             int class_len = strlen(group_class);
             if (!inst_class || strncmp(inst_class, group_class, class_len))
               {
                  pd->finished_inst_list =
                     eina_list_remove(pd->finished_inst_list, last_inst);
               }
          }
     }

   //member post end event is supported within class only (protected event)
   efl_event_callback_call(eo_obj,
                           EFL_ANIMATION_INSTANCE_GROUP_EVENT_MEMBER_POST_END,
                           NULL);

   if (pd->direction_forward)
     pd->current_index++;
   else
     pd->current_index--;

   Eina_List *instances = efl_animation_instance_group_instances_get(eo_obj);

   Eina_Bool group_finished = EINA_FALSE;
   if (pd->direction_forward)
     {
        if ((int)eina_list_count(instances) == pd->current_index)
          group_finished = EINA_TRUE;
     }
   else
     {
        if (pd->current_index < 0)
          group_finished = EINA_TRUE;
     }

   if (group_finished)
     {
        int repeat_count = efl_animation_instance_repeat_count_get(eo_obj);
        if (repeat_count > 0)
          {
             pd->remaining_repeat_count--;
             if (pd->remaining_repeat_count >= 0)
               {
                  if (efl_animation_instance_repeat_mode_get(eo_obj) ==
                      EFL_ANIMATION_INSTANCE_REPEAT_MODE_REVERSE)
                    pd->direction_forward = !pd->direction_forward;

                  if (pd->direction_forward)
                    {
                       eina_list_free(pd->finished_inst_list);
                       pd->finished_inst_list = NULL;

                       pd->current_index = 0;
                    }
                  else
                    {
                       /* The last finished instance should be removed from the
                        * finished instance list because the last finished
                        * instance will be animated from the next animation
                        * frame by calling reverse_member_start().
                        * However, group animation instances may have more than
                        * one member animation instances. So the member
                        * animation instances except the last member animation
                        * instance should be remained as finished instances.
                        */
                       Efl_Animation_Instance *last_inst = NULL;

                       Eina_List *last_inst_list =
                          eina_list_last(pd->finished_inst_list);
                       if (last_inst_list)
                         last_inst = eina_list_data_get(last_inst_list);

                       if (last_inst)
                         {
                            const char *inst_class = efl_class_name_get(last_inst);
                            const char *group_class = "Efl.Animation.Instance.Group";
                            int class_len = strlen(group_class);
                            if (!inst_class || strncmp(inst_class, group_class, class_len))
                              {
                                 pd->finished_inst_list =
                                    eina_list_remove(pd->finished_inst_list, last_inst);
                              }
                         }
                       pd->current_index = eina_list_count(instances) - 1;
                    }
                  goto next_start;
               }
          }
        else if (repeat_count == -1)
          {
             if (efl_animation_instance_repeat_mode_get(eo_obj) ==
                 EFL_ANIMATION_INSTANCE_REPEAT_MODE_REVERSE)
               pd->direction_forward = !pd->direction_forward;

             if (pd->direction_forward)
               {
                  eina_list_free(pd->finished_inst_list);
                  pd->finished_inst_list = NULL;

                  pd->current_index = 0;
               }
             else
               {
                  /* The last finished instance should be removed from the
                   * finished instance list because the last finished
                   * instance will be animated from the next animation
                   * frame by calling reverse_member_start().
                   * However, group animation instances may have more than
                   * one member animation instances. So the member
                   * animation instances except the last member animation
                   * instance should be remained as finished instances.
                   */
                  Efl_Animation_Instance *last_inst = NULL;

                  Eina_List *last_inst_list =
                     eina_list_last(pd->finished_inst_list);
                  if (last_inst_list)
                    last_inst = eina_list_data_get(last_inst_list);

                  if (last_inst)
                    {
                       const char *inst_class = efl_class_name_get(last_inst);
                       const char *group_class = "Efl.Animation.Instance.Group";
                       int class_len = strlen(group_class);
                       if (!inst_class || strncmp(inst_class, group_class, class_len))
                         {
                            pd->finished_inst_list =
                               eina_list_remove(pd->finished_inst_list, last_inst);
                         }
                    }
                  pd->current_index = eina_list_count(instances) - 1;
               }
             goto next_start;
          }

        efl_event_callback_call(eo_obj, EFL_ANIMATION_INSTANCE_EVENT_END, NULL);
        //post end event is supported within class only (protected event)
        efl_event_callback_call(eo_obj, EFL_ANIMATION_INSTANCE_EVENT_POST_END,
                                NULL);
        return;
     }

next_start:
   _index_animation_start(eo_obj, pd->current_index, pd->direction_forward);
}

static void
_member_post_end_cb(void *data, const Efl_Event *event)
{
   Eo *eo_obj = data;

   EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_DATA_GET(eo_obj, pd);

   //Save finished instances
   if (!eina_list_data_find(pd->finished_inst_list, event->object))
     {
        pd->finished_inst_list
           = eina_list_append(pd->finished_inst_list, event->object);
     }
   /* The finished instance in this function is a group instance.
    * Since a group instance should not be removed before the group instance is
    * finally finished, the finished instance should be removed only in
    * _post_end_cb() function.
    */

   //member post end event is supported within class only (protected event)
   efl_event_callback_call(eo_obj,
                           EFL_ANIMATION_INSTANCE_GROUP_EVENT_MEMBER_POST_END,
                           NULL);
}

static void
_index_animation_start(Eo *eo_obj, int index, Eina_Bool direction_forward)
{
   Efl_Animation_Instance *inst =
      eina_list_nth(efl_animation_instance_group_instances_get(eo_obj), index);
   if (!inst || efl_animation_instance_is_deleted(inst))
     return;

   if (direction_forward)
     efl_animation_instance_member_start(inst);
   else
     efl_animation_instance_reverse_member_start(inst);
}

static void
_init_start(Eo *eo_obj, Evas_Object_Animation_Instance_Group_Sequential_Data *pd)
{
   pd->started = EINA_TRUE;

   pd->remaining_repeat_count = efl_animation_instance_repeat_count_get(eo_obj);

   eina_list_free(pd->finished_inst_list);
   pd->finished_inst_list = NULL;

   if (pd->reverse_started)
     {
        Eina_List *instances =
           efl_animation_instance_group_instances_get(eo_obj);
        pd->finished_inst_list = eina_list_clone(instances);

        /* The last finished instance should be removed from the
         * finished instance list because the last finished
         * instance will be animated from the next animation
         * frame by calling reverse_member_start().
         * However, group animation instances may have more than
         * one member animation instances. So the member
         * animation instances except the last member animation
         * instance should be remained as finished instances.
         */
        Efl_Animation_Instance *last_inst = NULL;

        Eina_List *last_inst_list =
           eina_list_last(pd->finished_inst_list);
        if (last_inst_list)
          last_inst = eina_list_data_get(last_inst_list);

        if (last_inst)
          {
             const char *inst_class = efl_class_name_get(last_inst);
             const char *group_class = "Efl.Animation.Instance.Group";
             int class_len = strlen(group_class);
             if (!inst_class || strncmp(inst_class, group_class, class_len))
               {
                  pd->finished_inst_list =
                     eina_list_remove(pd->finished_inst_list, last_inst);
               }
          }
        pd->current_index = eina_list_count(instances) - 1;

        pd->direction_forward = EINA_FALSE;
     }
   else
     {
        pd->current_index = 0;

        pd->direction_forward = EINA_TRUE;
     }

   efl_event_callback_call(eo_obj, EFL_ANIMATION_INSTANCE_EVENT_START, NULL);
}

static void
_start(Eo *eo_obj, Evas_Object_Animation_Instance_Group_Sequential_Data *pd)
{
   _init_start(eo_obj, pd);

   _index_animation_start(eo_obj, pd->current_index, pd->direction_forward);
}

static Eina_Bool
_start_delay_timer_cb(void *data)
{
   Eo *eo_obj = data;
   EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_DATA_GET(eo_obj, pd);

   pd->start_delay_timer = NULL;

   if (pd->paused)
     _init_start(eo_obj, pd);
   else
     _start(eo_obj, pd);

   return ECORE_CALLBACK_CANCEL;
}

EOLIAN static void
_efl_animation_instance_group_sequential_efl_animation_instance_start(Eo *eo_obj,
                                                                      Evas_Object_Animation_Instance_Group_Sequential_Data *pd)
{
   EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_CHECK_OR_RETURN(eo_obj);

   if (pd->paused) return;

   if (pd->start_delay_timer) return;

   pd->is_group_member = EINA_FALSE;
   pd->reverse_started = EINA_FALSE;

   if (pd->start_delay_time > 0.0)
     {
        pd->start_delay_timer = ecore_timer_add(pd->start_delay_time,
                                                _start_delay_timer_cb, eo_obj);
        return;
     }

   _start(eo_obj, pd);
}

EOLIAN static void
_efl_animation_instance_group_sequential_efl_animation_instance_member_start(Eo *eo_obj,
                                                                             Evas_Object_Animation_Instance_Group_Sequential_Data *pd)
{
   EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_CHECK_OR_RETURN(eo_obj);

   if (pd->paused) return;

   if (pd->start_delay_timer) return;

   pd->is_group_member = EINA_TRUE;
   pd->reverse_started = EINA_FALSE;

   if (pd->start_delay_time > 0.0)
     {
        pd->start_delay_timer = ecore_timer_add(pd->start_delay_time,
                                                _start_delay_timer_cb, eo_obj);
        return;
     }

   _start(eo_obj, pd);
}

EOLIAN static void
_efl_animation_instance_group_sequential_efl_animation_instance_reverse_member_start(Eo *eo_obj,
                                                                                     Evas_Object_Animation_Instance_Group_Sequential_Data *pd)
{
   EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_CHECK_OR_RETURN(eo_obj);

   if (pd->paused) return;

   if (pd->start_delay_timer) return;

   pd->is_group_member = EINA_TRUE;
   pd->reverse_started = EINA_TRUE;

   if (pd->start_delay_time > 0.0)
     {
        pd->start_delay_timer = ecore_timer_add(pd->start_delay_time,
                                                _start_delay_timer_cb, eo_obj);
        return;
     }

   _start(eo_obj, pd);
}

EOLIAN static void
_efl_animation_instance_group_sequential_efl_animation_instance_start_delay_set(Eo *eo_obj,
                                                                                Evas_Object_Animation_Instance_Group_Sequential_Data *pd,
                                                                                double delay_time)
{
   EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_CHECK_OR_RETURN(eo_obj);

   if (delay_time < 0.0) return;

   pd->start_delay_time = delay_time;
}

EOLIAN static double
_efl_animation_instance_group_sequential_efl_animation_instance_start_delay_get(Eo *eo_obj,
                                                                                Evas_Object_Animation_Instance_Group_Sequential_Data *pd)
{
   EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_CHECK_OR_RETURN(eo_obj, 0.0);

   return pd->start_delay_time;
}

EOLIAN static void
_efl_animation_instance_group_sequential_efl_animation_instance_pause(Eo *eo_obj,
                                                                      Evas_Object_Animation_Instance_Group_Sequential_Data *pd)
{
   EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_CHECK_OR_RETURN(eo_obj);

   if (!pd->started) return;

   if (pd->paused) return;
   pd->paused = EINA_TRUE;

   if (pd->start_delay_timer) return;

   Eina_List *inst_list = efl_animation_instance_group_instances_get(eo_obj);
   Eina_List *l;
   Efl_Animation_Instance *inst;

   EINA_LIST_FOREACH(inst_list, l, inst)
     {
        efl_animation_instance_pause(inst);
     }
}

EOLIAN static void
_efl_animation_instance_group_sequential_efl_animation_instance_resume(Eo *eo_obj,
                                                                       Evas_Object_Animation_Instance_Group_Sequential_Data *pd)
{
   EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_CHECK_OR_RETURN(eo_obj);

   if (!pd->started) return;

   if (!pd->paused) return;
   pd->paused = EINA_FALSE;

   if (pd->start_delay_timer) return;

   Eina_List *inst_list = efl_animation_instance_group_instances_get(eo_obj);
   Eina_List *l;
   Efl_Animation_Instance *inst;

   EINA_LIST_FOREACH(inst_list, l, inst)
     {
        efl_animation_instance_resume(inst);
     }
}

EOLIAN static void
_efl_animation_instance_group_sequential_efl_animation_instance_group_instance_add(Eo *eo_obj,
                                                                                   Evas_Object_Animation_Instance_Group_Sequential_Data *pd EINA_UNUSED,
                                                                                   Efl_Animation_Instance *instance)
{
   EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_CHECK_OR_RETURN(eo_obj);

   if (!instance) return;

   //pre animate event is supported within class only (protected event)
   efl_event_callback_add(instance, EFL_ANIMATION_INSTANCE_EVENT_PRE_ANIMATE,
                          _pre_animate_cb, eo_obj);

   //post end event is supported within class only (protected event)
   efl_event_callback_add(instance, EFL_ANIMATION_INSTANCE_EVENT_POST_END,
                          _post_end_cb, eo_obj);

   //member post end event is supported within class only (protected event)
   efl_event_callback_add(instance,
                          EFL_ANIMATION_INSTANCE_GROUP_EVENT_MEMBER_POST_END,
                          _member_post_end_cb, eo_obj);

   efl_animation_instance_group_instance_add(efl_super(eo_obj, MY_CLASS), instance);
}

EOLIAN static void
_efl_animation_instance_group_sequential_efl_animation_instance_group_instance_del(Eo *eo_obj,
                                                                                   Evas_Object_Animation_Instance_Group_Sequential_Data *pd EINA_UNUSED,
                                                                                   Efl_Animation_Instance *instance)
{
   EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_CHECK_OR_RETURN(eo_obj);

   if (!instance) return;

   efl_event_callback_del(instance, EFL_ANIMATION_INSTANCE_EVENT_PRE_ANIMATE,
                          _pre_animate_cb, eo_obj);

   efl_event_callback_del(instance, EFL_ANIMATION_INSTANCE_EVENT_POST_END,
                          _post_end_cb, eo_obj);

   efl_event_callback_del(instance,
                          EFL_ANIMATION_INSTANCE_GROUP_EVENT_MEMBER_POST_END,
                          _member_post_end_cb, eo_obj);

   efl_animation_instance_group_instance_del(efl_super(eo_obj, MY_CLASS), instance);
}

EOLIAN static void
_efl_animation_instance_group_sequential_efl_animation_instance_final_state_show(Eo *eo_obj,
                                                                                 Evas_Object_Animation_Instance_Group_Sequential_Data *pd)
{
   EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_CHECK_OR_RETURN(eo_obj);

   Eina_List *l;
   Efl_Animation *inst;
   EINA_LIST_FOREACH(pd->finished_inst_list, l, inst)
     {
        efl_animation_instance_final_state_show(inst);
     }
}

/* Internal EO APIs */

#define EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_EXTRA_OPS \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_member_start, _efl_animation_instance_group_sequential_efl_animation_instance_member_start), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_reverse_member_start, _efl_animation_instance_group_sequential_efl_animation_instance_reverse_member_start), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_final_state_show, _efl_animation_instance_group_sequential_efl_animation_instance_final_state_show), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_group_instance_add, _efl_animation_instance_group_sequential_efl_animation_instance_group_instance_add), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_group_instance_del, _efl_animation_instance_group_sequential_efl_animation_instance_group_instance_del), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_start_delay_set, _efl_animation_instance_group_sequential_efl_animation_instance_start_delay_set), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_start_delay_get, _efl_animation_instance_group_sequential_efl_animation_instance_start_delay_get)

#include "efl_animation_instance_group_sequential.eo.c"
