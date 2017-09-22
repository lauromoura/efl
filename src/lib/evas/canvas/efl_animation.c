#include "efl_animation_private.h"

static void
_target_del_cb(void *data, const Efl_Event *event EINA_UNUSED)
{
   Eo *eo_obj = data;

   EFL_ANIMATION_CHECK_OR_RETURN(eo_obj);
   EFL_ANIMATION_DATA_GET(eo_obj, pd);

   pd->target = NULL;
}

EOLIAN static void
_efl_animation_target_set(Eo *eo_obj,
                          Efl_Animation_Data *pd,
                          Efl_Canvas_Object *target)
{
   EFL_ANIMATION_CHECK_OR_RETURN(eo_obj);

   efl_event_callback_add(target, EFL_EVENT_DEL, _target_del_cb, eo_obj);

   pd->target = target;
}

EOLIAN static Efl_Canvas_Object *
_efl_animation_target_get(Eo *eo_obj, Efl_Animation_Data *pd)
{
   EFL_ANIMATION_CHECK_OR_RETURN(eo_obj, NULL);

   return pd->target;
}

EOLIAN static void
_efl_animation_duration_set(Eo *eo_obj,
                            Efl_Animation_Data *pd,
                            double duration)
{
   EFL_ANIMATION_CHECK_OR_RETURN(eo_obj);

   efl_animation_total_duration_set(eo_obj, duration);

   pd->duration = duration;
}

EOLIAN static double
_efl_animation_duration_get(Eo *eo_obj, Efl_Animation_Data *pd)
{
   EFL_ANIMATION_CHECK_OR_RETURN(eo_obj, 0.0);

   return pd->duration;
}

EOLIAN static void
_efl_animation_duration_only_set(Eo *eo_obj,
                                 Efl_Animation_Data *pd,
                                 double duration)
{
   EFL_ANIMATION_CHECK_OR_RETURN(eo_obj);

   pd->duration = duration;
}

EOLIAN static void
_efl_animation_total_duration_set(Eo *eo_obj,
                                  Efl_Animation_Data *pd,
                                  double total_duration)
{
   EFL_ANIMATION_CHECK_OR_RETURN(eo_obj);

   pd->total_duration = total_duration;
}

EOLIAN static double
_efl_animation_total_duration_get(Eo *eo_obj, Efl_Animation_Data *pd)
{
   EFL_ANIMATION_CHECK_OR_RETURN(eo_obj, 0.0);

   return pd->total_duration;
}

EOLIAN static Eina_Bool
_efl_animation_is_deleted(Eo *eo_obj, Efl_Animation_Data *pd)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(eo_obj, EINA_TRUE);

   return pd->is_deleted;
}

EOLIAN static void
_efl_animation_final_state_keep_set(Eo *eo_obj,
                                    Efl_Animation_Data *pd,
                                    Eina_Bool keep_final_state)
{
   EFL_ANIMATION_CHECK_OR_RETURN(eo_obj);

   if (pd->keep_final_state == keep_final_state) return;

   pd->keep_final_state = !!keep_final_state;
}

EOLIAN static Eina_Bool
_efl_animation_final_state_keep_get(Eo *eo_obj, Efl_Animation_Data *pd)
{
   EFL_ANIMATION_CHECK_OR_RETURN(eo_obj, EINA_FALSE);

   return pd->keep_final_state;
}

EOLIAN static void
_efl_animation_repeat_mode_set(Eo *eo_obj,
                               Efl_Animation_Data *pd,
                               Efl_Animation_Repeat_Mode mode)
{
   EFL_ANIMATION_CHECK_OR_RETURN(eo_obj);

   if ((mode == EFL_ANIMATION_REPEAT_MODE_RESTART) ||
       (mode == EFL_ANIMATION_REPEAT_MODE_REVERSE))
     pd->repeat_mode = mode;
}

EOLIAN static Efl_Animation_Repeat_Mode
_efl_animation_repeat_mode_get(Eo *eo_obj, Efl_Animation_Data *pd)
{
   EFL_ANIMATION_CHECK_OR_RETURN(eo_obj,
                                 EFL_ANIMATION_REPEAT_MODE_RESTART);

   return pd->repeat_mode;
}

