#include "efl_animation_object_private.h"

static void
_target_del_cb(void *data, const Efl_Event *event EINA_UNUSED)
{
   Eo *eo_obj = data;

   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN(eo_obj);
   EFL_ANIMATION_OBJECT_DATA_GET(eo_obj, pd);

   pd->target = NULL;
   efl_animation_object_cancel(eo_obj);
}

EOLIAN static void
_efl_animation_object_target_set(Eo *eo_obj,
                                 Efl_Animation_Object_Data *pd,
                                 Efl_Canvas_Object *target)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN(eo_obj);

   if (pd->target == target) return;

   if (pd->target)
     efl_event_callback_del(pd->target, EFL_EVENT_DEL, _target_del_cb, eo_obj);

   efl_event_callback_add(target, EFL_EVENT_DEL, _target_del_cb, eo_obj);

   pd->target = target;
}

EOLIAN static Efl_Canvas_Object *
_efl_animation_object_target_get(Eo *eo_obj, Efl_Animation_Object_Data *pd)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN((Eo *)eo_obj, NULL);

   return pd->target;
}

EOLIAN static void
_efl_animation_object_duration_set(Eo *eo_obj,
                                   Efl_Animation_Object_Data *pd,
                                   double duration)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN(eo_obj);

   efl_animation_object_total_duration_set(eo_obj, duration);

   pd->duration = duration;
}

EOLIAN static double
_efl_animation_object_duration_get(Eo *eo_obj,
                                   Efl_Animation_Object_Data *pd)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN((Eo *)eo_obj, 0.0);

   return pd->duration;
}

EOLIAN static void
_efl_animation_object_duration_only_set(Eo *eo_obj,
                                        Efl_Animation_Object_Data *pd,
                                        double duration)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN(eo_obj);

   pd->duration = duration;
}

EOLIAN static void
_efl_animation_object_total_duration_set(Eo *eo_obj,
                                         Efl_Animation_Object_Data *pd,
                                         double total_duration)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN(eo_obj);

   pd->total_duration = total_duration;
}

EOLIAN static double
_efl_animation_object_total_duration_get(Eo *eo_obj, Efl_Animation_Object_Data *pd)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN(eo_obj, 0.0);

   return pd->total_duration;
}

EOLIAN static void
_efl_animation_object_repeat_count_set(Eo *eo_obj,
                                       Efl_Animation_Object_Data *pd,
                                       int count)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN(eo_obj);

   //EFL_ANIMATION_OBJECT_REPEAT_INFINITE repeats animation infinitely
   if ((count < 0) && (count != EFL_ANIMATION_OBJECT_REPEAT_INFINITE)) return;

   pd->repeat_count = count;
}

EOLIAN static int
_efl_animation_object_repeat_count_get(const Eo *eo_obj,
                                       Efl_Animation_Object_Data *pd)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN((Eo *)eo_obj, 0);

   return pd->repeat_count;
}

EOLIAN static void
_efl_animation_object_start_delay_set(Eo *eo_obj,
                                      Efl_Animation_Object_Data *pd,
                                      double delay_time)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN(eo_obj);

   if (delay_time < 0.0) return;

   pd->start_delay_time = delay_time;
}

EOLIAN static double
_efl_animation_object_start_delay_get(Eo *eo_obj,
                                      Efl_Animation_Object_Data *pd)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN(eo_obj, 0.0);

   return pd->start_delay_time;
}


EOLIAN static Eina_Bool
_efl_animation_object_is_deleted(Eo *eo_obj,
                                 Efl_Animation_Object_Data *pd)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(eo_obj, EINA_TRUE);

   return pd->is_deleted;
}

EOLIAN static void
_efl_animation_object_target_state_save(Eo *eo_obj,
                                        Efl_Animation_Object_Data *pd)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN(eo_obj);

   if (!pd->target || !pd->target_state) return;

   Evas_Coord x, y, w, h;
   evas_object_geometry_get(pd->target, &x, &y, &w, &h);

   pd->target_state->x = x;
   pd->target_state->y = y;
   pd->target_state->w = w;
   pd->target_state->h = h;

   int r, g, b, a;
   evas_object_color_get(pd->target, &r, &g, &b, &a);

   pd->target_state->r = r;
   pd->target_state->g = g;
   pd->target_state->b = b;
   pd->target_state->a = a;

   Evas_Map *map = evas_map_dup(evas_object_map_get(pd->target));
   pd->target_state->map = map;

   Eina_Bool enable_map = evas_object_map_enable_get(pd->target);
   pd->target_state->enable_map = enable_map;
}

