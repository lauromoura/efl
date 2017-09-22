#include "efl_animation_rotate_private.h"

EOLIAN static void
_efl_animation_rotate_rotate_set(Eo *eo_obj,
                                 Efl_Animation_Rotate_Data *pd,
                                 double from_degree,
                                 double to_degree,
                                 Efl_Canvas_Object *pivot,
                                 double cx,
                                 double cy)
{
   EFL_ANIMATION_ROTATE_CHECK_OR_RETURN(eo_obj);

   pd->from.degree = from_degree;
   pd->to.degree = to_degree;

   pd->rel_pivot.obj = pivot;
   pd->rel_pivot.cx = cx;
   pd->rel_pivot.cy = cy;

   //Update absolute pivot based on relative pivot
   Evas_Coord x = 0;
   Evas_Coord y = 0;
   Evas_Coord w = 0;
   Evas_Coord h = 0;

   if (pivot)
     evas_object_geometry_get(pivot, &x, &y, &w, &h);
   else
     {
        Efl_Canvas_Object *target = efl_animation_target_get(eo_obj);
        if (target)
          evas_object_geometry_get(target, &x, &y, &w, &h);
     }

   pd->abs_pivot.cx = x + (w * cx);
   pd->abs_pivot.cy = y + (h * cy);

   pd->use_rel_pivot = EINA_TRUE;
}

EOLIAN static void
_efl_animation_rotate_rotate_get(Eo *eo_obj,
                                 Efl_Animation_Rotate_Data *pd,
                                 double *from_degree,
                                 double *to_degree,
                                 Efl_Canvas_Object **pivot,
                                 double *cx,
                                 double *cy)
{
   EFL_ANIMATION_ROTATE_CHECK_OR_RETURN(eo_obj);

   //Update relative pivot based on absolute pivot
   if (!pd->use_rel_pivot)
     {
        Evas_Coord x = 0;
        Evas_Coord y = 0;
        Evas_Coord w = 0;
        Evas_Coord h = 0;

        Efl_Canvas_Object *target = efl_animation_target_get(eo_obj);
        if (target)
          evas_object_geometry_get(target, &x, &y, &w, &h);

        if (w != 0)
          pd->rel_pivot.cx = (double)(pd->abs_pivot.cx - x) / w;
        else
          pd->rel_pivot.cx = 0.0;

        if (h != 0)
          pd->rel_pivot.cy = (double)(pd->abs_pivot.cy - y) / h;
        else
          pd->rel_pivot.cy = 0.0;
     }

   if (from_degree)
     *from_degree = pd->from.degree;

   if (to_degree)
     *to_degree = pd->to.degree;

   if (pivot)
     *pivot = pd->rel_pivot.obj;

   if (cx)
     *cx = pd->rel_pivot.cx;

   if (cy)
     *cy = pd->rel_pivot.cy;
}

EOLIAN static void
_efl_animation_rotate_rotate_absolute_set(Eo *eo_obj,
                                          Efl_Animation_Rotate_Data *pd,
                                          double from_degree,
                                          double to_degree,
                                          Evas_Coord cx,
                                          Evas_Coord cy)
{
   EFL_ANIMATION_ROTATE_CHECK_OR_RETURN(eo_obj);

   pd->from.degree = from_degree;
   pd->to.degree = to_degree;

   pd->abs_pivot.cx = cx;
   pd->abs_pivot.cy = cy;

   //Update relative pivot based on absolute pivot
   Evas_Coord x = 0;
   Evas_Coord y = 0;
   Evas_Coord w = 0;
   Evas_Coord h = 0;

   Efl_Canvas_Object *target = efl_animation_target_get(eo_obj);
   if (target)
     evas_object_geometry_get(target, &x, &y, &w, &h);

   pd->rel_pivot.obj = NULL;

   if (w != 0)
     pd->rel_pivot.cx = (double)(cx - x) / w;
   else
     pd->rel_pivot.cx = 0.0;

   if (h != 0)
     pd->rel_pivot.cy = (double)(cy - y) / h;
   else
     pd->rel_pivot.cy = 0.0;

   pd->use_rel_pivot = EINA_FALSE;
}

