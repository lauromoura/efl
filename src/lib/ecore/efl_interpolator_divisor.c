#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "Ecore.h"
#include "ecore_private.h"

#define MY_CLASS EFL_INTERPOLATOR_DIVISOR_CLASS

typedef struct _Efl_Interpolator_Divisor_Data Efl_Interpolator_Divisor_Data;

struct _Efl_Interpolator_Divisor_Data
{
   double factor[2];
};

EOLIAN static double
_efl_interpolator_divisor_efl_interpolator_interpolate(Eo *eo_obj EINA_UNUSED,
                                                       Efl_Interpolator_Divisor_Data *pd EINA_UNUSED,
                                                       double progress)
{
   if ((progress < 0.0) || (progress > 1.0))
     return progress;

   return ecore_animator_pos_map(progress, ECORE_POS_MAP_DIVISOR_INTERP,
                                 pd->factor[0], pd->factor[1]);
}

EOLIAN static void
_efl_interpolator_divisor_factors_set(Eo *eo_obj EINA_UNUSED,
                                      Efl_Interpolator_Divisor_Data *pd,
                                      double factor1, double factor2)
{
   pd->factor[0] = factor1;
   pd->factor[1] = factor2;
}

EOLIAN static void
_efl_interpolator_divisor_factors_get(Eo *eo_obj EINA_UNUSED,
                                      Efl_Interpolator_Divisor_Data *pd,
                                      double *factor1, double *factor2)
{
   if (factor1)
     *factor1 = pd->factor[0];

   if (factor2)
     *factor2 = pd->factor[1];
}

EOLIAN static Efl_Object *
_efl_interpolator_divisor_efl_object_constructor(Eo *eo_obj,
                                                 Efl_Interpolator_Divisor_Data *pd)
{
   eo_obj = efl_constructor(efl_super(eo_obj, MY_CLASS));

   pd->factor[0] = 1.0;
   pd->factor[1] = 1.0;

   return eo_obj;
}

#include "efl_interpolator_divisor.eo.c"