EOLIAN static void
_efl_animation_object_target_state_reset(Eo *eo_obj,
                                         Efl_Animation_Object_Data *pd)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN(eo_obj);

   if (!pd->target) return;

   if (efl_gfx_map_has(pd->target))
     efl_gfx_map_reset(pd->target);

   if (!pd->target_state) return;

   Evas_Coord x, y, w, h;
   x = pd->target_state->x;
   y = pd->target_state->y;
   w = pd->target_state->w;
   h = pd->target_state->h;

   evas_object_resize(pd->target, w, h);
   evas_object_move(pd->target, x, y);

   int r, g, b, a;
   r = pd->target_state->r;
   g = pd->target_state->g;
   b = pd->target_state->b;
   a = pd->target_state->a;

   evas_object_color_set(pd->target, r, g, b, a);

   Evas_Map *map = pd->target_state->map;
   evas_object_map_set(pd->target, map);

   Eina_Bool enable_map = pd->target_state->enable_map;
   evas_object_map_enable_set(pd->target, enable_map);
}

EOLIAN static void
_efl_animation_object_target_map_reset(Eo *eo_obj,
                                       Efl_Animation_Object_Data *pd)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN(eo_obj);

   if (!pd->target) return;

   if (efl_gfx_map_has(pd->target))
     efl_gfx_map_reset(pd->target);
}

EOLIAN static void
_efl_animation_object_final_state_keep_set(Eo *eo_obj,
                                           Efl_Animation_Object_Data *pd,
                                           Eina_Bool keep_final_state)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN(eo_obj);

   if (pd->keep_final_state == keep_final_state) return;

   pd->keep_final_state = !!keep_final_state;
}

EOLIAN static Eina_Bool
_efl_animation_object_final_state_keep_get(Eo *eo_obj,
                                           Efl_Animation_Object_Data *pd)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN((Eo *)eo_obj, EINA_FALSE);

   return pd->keep_final_state;
}

static Eina_Bool
_animator_cb(void *data)
{
   Eo *eo_obj = data;
   EFL_ANIMATION_OBJECT_DATA_GET(eo_obj, pd);

   if (pd->is_paused)
     {
        pd->animator = NULL;
        return ECORE_CALLBACK_CANCEL;
     }

   if (pd->is_cancelled) goto end;

   double paused_time = pd->paused_time;

   double total_duration = pd->total_duration;

   pd->time.current = ecore_loop_time_get();
   double elapsed_time = pd->time.current - pd->time.begin;

   if ((elapsed_time - paused_time) > total_duration)
     elapsed_time = total_duration + paused_time;

   if (total_duration < 0.0) goto end;

   if (total_duration == 0.0)
     pd->progress = 1.0;
   else
     pd->progress = (elapsed_time - paused_time) / total_duration;

   //Reset previous animation effect before applying animation effect
   /* FIXME: When the target state is saved, it may not be finished to calculate
    * target geometry.
    * In this case, incorrect geometry is saved and the target moves to the
    * incorrect position when animation is paused and resumed.
    * As a result, flicking issue happens.
    * To avoid the flicking issue, reset map only during animation. */
   efl_animation_object_target_map_reset(eo_obj);

   efl_animation_object_progress_set(eo_obj, pd->progress);

   //Not end. Keep going.
   if ((elapsed_time - paused_time) < total_duration)
     return ECORE_CALLBACK_RENEW;

end:
   //Repeat animation
   if ((pd->repeat_count == EFL_ANIMATION_OBJECT_REPEAT_INFINITE) ||
       (pd->remaining_repeat_count > 0))
     {
        if (pd->remaining_repeat_count > 0)
          pd->remaining_repeat_count--;

        pd->time.begin = ecore_loop_time_get();
        pd->paused_time = 0.0;

        efl_animation_object_target_state_reset(eo_obj);

        return ECORE_CALLBACK_RENEW;
     }

   pd->is_ended = EINA_TRUE;
   pd->animator = NULL;

   //Reset the state of the target to the initial state
   if (!pd->keep_final_state)
     efl_animation_object_target_state_reset(eo_obj);

   efl_event_callback_call(eo_obj, EFL_ANIMATION_OBJECT_EVENT_END, NULL);

   //FIXME: Delete object here
   return ECORE_CALLBACK_CANCEL;
}

static void
_init_start(Eo *eo_obj, Efl_Animation_Object_Data *pd)
{
   //Save the current state of the target
   efl_animation_object_target_state_save(eo_obj);

   pd->is_started = EINA_TRUE;
   pd->is_cancelled = EINA_FALSE;
   pd->is_ended = EINA_FALSE;

   pd->paused_time = 0.0;

   pd->remaining_repeat_count = pd->repeat_count;

   ecore_animator_del(pd->animator);
   pd->animator = NULL;

   pd->time.begin = ecore_loop_time_get();

   //pre start event is supported within class only (protected event)
   efl_event_callback_call(eo_obj, EFL_ANIMATION_OBJECT_EVENT_PRE_START, NULL);
   efl_event_callback_call(eo_obj, EFL_ANIMATION_OBJECT_EVENT_START, NULL);
}

