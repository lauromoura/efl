#include "evas_common_private.h"
#include "evas_private.h"
#include <Ecore.h>

#define MY_CLASS EFL_ANIMATION_INSTANCE_CLASS
#define MY_CLASS_NAME efl_class_name_get(MY_CLASS)

#define EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN(inst, ...) \
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

#define EFL_ANIMATION_INSTANCE_DATA_GET(o, pd) \
   Evas_Object_Animation_Instance_Data *pd = efl_data_scope_get(o, EFL_ANIMATION_INSTANCE_CLASS)

typedef struct _Target_State
{
   int r, g, b, a;
} Target_State;

struct _Evas_Object_Animation_Instance_Data
{
   /*
    * FIXME: The following properties should be added.
    * Current Interpolation Function
    * Current State
    * State (State Change) Callbacks
    * Eina_Bool auto_delete;
    * int repeat_time;
    * int current_loop;               //Default value is 1
    * Eina_Bool is_reverse;
    */
   Ecore_Animator                    *animator;

   Ecore_Timer                       *start_delay_timer;
   double                             start_delay_time;

   struct
     {
        double begin;
        double current;
        double pause_begin;
     } time;

   Efl_Canvas_Object                 *target;
   Target_State                      *target_state;

   double                             progress;
   double                             duration;
   double                             paused_time;

   Efl_Animation_Instance_Repeat_Mode repeat_mode;
   int                                repeat_count;
   int                                remaining_repeat_count;

   Eina_Bool                          deleted : 1;
   Eina_Bool                          started : 1;
   Eina_Bool                          reverse_started : 1;
   Eina_Bool                          cancelled : 1;
   Eina_Bool                          ended : 1;
   Eina_Bool                          paused : 1;
   Eina_Bool                          state_keep : 1;
   Eina_Bool                          is_group_member : 1;
   Eina_Bool                          direction_forward : 1;
};

static void
_target_del_cb(void *data, const Efl_Event *event EINA_UNUSED)
{
   Eo *eo_obj = data;

   EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN(eo_obj);
   EFL_ANIMATION_INSTANCE_DATA_GET(eo_obj, pd);

   pd->target = NULL;
   efl_animation_instance_cancel(eo_obj);
}

EOLIAN static void
_efl_animation_instance_target_set(Eo *eo_obj,
                                   Evas_Object_Animation_Instance_Data *pd,
                                   Efl_Canvas_Object *target)
{
   EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN(eo_obj);

   efl_event_callback_add(target, EFL_EVENT_DEL, _target_del_cb, eo_obj);

   pd->target = target;
}

EOLIAN static Efl_Canvas_Object *
_efl_animation_instance_target_get(const Eo *eo_obj,
                                   Evas_Object_Animation_Instance_Data *pd)
{
   EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN((Eo *)eo_obj, NULL);

   return pd->target;
}


EOLIAN static void
_efl_animation_instance_duration_set(Eo *eo_obj,
                                     Evas_Object_Animation_Instance_Data *pd,
                                     double duration)
{
   EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN(eo_obj);

   //If duration is 0, then show the end state of the animation.
   if (duration < 0.0) return;

   pd->duration = duration;
}

EOLIAN static double
_efl_animation_instance_duration_get(const Eo *eo_obj,
                                     Evas_Object_Animation_Instance_Data *pd)
{
   EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN((Eo *)eo_obj, 0.0);

   return pd->duration;
}

EOLIAN static void
_efl_animation_instance_repeat_mode_set(Eo *eo_obj,
                                        Evas_Object_Animation_Instance_Data *pd,
                                        Efl_Animation_Instance_Repeat_Mode mode)
{
   EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN(eo_obj);

   if ((mode == EFL_ANIMATION_INSTANCE_REPEAT_MODE_RESTART) ||
       (mode == EFL_ANIMATION_INSTANCE_REPEAT_MODE_REVERSE))
     pd->repeat_mode = mode;
}

EOLIAN static Efl_Animation_Instance_Repeat_Mode
_efl_animation_instance_repeat_mode_get(const Eo *eo_obj,
                                        Evas_Object_Animation_Instance_Data *pd)
{
   EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN((Eo *)eo_obj, EFL_ANIMATION_INSTANCE_REPEAT_MODE_RESTART);

   return pd->repeat_mode;
}