EOLIAN static void
_efl_animation_repeat_count_set(Eo *eo_obj,
                                Efl_Animation_Data *pd,
                                int count)
{
   EFL_ANIMATION_CHECK_OR_RETURN(eo_obj);

   //EFL_ANIMATION_REPEAT_INFINITE repeats animation infinitely
   if ((count < 0) && (count != EFL_ANIMATION_REPEAT_INFINITE)) return;

   pd->repeat_count = count;
}

EOLIAN static int
_efl_animation_repeat_count_get(Eo *eo_obj, Efl_Animation_Data *pd)
{
   EFL_ANIMATION_CHECK_OR_RETURN(eo_obj, 0);

   return pd->repeat_count;
}

EOLIAN static void
_efl_animation_start_delay_set(Eo *eo_obj,
                               Efl_Animation_Data *pd,
                               double delay_time)
{
   EFL_ANIMATION_CHECK_OR_RETURN(eo_obj);

   if (delay_time < 0.0) return;

   pd->start_delay_time = delay_time;
}

EOLIAN static double
_efl_animation_start_delay_get(Eo *eo_obj,
                               Efl_Animation_Data *pd)
{
   EFL_ANIMATION_CHECK_OR_RETURN(eo_obj, 0.0);

   return pd->start_delay_time;
}

EOLIAN static void
_efl_animation_interpolator_set(Eo *eo_obj,
                                Efl_Animation_Data *pd,
                                Efl_Interpolator *interpolator)
{
   EFL_ANIMATION_CHECK_OR_RETURN(eo_obj);

   pd->interpolator = interpolator;
}

EOLIAN static Efl_Interpolator *
_efl_animation_interpolator_get(Eo *eo_obj,
                                Efl_Animation_Data *pd)
{
   EFL_ANIMATION_CHECK_OR_RETURN(eo_obj, NULL);

   return pd->interpolator;
}

EOLIAN static Efl_Animation_Instance *
_efl_animation_instance_create(Eo *eo_obj, Efl_Animation_Data *pd EINA_UNUSED)
{
   EFL_ANIMATION_CHECK_OR_RETURN(eo_obj, NULL);

   Efl_Animation_Instance *instance
      = efl_add(EFL_ANIMATION_INSTANCE_CLASS, NULL);

   Efl_Canvas_Object *target = efl_animation_target_get(eo_obj);
   efl_animation_instance_target_set(instance, target);

   Eina_Bool state_keep = efl_animation_final_state_keep_get(eo_obj);
   efl_animation_instance_final_state_keep_set(instance, state_keep);

   double duration = efl_animation_duration_get(eo_obj);
   efl_animation_instance_duration_set(instance, duration);

   double total_duration = efl_animation_total_duration_get(eo_obj);
   efl_animation_instance_total_duration_set(instance, total_duration);

   double start_delay_time = efl_animation_start_delay_get(eo_obj);
   efl_animation_instance_start_delay_set(instance, start_delay_time);

   Efl_Animation_Instance_Repeat_Mode repeat_mode =
      (Efl_Animation_Instance_Repeat_Mode)efl_animation_repeat_mode_get(eo_obj);
   efl_animation_instance_repeat_mode_set(instance, repeat_mode);

   int repeat_count = efl_animation_repeat_count_get(eo_obj);
   efl_animation_instance_repeat_count_set(instance, repeat_count);

   Efl_Interpolator *interpolator = efl_animation_interpolator_get(eo_obj);
   efl_animation_instance_interpolator_set(instance, interpolator);

   return instance;
}

EOLIAN static Efl_Object *
_efl_animation_efl_object_constructor(Eo *eo_obj,
                                      Efl_Animation_Data *pd)
{
   eo_obj = efl_constructor(efl_super(eo_obj, MY_CLASS));

   pd->target = NULL;

   pd->duration = 0.0;
   pd->total_duration = 0.0;

   pd->start_delay_time = 0.0;

   pd->repeat_count = 0;

   pd->interpolator = NULL;

   pd->is_deleted = EINA_FALSE;
   pd->keep_final_state = EINA_FALSE;

   return eo_obj;
}

EOLIAN static void
_efl_animation_efl_object_destructor(Eo *eo_obj, Efl_Animation_Data *pd)
{
   pd->is_deleted = EINA_TRUE;

   if (pd->target)
     efl_event_callback_del(pd->target, EFL_EVENT_DEL, _target_del_cb, eo_obj);

   efl_destructor(efl_super(eo_obj, MY_CLASS));
}

#include "efl_animation.eo.c"
