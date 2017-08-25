#include "efl_animation_instance_translate_private.h"

EOLIAN static void
_efl_animation_instance_translate_translate_set(Eo *eo_obj,
                                                Efl_Animation_Instance_Translate_Data *pd,
                                                Evas_Coord from_x,
                                                Evas_Coord from_y,
                                                Evas_Coord to_x,
                                                Evas_Coord to_y)
{
   EFL_ANIMATION_INSTANCE_TRANSLATE_CHECK_OR_RETURN(eo_obj);

   pd->from.move_x = from_x;
   pd->from.move_y = from_y;

   pd->to.move_x = to_x;
   pd->to.move_y = to_y;

   //Update absolute coordinate based on relative move
   Evas_Coord x = 0;
   Evas_Coord y = 0;

   Efl_Canvas_Object *target = efl_animation_instance_target_get(eo_obj);
   if (target)
     evas_object_geometry_get(target, &x, &y, NULL, NULL);

   pd->from.x = pd->from.move_x + x;
   pd->from.y = pd->from.move_y + y;

   pd->to.x = pd->to.move_x + x;
   pd->to.y = pd->to.move_y + y;

   pd->use_rel_move = EINA_TRUE;
}

EOLIAN static void
_efl_animation_instance_translate_translate_get(Eo *eo_obj,
                                                Efl_Animation_Instance_Translate_Data *pd,
                                                Evas_Coord *from_x,
                                                Evas_Coord *from_y,
                                                Evas_Coord *to_x,
                                                Evas_Coord *to_y)
{
   EFL_ANIMATION_INSTANCE_TRANSLATE_CHECK_OR_RETURN(eo_obj);

   //Update relative move based on absolute coordinate
   if (!pd->use_rel_move)
     {
        Evas_Coord x = 0;
        Evas_Coord y = 0;

        Efl_Canvas_Object *target = efl_animation_instance_target_get(eo_obj);
        if (target)
          evas_object_geometry_get(target, &x, &y, NULL, NULL);

        pd->from.move_x = pd->from.x - x;
        pd->from.move_y = pd->from.y - y;

        pd->to.move_x = pd->to.x - x;
        pd->to.move_y = pd->to.y - y;
     }

   if (from_x)
     *from_x = pd->from.move_x;
   if (from_y)
     *from_y = pd->from.move_y;

   if (to_x)
     *to_x = pd->to.move_x;
   if (to_y)
     *to_y = pd->to.move_y;
}

EOLIAN static void
_efl_animation_instance_translate_translate_absolute_set(Eo *eo_obj,
                                                         Efl_Animation_Instance_Translate_Data *pd,
                                                         Evas_Coord from_x,
                                                         Evas_Coord from_y,
                                                         Evas_Coord to_x,
                                                         Evas_Coord to_y)
{
   EFL_ANIMATION_INSTANCE_TRANSLATE_CHECK_OR_RETURN(eo_obj);

   pd->from.x = from_x;
   pd->from.y = from_y;

   pd->to.x = to_x;
   pd->to.y = to_y;

   //Update relative move based on absolute coordinate
   Evas_Coord x = 0;
   Evas_Coord y = 0;

   Efl_Canvas_Object *target = efl_animation_instance_target_get(eo_obj);
   if (target)
     evas_object_geometry_get(target, &x, &y, NULL, NULL);

   pd->from.move_x = pd->from.x - x;
   pd->from.move_y = pd->from.y - y;

   pd->to.move_x = pd->to.x - x;
   pd->to.move_y = pd->to.y - y;

   pd->use_rel_move = EINA_FALSE;
}

EOLIAN static void
_efl_animation_instance_translate_translate_absolute_get(Eo *eo_obj,
                                                         Efl_Animation_Instance_Translate_Data *pd,
                                                         Evas_Coord *from_x,
                                                         Evas_Coord *from_y,
                                                         Evas_Coord *to_x,
                                                         Evas_Coord *to_y)
{
   EFL_ANIMATION_INSTANCE_TRANSLATE_CHECK_OR_RETURN(eo_obj);

   //Update absolute coordinate based on relative move
   if (pd->use_rel_move)
     {
        Evas_Coord x = 0;
        Evas_Coord y = 0;

        Efl_Canvas_Object *target = efl_animation_instance_target_get(eo_obj);
        if (target)
          evas_object_geometry_get(target, &x, &y, NULL, NULL);

        pd->from.x = pd->from.move_x + x;
        pd->from.y = pd->from.move_y + y;

        pd->to.x = pd->to.move_x + x;
        pd->to.y = pd->to.move_y + y;
     }

   if (from_x)
     *from_x = pd->from.x;
   if (from_y)
     *from_y = pd->from.y;

   if (to_x)
     *to_x = pd->to.x;
   if (to_y)
     *to_y = pd->to.y;
}