EOLIAN static void
_efl_animation_instance_repeat_count_set(Eo *eo_obj,
                                         Evas_Object_Animation_Instance_Data *pd,
                                         int count)
{
   EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN(eo_obj);

   //-1 repeats animation infinitely
   if ((count < 0) && (count != -1)) return;

   pd->repeat_count = count;
}

EOLIAN static int
_efl_animation_instance_repeat_count_get(const Eo *eo_obj,
                                         Evas_Object_Animation_Instance_Data *pd)
{
   EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN((Eo *)eo_obj, 0);

   return pd->repeat_count;
}

EOLIAN static Eina_Bool
_efl_animation_instance_is_deleted(Eo *eo_obj,
                                   Evas_Object_Animation_Instance_Data *pd)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(eo_obj, EINA_TRUE);

   return pd->deleted;
}

EOLIAN static void
_efl_animation_instance_final_state_keep_set(Eo *eo_obj,
                                             Evas_Object_Animation_Instance_Data *pd,
                                             Eina_Bool state_keep)
{
   EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN(eo_obj);

   if (pd->state_keep == state_keep) return;

   pd->state_keep = !!state_keep;
}

EOLIAN static Eina_Bool
_efl_animation_instance_final_state_keep_get(const Eo *eo_obj,
                                             Evas_Object_Animation_Instance_Data *pd)
{
   EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN((Eo *)eo_obj, EINA_FALSE);

   return pd->state_keep;
}

static void
_target_state_save(Efl_Canvas_Object *target, Target_State *target_state)
{
   if (!target || !target_state) return;

   int r, g, b, a;
   evas_object_color_get(target, &r, &g, &b, &a);

   target_state->r = r;
   target_state->g = g;
   target_state->b = b;
   target_state->a = a;
}

static void
_target_state_restore(Efl_Canvas_Object *target, Target_State *target_state)
{
   if (!target || !target_state) return;

   int r, g, b, a;
   r = target_state->r;
   g = target_state->g;
   b = target_state->b;
   a = target_state->a;

   evas_object_color_set(target, r, g, b, a);

   if (efl_gfx_map_has(target))
     efl_gfx_map_reset(target);
}

static Eina_Bool
_animator_cb(void *data)
{
   Eo *eo_obj = data;
   EFL_ANIMATION_INSTANCE_DATA_GET(eo_obj, pd);

   if (pd->paused)
     {
        pd->animator = NULL;
        return ECORE_CALLBACK_CANCEL;
     }

   if (pd->cancelled) goto end;

   double duration = pd->duration;

   //If duration is 0, then show the end state of the animation.
   //Therefore, the animator should not be called again.
   if (duration == 0.0)
     {
        ecore_animator_del(pd->animator);
        pd->animator = NULL;
     }

   pd->time.current = ecore_loop_time_get();
   double elapsed_time = pd->time.current - pd->time.begin;
   double paused_time = pd->paused_time;

   //If duration is 0, then show the end state of the animation.
   if ((duration == 0.0) || ((elapsed_time - paused_time) > duration))
     {
        elapsed_time = duration + paused_time;

        if (pd->direction_forward)
          pd->progress = 1.0;
        else
          pd->progress = 0.0;
     }
   else
     {
        if (pd->direction_forward)
          pd->progress = (elapsed_time - paused_time) / duration;
        else
          pd->progress = 1.0 - ((elapsed_time - paused_time) / duration);
     }

   Efl_Animation_Instance_Animate_Event_Info event_info;
   event_info.progress = pd->progress;

   if (!pd->is_group_member)
     {
        //Reset previous map effect before apply map effect
        if (efl_gfx_map_has(pd->target))
          efl_gfx_map_reset(pd->target);
     }

   //pre animate event is supported within class only (protected event)
   efl_event_callback_call(eo_obj, EFL_ANIMATION_INSTANCE_EVENT_PRE_ANIMATE,
                           &event_info);
   efl_event_callback_call(eo_obj, EFL_ANIMATION_INSTANCE_EVENT_ANIMATE, &event_info);

  /* Not end. Keep going. */
   if ((elapsed_time - paused_time) < duration) return ECORE_CALLBACK_RENEW;

end:
   if (pd->repeat_count > 0)
     {
        pd->remaining_repeat_count--;
        if (pd->remaining_repeat_count >= 0)
          {
             pd->time.begin = ecore_loop_time_get();
             pd->paused_time = 0.0;

             if (pd->repeat_mode == EFL_ANIMATION_INSTANCE_REPEAT_MODE_REVERSE)
               pd->direction_forward = !pd->direction_forward;

             _target_state_restore(pd->target, pd->target_state);

             return ECORE_CALLBACK_RENEW;
          }
     }
   else if (pd->repeat_count == -1)
     {
        pd->time.begin = ecore_loop_time_get();
        pd->paused_time = 0.0;

        if (pd->repeat_mode == EFL_ANIMATION_INSTANCE_REPEAT_MODE_REVERSE)
          pd->direction_forward = !pd->direction_forward;

        _target_state_restore(pd->target, pd->target_state);

        return ECORE_CALLBACK_RENEW;
     }

   pd->ended = EINA_TRUE;
   pd->animator = NULL;

   //Restore initial state of target object
   if (!pd->state_keep)
     _target_state_restore(pd->target, pd->target_state);

   efl_event_callback_call(eo_obj, EFL_ANIMATION_INSTANCE_EVENT_END, NULL);
   //post end event is supported within class only (protected event)
   efl_event_callback_call(eo_obj, EFL_ANIMATION_INSTANCE_EVENT_POST_END, NULL);

   //FIXME: Delete animation here
   return ECORE_CALLBACK_CANCEL;
}