static void
_start(Eo *eo_obj, Efl_Animation_Object_Data *pd)
{
   if (pd->total_duration < 0.0) return;

   _init_start(eo_obj, pd);

   pd->animator = ecore_animator_add(_animator_cb, eo_obj);

   _animator_cb(eo_obj);
}

static Eina_Bool
_start_delay_timer_cb(void *data)
{
   Eo *eo_obj = data;
   EFL_ANIMATION_OBJECT_DATA_GET(eo_obj, pd);

   pd->start_delay_timer = NULL;

   if (pd->is_paused)
     {
        pd->time.pause_begin = ecore_loop_time_get();
        _init_start(eo_obj, pd);
     }
   else
     _start(eo_obj, pd);

   return ECORE_CALLBACK_CANCEL;
}

EOLIAN static void
_efl_animation_object_start(Eo *eo_obj,
                            Efl_Animation_Object_Data *pd)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN(eo_obj);

   if (pd->is_paused) return;

   if (pd->start_delay_timer) return;

   if (pd->start_delay_time > 0.0)
     {
        pd->start_delay_timer = ecore_timer_add(pd->start_delay_time,
                                                _start_delay_timer_cb, eo_obj);
        return;
     }

   _start(eo_obj, pd);
}

EOLIAN static void
_efl_animation_object_cancel(Eo *eo_obj,
                             Efl_Animation_Object_Data *pd)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN(eo_obj);

   ecore_timer_del(pd->start_delay_timer);
   pd->start_delay_timer = NULL;

   pd->is_cancelled = EINA_TRUE;
   pd->is_ended = EINA_TRUE;

   if (pd->animator)
     {
        ecore_animator_del(pd->animator);
        pd->animator = NULL;

        //Reset the state of the target to the initial state
        if (!pd->keep_final_state)
          efl_animation_object_target_state_reset(eo_obj);

        efl_event_callback_call(eo_obj, EFL_ANIMATION_OBJECT_EVENT_END, NULL);
     }
}

EOLIAN static void
_efl_animation_object_progress_set(Eo *eo_obj,
                                   Efl_Animation_Object_Data *pd EINA_UNUSED,
                                   double progress)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN(eo_obj);

   if ((progress < 0.0) || (progress > 1.0)) return;

   Efl_Animation_Object_Animate_Event_Info event_info;
   event_info.progress = progress;

   efl_event_callback_call(eo_obj, EFL_ANIMATION_OBJECT_EVENT_ANIMATE,
                           &event_info);
}

EOLIAN static void
_efl_animation_object_pause(Eo *eo_obj,
                            Efl_Animation_Object_Data *pd)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN(eo_obj);

   if (!pd->is_started) return;
   if (pd->is_ended) return;
   if (pd->is_paused) return;

   pd->is_paused = EINA_TRUE;

   if (pd->start_delay_timer) return;

   ecore_animator_del(pd->animator);
   pd->animator = NULL;

   pd->time.pause_begin = ecore_loop_time_get();
}

EOLIAN static void
_efl_animation_object_resume(Eo *eo_obj,
                             Efl_Animation_Object_Data *pd)
{
   EFL_ANIMATION_OBJECT_CHECK_OR_RETURN(eo_obj);

   if (!pd->is_started) return;
   if (pd->is_ended) return;
   if (!pd->is_paused) return;

   pd->is_paused = EINA_FALSE;

   if (pd->start_delay_timer) return;

   pd->paused_time += (ecore_loop_time_get() - pd->time.pause_begin);

   pd->animator = ecore_animator_add(_animator_cb, eo_obj);

   _animator_cb(eo_obj);
}

EOLIAN static Efl_Object *
_efl_animation_object_efl_object_constructor(Eo *eo_obj,
                                             Efl_Animation_Object_Data *pd)
{
   eo_obj = efl_constructor(efl_super(eo_obj, MY_CLASS));

   pd->time.begin = 0.0;
   pd->time.current = 0.0;

   pd->target = NULL;
   pd->target_state = (Target_State *) calloc(1, sizeof(Target_State));

   pd->progress = 0.0;

   pd->duration = 0.0;
   pd->total_duration = 0.0;

   pd->repeat_count = 0;

   pd->is_deleted = EINA_FALSE;
   pd->is_cancelled = EINA_FALSE;
   pd->keep_final_state = EINA_FALSE;

   return eo_obj;
}

