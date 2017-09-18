#include "efl_animation_alpha_private.h"

EOLIAN static void
_efl_animation_alpha_alpha_set(Eo *eo_obj,
                               Efl_Animation_Alpha_Data *pd,
                               double from_alpha,
                               double to_alpha)
{
   EFL_ANIMATION_ALPHA_CHECK_OR_RETURN(eo_obj);

   pd->from.alpha = from_alpha;
   pd->to.alpha = to_alpha;
}

EOLIAN static void
_efl_animation_alpha_alpha_get(Eo *eo_obj EINA_UNUSED,
                               Efl_Animation_Alpha_Data *pd,
                               double *from_alpha,
                               double *to_alpha)
{
   EFL_ANIMATION_ALPHA_CHECK_OR_RETURN(eo_obj);

   if (from_alpha)
     *from_alpha = pd->from.alpha;
   if (to_alpha)
     *to_alpha = pd->to.alpha;
}

EOLIAN static void
_efl_animation_alpha_efl_animation_duration_set(Eo *eo_obj,
                                                Efl_Animation_Alpha_Data *pd EINA_UNUSED,
                                                double duration)
{
   EFL_ANIMATION_ALPHA_CHECK_OR_RETURN(eo_obj);

   //For a single animation, duration should be equal to or bigger than 0.0.
   if (duration < 0.0) return;

   //For a single animation, total duration is the same as duration.
   efl_animation_total_duration_set(eo_obj, duration);

   efl_animation_duration_set(efl_super(eo_obj, MY_CLASS), duration);
}

EOLIAN static Efl_Animation_Instance *
_efl_animation_alpha_efl_animation_instance_create(Eo *eo_obj,
                                                   Efl_Animation_Alpha_Data *pd)
{
   EFL_ANIMATION_ALPHA_CHECK_OR_RETURN(eo_obj, NULL);

   Efl_Animation_Instance_Alpha *instance
      = efl_add(EFL_ANIMATION_INSTANCE_ALPHA_CLASS, NULL);

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

   efl_animation_instance_alpha_set(instance, pd->from.alpha, pd->to.alpha);

   return instance;
}

EOLIAN static Efl_Object *
_efl_animation_alpha_efl_object_constructor(Eo *eo_obj,
                                            Efl_Animation_Alpha_Data *pd)
{
   eo_obj = efl_constructor(efl_super(eo_obj, MY_CLASS));

   pd->from.alpha = 1.0;
   pd->to.alpha = 1.0;

   return eo_obj;
}

#include "efl_animation_alpha.eo.c"