static void
_init_start(Eo *eo_obj, Evas_Object_Animation_Instance_Data *pd)
{
   //Save current state of target object
   _target_state_save(pd->target, pd->target_state);

   pd->cancelled = EINA_FALSE;
   pd->ended = EINA_FALSE;

   pd->paused_time = 0.0;

   pd->remaining_repeat_count = pd->repeat_count;

   if (pd->reverse_started)
     pd->direction_forward = EINA_FALSE;
   else
     pd->direction_forward = EINA_TRUE;

   ecore_animator_del(pd->animator);
   pd->animator = NULL;

   pd->time.begin = ecore_loop_time_get();

   pd->started = EINA_TRUE;

   //pre start event is supported within class only (protected event)
   efl_event_callback_call(eo_obj, EFL_ANIMATION_INSTANCE_EVENT_PRE_START, NULL);
   efl_event_callback_call(eo_obj, EFL_ANIMATION_INSTANCE_EVENT_START, NULL);
}

static void
_start(Eo *eo_obj, Evas_Object_Animation_Instance_Data *pd)
{
   _init_start(eo_obj, pd);

   pd->animator = ecore_animator_add(_animator_cb, eo_obj);

   _animator_cb(eo_obj);
}

static Eina_Bool
_start_delay_timer_cb(void *data)
{
   Eo *eo_obj = data;
   EFL_ANIMATION_INSTANCE_DATA_GET(eo_obj, pd);

   pd->start_delay_timer = NULL;

   if (pd->paused)
     {
        pd->time.pause_begin = ecore_loop_time_get();
        _init_start(eo_obj, pd);
     }
   else
     _start(eo_obj, pd);

   return ECORE_CALLBACK_CANCEL;
}