EOLIAN static void
_efl_animation_rotate_rotate_absolute_get(Eo *eo_obj,
                                          Efl_Animation_Rotate_Data *pd,
                                          double *from_degree,
                                          double *to_degree,
                                          Evas_Coord *cx,
                                          Evas_Coord *cy)
{
   EFL_ANIMATION_ROTATE_CHECK_OR_RETURN(eo_obj);

   //Update relative pivot based on absolute pivot
   if (pd->use_rel_pivot)
     {
        Evas_Coord x = 0;
        Evas_Coord y = 0;
        Evas_Coord w = 0;
        Evas_Coord h = 0;

        if (pd->rel_pivot.obj)
          evas_object_geometry_get(pd->rel_pivot.obj, &x, &y, &w, &h);
        else
          {
             Efl_Canvas_Object *target = efl_animation_target_get(eo_obj);
             if (target)
               evas_object_geometry_get(target, &x, &y, &w, &h);
          }

        pd->abs_pivot.cx = x + (w * pd->rel_pivot.cx);
        pd->abs_pivot.cy = y + (h * pd->rel_pivot.cy);
     }

   if (from_degree)
     *from_degree = pd->from.degree;

   if (to_degree)
     *to_degree = pd->to.degree;

   if (cx)
     *cx = pd->abs_pivot.cx;

   if (cy)
     *cy = pd->abs_pivot.cy;
}

EOLIAN static void
_efl_animation_rotate_efl_animation_duration_set(Eo *eo_obj,
                                                 Efl_Animation_Rotate_Data *pd EINA_UNUSED,
                                                 double duration)
{
   EFL_ANIMATION_ROTATE_CHECK_OR_RETURN(eo_obj);

   //For a single animation, duration should be equal to or bigger than 0.0.
   if (duration < 0.0) return;

   efl_animation_duration_set(efl_super(eo_obj, MY_CLASS), duration);
}

EOLIAN static Efl_Animation_Instance *
_efl_animation_rotate_efl_animation_instance_create(Eo *eo_obj,
                                                    Efl_Animation_Rotate_Data *pd)
{
   EFL_ANIMATION_ROTATE_CHECK_OR_RETURN(eo_obj, NULL);

   Efl_Animation_Instance_Rotate *instance
      = efl_add(EFL_ANIMATION_INSTANCE_ROTATE_CLASS, NULL);

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

   if (pd->use_rel_pivot)
     {
        efl_animation_instance_rotate_set(instance,
                                          pd->from.degree,
                                          pd->to.degree,
                                          pd->rel_pivot.obj,
                                          pd->rel_pivot.cx,
                                          pd->rel_pivot.cy);
     }
   else
     {
        efl_animation_instance_rotate_absolute_set(instance,
                                                   pd->from.degree,
                                                   pd->to.degree,
                                                   pd->abs_pivot.cx,
                                                   pd->abs_pivot.cy);
     }

   return instance;
}

EOLIAN static Efl_Object *
_efl_animation_rotate_efl_object_constructor(Eo *eo_obj,
                                             Efl_Animation_Rotate_Data *pd)
{
   eo_obj = efl_constructor(efl_super(eo_obj, MY_CLASS));

   pd->from.degree = 0.0;
   pd->to.degree = 0.0;

   pd->rel_pivot.obj = NULL;
   pd->rel_pivot.cx = 0.5;
   pd->rel_pivot.cy = 0.5;

   pd->abs_pivot.cx = 0;
   pd->abs_pivot.cy = 0;

   pd->use_rel_pivot = EINA_TRUE;

   return eo_obj;
}

#include "efl_animation_rotate.eo.c"
