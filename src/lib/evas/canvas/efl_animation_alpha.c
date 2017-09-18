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