EOLIAN static void
_efl_animation_instance_start(Eo *eo_obj,
                              Evas_Object_Animation_Instance_Data *pd)
{
   EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN(eo_obj);

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
_efl_animation_instance_member_start(Eo *eo_obj,
                                     Evas_Object_Animation_Instance_Data *pd)
{
   EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN(eo_obj);

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
_efl_animation_instance_reverse_member_start(Eo *eo_obj,
                                             Evas_Object_Animation_Instance_Data *pd)
{
   EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN(eo_obj);

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
_efl_animation_instance_start_delay_set(Eo *eo_obj,
                                        Evas_Object_Animation_Instance_Data *pd,
                                        double delay_time)
{
   EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN(eo_obj);

   if (delay_time < 0.0) return;

   pd->start_delay_time = delay_time;
}

EOLIAN static double
_efl_animation_instance_start_delay_get(Eo *eo_obj,
                                        Evas_Object_Animation_Instance_Data *pd)
{
   EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN(eo_obj, 0.0);

   return pd->start_delay_time;
}

EOLIAN static void
_efl_animation_instance_cancel(Eo *eo_obj,
                               Evas_Object_Animation_Instance_Data *pd)
{
   EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN(eo_obj);

   ecore_timer_del(pd->start_delay_timer);
   pd->start_delay_timer = NULL;

   pd->cancelled = EINA_TRUE;

   if (pd->animator)
     {
        pd->ended = EINA_TRUE;

        ecore_animator_del(pd->animator);
        pd->animator = NULL;

        //Restore initial state of target object
        if (!pd->state_keep)
          _target_state_restore(pd->target, pd->target_state);

        efl_event_callback_call(eo_obj, EFL_ANIMATION_INSTANCE_EVENT_END, NULL);
        //post end event is supported within class only (protected event)
        efl_event_callback_call(eo_obj, EFL_ANIMATION_INSTANCE_EVENT_POST_END, NULL);
     }
}

EOLIAN static void
_efl_animation_instance_pause(Eo *eo_obj,
                              Evas_Object_Animation_Instance_Data *pd)
{
   EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN(eo_obj);

   if (!pd->started) return;

   if (pd->ended) return;

   if (pd->paused) return;
   pd->paused = EINA_TRUE;

   if (pd->start_delay_timer) return;

   ecore_animator_del(pd->animator);
   pd->animator = NULL;

   pd->time.pause_begin = ecore_loop_time_get();
}

EOLIAN static void
_efl_animation_instance_resume(Eo *eo_obj,
                               Evas_Object_Animation_Instance_Data *pd)
{
   EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN(eo_obj);

   if (!pd->started) return;

   if (pd->ended) return;

   if (!pd->paused) return;
   pd->paused = EINA_FALSE;

   if (pd->start_delay_timer) return;

   pd->paused_time += (ecore_loop_time_get() - pd->time.pause_begin);

   pd->animator = ecore_animator_add(_animator_cb, eo_obj);

   _animator_cb(eo_obj);
}

EOLIAN static void
_efl_animation_instance_map_reset(Eo *eo_obj,
                                  Evas_Object_Animation_Instance_Data *pd EINA_UNUSED)
{
   EFL_ANIMATION_INSTANCE_CHECK_OR_RETURN(eo_obj);

   Efl_Canvas_Object *target = efl_animation_instance_target_get(eo_obj);
   if (target)
     {
        if (efl_gfx_map_has(target))
          efl_gfx_map_reset(target);
     }
}

EOLIAN static Efl_Object *
_efl_animation_instance_efl_object_constructor(Eo *eo_obj,
                                               Evas_Object_Animation_Instance_Data *pd)
{
   eo_obj = efl_constructor(efl_super(eo_obj, MY_CLASS));

   pd->time.begin = 0.0;
   pd->time.current = 0.0;

   pd->target = NULL;
   pd->target_state = (Target_State *) calloc(1, sizeof(Target_State));

   pd->progress = 0.0;
   pd->duration = 0.0;

   pd->repeat_mode = EFL_ANIMATION_INSTANCE_REPEAT_MODE_RESTART;
   pd->repeat_count = 0;

   pd->deleted = EINA_FALSE;
   pd->cancelled = EINA_FALSE;
   pd->ended = EINA_FALSE;
   pd->state_keep = EINA_FALSE;

   return eo_obj;
}

EOLIAN static void
_efl_animation_instance_efl_object_destructor(Eo *eo_obj,
                                              Evas_Object_Animation_Instance_Data *pd)
{
   pd->deleted = EINA_TRUE;

   if (pd->animator)
     {
        ecore_animator_del(pd->animator);
        pd->animator = NULL;
        pd->ended = EINA_TRUE;

        //Restore initial state of target object
        if (!pd->state_keep)
          _target_state_restore(pd->target, pd->target_state);

        efl_event_callback_call(eo_obj, EFL_ANIMATION_INSTANCE_EVENT_END, NULL);
        //post end event is supported within class only (protected event)
        efl_event_callback_call(eo_obj, EFL_ANIMATION_INSTANCE_EVENT_POST_END, NULL);
     }

   if (pd->target)
     efl_event_callback_del(pd->target, EFL_EVENT_DEL, _target_del_cb, eo_obj);

   free(pd->target_state);
   pd->target_state = NULL;

   efl_destructor(efl_super(eo_obj, MY_CLASS));
}

/* Internal EO APIs */

EOAPI EFL_VOID_FUNC_BODYV(efl_animation_instance_target_set, EFL_FUNC_CALL(target), Efl_Canvas_Object *target);
EOAPI EFL_FUNC_BODY_CONST(efl_animation_instance_target_get, Efl_Canvas_Object *, NULL);

EOAPI EFL_VOID_FUNC_BODYV(efl_animation_instance_final_state_keep_set, EFL_FUNC_CALL(state_keep), Eina_Bool state_keep);
EOAPI EFL_FUNC_BODY_CONST(efl_animation_instance_final_state_keep_get, Eina_Bool, 0);

EOAPI EFL_VOID_FUNC_BODYV(efl_animation_instance_duration_set, EFL_FUNC_CALL(duration), double duration);
EOAPI EFL_FUNC_BODY_CONST(efl_animation_instance_duration_get, double, 0);

EOAPI EFL_VOID_FUNC_BODYV(efl_animation_instance_start_delay_set, EFL_FUNC_CALL(delay_time), double delay_time);
EOAPI EFL_FUNC_BODY_CONST(efl_animation_instance_start_delay_get, double, 0);

EOAPI EFL_VOID_FUNC_BODYV(efl_animation_instance_repeat_mode_set, EFL_FUNC_CALL(mode), Efl_Animation_Instance_Repeat_Mode mode);
EOAPI EFL_FUNC_BODY_CONST(efl_animation_instance_repeat_mode_get, Efl_Animation_Instance_Repeat_Mode, 0);

EOAPI EFL_VOID_FUNC_BODYV(efl_animation_instance_repeat_count_set, EFL_FUNC_CALL(count), int count);
EOAPI EFL_FUNC_BODY_CONST(efl_animation_instance_repeat_count_get, int, 0);

EOAPI EFL_FUNC_BODY(efl_animation_instance_is_deleted, Eina_Bool, 0);

EOAPI EFL_VOID_FUNC_BODY(efl_animation_instance_member_start);

EOAPI EFL_VOID_FUNC_BODY(efl_animation_instance_reverse_member_start);

EOAPI EFL_VOID_FUNC_BODY(efl_animation_instance_map_reset);

EOAPI EFL_VOID_FUNC_BODY(efl_animation_instance_final_state_show);

EWAPI const Efl_Event_Description _EFL_ANIMATION_INSTANCE_EVENT_PRE_START =
   EFL_EVENT_DESCRIPTION("pre_start");
EWAPI const Efl_Event_Description _EFL_ANIMATION_INSTANCE_EVENT_PRE_ANIMATE =
   EFL_EVENT_DESCRIPTION("pre_animate");
EWAPI const Efl_Event_Description _EFL_ANIMATION_INSTANCE_EVENT_POST_END =
   EFL_EVENT_DESCRIPTION("post_end");

#define EFL_ANIMATION_INSTANCE_EXTRA_OPS \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_target_set, _efl_animation_instance_target_set), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_target_get, _efl_animation_instance_target_get), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_final_state_keep_set, _efl_animation_instance_final_state_keep_set), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_final_state_keep_get, _efl_animation_instance_final_state_keep_get), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_duration_set, _efl_animation_instance_duration_set), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_duration_get, _efl_animation_instance_duration_get), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_start_delay_set, _efl_animation_instance_start_delay_set), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_start_delay_get, _efl_animation_instance_start_delay_get), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_repeat_mode_set, _efl_animation_instance_repeat_mode_set), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_repeat_mode_get, _efl_animation_instance_repeat_mode_get), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_repeat_count_set, _efl_animation_instance_repeat_count_set), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_repeat_count_get, _efl_animation_instance_repeat_count_get), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_is_deleted, _efl_animation_instance_is_deleted), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_member_start, _efl_animation_instance_member_start), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_reverse_member_start, _efl_animation_instance_reverse_member_start), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_map_reset, _efl_animation_instance_map_reset), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_final_state_show, NULL)

#include "efl_animation_instance.eo.c"