EOLIAN static void
_efl_animation_instance_translate_efl_animation_instance_duration_set(Eo *eo_obj,
                                                                      Efl_Animation_Instance_Translate_Data *pd EINA_UNUSED,
                                                                      double duration)
{
   EFL_ANIMATION_INSTANCE_TRANSLATE_CHECK_OR_RETURN(eo_obj);

   /* For a single animation instance, duration should be equal to or bigger
    * than 0.0. */
   if (duration < 0.0) return;

   //For a single animation instance, total duration is the same as duration.
   efl_animation_instance_total_duration_set(eo_obj, duration);

   efl_animation_instance_duration_set(efl_super(eo_obj, MY_CLASS), duration);
}

static void
_pre_start_cb(void *data EINA_UNUSED, const Efl_Event *event)
{
   EFL_ANIMATION_INSTANCE_TRANSLATE_DATA_GET(event->object, pd);

   pd->start_x = 0;
   pd->start_y = 0;

   Efl_Canvas_Object *target = efl_animation_instance_target_get(event->object);
   if (!target) return;

   Evas_Coord x, y;
   evas_object_geometry_get(target, &x, &y, NULL, NULL);

   pd->start_x = x;
   pd->start_y = y;
}

static void
_progress_set(Eo *eo_obj, double progress)
{
   EFL_ANIMATION_INSTANCE_TRANSLATE_DATA_GET(eo_obj, pd);

   Efl_Canvas_Object *target = efl_animation_instance_target_get(eo_obj);
   if (!target) return;

   Evas_Coord new_x = 0;
   Evas_Coord new_y = 0;

   if (pd->use_rel_move)
     {
        Evas_Coord move_x, move_y;

        move_x
           = (pd->from.move_x * (1.0 - progress)) + (pd->to.move_x * progress);
        move_y
           = (pd->from.move_y * (1.0 - progress)) + (pd->to.move_y * progress);

        new_x = pd->start_x + move_x;
        new_y = pd->start_y + move_y;
     }
   else
     {
        new_x = (pd->from.x * (1.0 - progress)) + (pd->to.x * progress);
        new_y = (pd->from.y * (1.0 - progress)) + (pd->to.y * progress);
     }

   evas_object_move(target, new_x, new_y);
}

EOLIAN static void
_efl_animation_instance_translate_efl_animation_instance_progress_set(Eo *eo_obj,
                                                                      Efl_Animation_Instance_Translate_Data *pd EINA_UNUSED,
                                                                      double progress)
{
   EFL_ANIMATION_INSTANCE_TRANSLATE_CHECK_OR_RETURN(eo_obj);

   if ((progress < 0.0) || (progress > 1.0)) return;

   _progress_set(eo_obj, progress);
}

EOLIAN static Efl_Object *
_efl_animation_instance_translate_efl_object_constructor(Eo *eo_obj,
                                                         Efl_Animation_Instance_Translate_Data *pd)
{
   eo_obj = efl_constructor(efl_super(eo_obj, MY_CLASS));

   pd->from.move_x = 0;
   pd->from.move_y = 0;
   pd->from.x = 0;
   pd->from.y = 0;

   pd->to.move_x = 0;
   pd->to.move_y = 0;
   pd->to.x = 0;
   pd->to.y = 0;

   pd->start_x = 0;
   pd->start_y = 0;

   pd->use_rel_move = EINA_TRUE;

   //pre start event is supported within class only (protected event)
   efl_event_callback_add(eo_obj, EFL_ANIMATION_INSTANCE_EVENT_PRE_START,
                          _pre_start_cb, NULL);

   return eo_obj;
}

/* Internal EO APIs */

EOAPI EFL_VOID_FUNC_BODYV(efl_animation_instance_translate_set, EFL_FUNC_CALL(from_x, from_y, to_x, to_y), int from_x, int from_y, int to_x, int to_y);
EOAPI EFL_VOID_FUNC_BODYV_CONST(efl_animation_instance_translate_get, EFL_FUNC_CALL(from_x, from_y, to_x, to_y), int *from_x, int *from_y, int *to_x, int *to_y);

EOAPI EFL_VOID_FUNC_BODYV(efl_animation_instance_translate_absolute_set, EFL_FUNC_CALL(from_x, from_y, to_x, to_y), int from_x, int from_y, int to_x, int to_y);
EOAPI EFL_VOID_FUNC_BODYV_CONST(efl_animation_instance_translate_absolute_get, EFL_FUNC_CALL(from_x, from_y, to_x, to_y), int *from_x, int *from_y, int *to_x, int *to_y);

#define EFL_ANIMATION_INSTANCE_TRANSLATE_EXTRA_OPS \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_translate_set, _efl_animation_instance_translate_translate_set), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_translate_get, _efl_animation_instance_translate_translate_get), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_translate_absolute_set, _efl_animation_instance_translate_translate_absolute_set), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_translate_absolute_get, _efl_animation_instance_translate_translate_absolute_get), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_duration_set, _efl_animation_instance_translate_efl_animation_instance_duration_set)

#include "efl_animation_instance_translate.eo.c"
