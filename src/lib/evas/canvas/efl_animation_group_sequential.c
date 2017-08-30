#include "efl_animation_group_sequential_private.h"

EOLIAN static void
_efl_animation_group_sequential_efl_animation_group_animation_add(Eo *eo_obj,
                                                                  Efl_Animation_Group_Sequential_Data *pd EINA_UNUSED,
                                                                  Efl_Animation *animation)
{
   EFL_ANIMATION_GROUP_SEQUENTIAL_CHECK_OR_RETURN(eo_obj);

   if (!animation) return;

   efl_animation_group_animation_add(efl_super(eo_obj, MY_CLASS),
                                     animation);

   /* Total duration is calculated in efl_animation_total_duration_get() based
    * on the current group animation list.
    * Therefore, the calculated total duration should be set to update total
    * duration. */
   double total_duration =
      efl_animation_total_duration_get(eo_obj);
   efl_animation_total_duration_set(eo_obj, total_duration);
}

EOLIAN static void
_efl_animation_group_sequential_efl_animation_group_animation_del(Eo *eo_obj,
                                                                  Efl_Animation_Group_Sequential_Data *pd EINA_UNUSED,
                                                                  Efl_Animation *animation)
{
   EFL_ANIMATION_GROUP_SEQUENTIAL_CHECK_OR_RETURN(eo_obj);

   if (!animation) return;

   efl_animation_group_animation_del(efl_super(eo_obj, MY_CLASS),
                                     animation);

   /* Total duration is calculated in efl_animation_total_duration_get() based
    * on the current group animation list.
    * Therefore, the calculated total duration should be set to update total
    * duration. */
   double total_duration =
      efl_animation_total_duration_get(eo_obj);
   efl_animation_total_duration_set(eo_obj, total_duration);
}

EOLIAN static double
_efl_animation_group_sequential_efl_animation_total_duration_get(Eo *eo_obj,
                                                                 Efl_Animation_Group_Sequential_Data *pd EINA_UNUSED)
{
   EFL_ANIMATION_GROUP_SEQUENTIAL_CHECK_OR_RETURN(eo_obj, 0.0);

   Eina_List *animations =
      efl_animation_group_animations_get(eo_obj);
   if (!animations) return 0.0;

   double total_duration = 0.0;
   Eina_List *l;
   Efl_Animation *anim;
   EINA_LIST_FOREACH(animations, l, anim)
     {
        total_duration += efl_animation_total_duration_get(anim);
     }
   return total_duration;
}

EOLIAN static Efl_Animation_Instance *
_efl_animation_group_sequential_efl_animation_instance_create(Eo *eo_obj,
                                                              Efl_Animation_Group_Sequential_Data *pd EINA_UNUSED)
{
   EFL_ANIMATION_GROUP_SEQUENTIAL_CHECK_OR_RETURN(eo_obj, NULL);

   Efl_Animation_Instance_Group_Sequential *group_inst
      = efl_add(EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_CLASS, NULL);

   Eina_List *animations = efl_animation_group_animations_get(eo_obj);
   Eina_List *l;
   Efl_Animation *child_anim;
   Efl_Animation_Instance *child_inst;

   EINA_LIST_FOREACH(animations, l, child_anim)
     {
        child_inst = efl_animation_instance_create(child_anim);
        efl_animation_instance_group_instance_add(group_inst, child_inst);
     }

   Efl_Canvas_Object *target = efl_animation_target_get(eo_obj);
   if (target)
     efl_animation_instance_target_set(group_inst, target);

   Eina_Bool state_keep = efl_animation_final_state_keep_get(eo_obj);
   efl_animation_instance_final_state_keep_set(group_inst, state_keep);

   double duration = efl_animation_duration_get(eo_obj);
   efl_animation_instance_duration_set(group_inst, duration);

   double total_duration = efl_animation_total_duration_get(eo_obj);
   efl_animation_instance_total_duration_set(group_inst, total_duration);

   return group_inst;
}

#include "efl_animation_group_sequential.eo.c"