EOLIAN static void
_efl_animation_object_efl_object_destructor(Eo *eo_obj,
                                            Efl_Animation_Object_Data *pd)
{
   pd->is_deleted = EINA_TRUE;

   if (pd->animator)
     {
        ecore_animator_del(pd->animator);
        pd->animator = NULL;

        //Reset the state of the target to the initial state
        if (!pd->keep_final_state)
          efl_animation_object_target_state_reset(eo_obj);

        efl_event_callback_call(eo_obj, EFL_ANIMATION_OBJECT_EVENT_END, NULL);
     }

   if (pd->target)
     efl_event_callback_del(pd->target, EFL_EVENT_DEL, _target_del_cb, eo_obj);

   free(pd->target_state);
   pd->target_state = NULL;

   efl_destructor(efl_super(eo_obj, MY_CLASS));
}

/* Internal EO APIs */

EOAPI EFL_VOID_FUNC_BODYV(efl_animation_object_target_set, EFL_FUNC_CALL(target), Efl_Canvas_Object *target);
EOAPI EFL_FUNC_BODY_CONST(efl_animation_object_target_get, Efl_Canvas_Object *, NULL);

EOAPI EFL_VOID_FUNC_BODYV(efl_animation_object_final_state_keep_set, EFL_FUNC_CALL(state_keep), Eina_Bool state_keep);
EOAPI EFL_FUNC_BODY_CONST(efl_animation_object_final_state_keep_get, Eina_Bool, 0);

EOAPI EFL_VOID_FUNC_BODYV(efl_animation_object_duration_set, EFL_FUNC_CALL(duration), double duration);
EOAPI EFL_FUNC_BODY_CONST(efl_animation_object_duration_get, double, 0);

EOAPI EFL_VOID_FUNC_BODYV(efl_animation_object_duration_only_set, EFL_FUNC_CALL(duration), double duration);

EOAPI EFL_VOID_FUNC_BODYV(efl_animation_object_total_duration_set, EFL_FUNC_CALL(total_duration), double total_duration);
EOAPI EFL_FUNC_BODY_CONST(efl_animation_object_total_duration_get, double, 0);

EOAPI EFL_VOID_FUNC_BODYV(efl_animation_object_start_delay_set, EFL_FUNC_CALL(delay_time), double delay_time);
EOAPI EFL_FUNC_BODY_CONST(efl_animation_object_start_delay_get, double, 0);

EOAPI EFL_VOID_FUNC_BODYV(efl_animation_object_repeat_count_set, EFL_FUNC_CALL(count), int count);
EOAPI EFL_FUNC_BODY_CONST(efl_animation_object_repeat_count_get, int, 0);

#define EFL_ANIMATION_OBJECT_EXTRA_OPS \
   EFL_OBJECT_OP_FUNC(efl_animation_object_target_set, _efl_animation_object_target_set), \
   EFL_OBJECT_OP_FUNC(efl_animation_object_target_get, _efl_animation_object_target_get), \
   EFL_OBJECT_OP_FUNC(efl_animation_object_final_state_keep_set, _efl_animation_object_final_state_keep_set), \
   EFL_OBJECT_OP_FUNC(efl_animation_object_final_state_keep_get, _efl_animation_object_final_state_keep_get), \
   EFL_OBJECT_OP_FUNC(efl_animation_object_duration_set, _efl_animation_object_duration_set), \
   EFL_OBJECT_OP_FUNC(efl_animation_object_duration_get, _efl_animation_object_duration_get), \
   EFL_OBJECT_OP_FUNC(efl_animation_object_duration_only_set, _efl_animation_object_duration_only_set), \
   EFL_OBJECT_OP_FUNC(efl_animation_object_total_duration_set, _efl_animation_object_total_duration_set), \
   EFL_OBJECT_OP_FUNC(efl_animation_object_total_duration_get, _efl_animation_object_total_duration_get), \
   EFL_OBJECT_OP_FUNC(efl_animation_object_start_delay_set, _efl_animation_object_start_delay_set), \
   EFL_OBJECT_OP_FUNC(efl_animation_object_start_delay_get, _efl_animation_object_start_delay_get), \
   EFL_OBJECT_OP_FUNC(efl_animation_object_repeat_count_set, _efl_animation_object_repeat_count_set), \
   EFL_OBJECT_OP_FUNC(efl_animation_object_repeat_count_get, _efl_animation_object_repeat_count_get)

EWAPI const Efl_Event_Description _EFL_ANIMATION_OBJECT_EVENT_PRE_START =
   EFL_EVENT_DESCRIPTION("pre_start");

#include "efl_animation_object.eo.c"
