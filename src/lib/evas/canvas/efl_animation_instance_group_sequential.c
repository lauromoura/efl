#include "efl_animation_instance_group_sequential_private.h"

EOLIAN static void
_efl_animation_instance_group_sequential_efl_animation_instance_group_instance_add(Eo *eo_obj,
                                                                                   Efl_Animation_Instance_Group_Sequential_Data *pd EINA_UNUSED,
                                                                                   Efl_Animation_Instance *instance)
{
   EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_CHECK_OR_RETURN(eo_obj);

   if (!instance) return;

   efl_animation_instance_group_instance_add(efl_super(eo_obj, MY_CLASS),
                                             instance);

   /* Total duration is calculated in
    * efl_animation_instance_total_duration_get() based on the current group
    * animation instance list.
    * Therefore, the calculated total duration should be set to update total
    * duration. */
   double total_duration =
      efl_animation_instance_total_duration_get(eo_obj);
   efl_animation_instance_total_duration_set(eo_obj, total_duration);
}

EOLIAN static void
_efl_animation_instance_group_sequential_efl_animation_instance_group_instance_del(Eo *eo_obj,
                                                                                   Efl_Animation_Instance_Group_Sequential_Data *pd EINA_UNUSED,
                                                                                   Efl_Animation_Instance *instance)
{
   EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_CHECK_OR_RETURN(eo_obj);

   if (!instance) return;

   efl_animation_instance_group_instance_del(efl_super(eo_obj, MY_CLASS),
                                             instance);

   /* Total duration is calculated in
    * efl_animation_instance_total_duration_get() based on the current group
    * animation instance list.
    * Therefore, the calculated total duration should be set to update total
    * duration. */
   double total_duration =
      efl_animation_instance_total_duration_get(eo_obj);
   efl_animation_instance_total_duration_set(eo_obj, total_duration);
}

EOLIAN static double
_efl_animation_instance_group_sequential_efl_animation_instance_total_duration_get(Eo *eo_obj,
                                                                                   Efl_Animation_Instance_Group_Sequential_Data *pd EINA_UNUSED)
{
   EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_CHECK_OR_RETURN(eo_obj, 0.0);

   Eina_List *instances =
      efl_animation_instance_group_instances_get(eo_obj);
   if (!instances) return 0.0;

   double total_duration = 0.0;
   Eina_List *l;
   Efl_Animation *inst;
   EINA_LIST_FOREACH(instances, l, inst)
     {
        total_duration += efl_animation_instance_total_duration_get(inst);
     }
   return total_duration;
}

EOLIAN static void
_efl_animation_instance_group_sequential_efl_animation_instance_progress_set(Eo *eo_obj,
                                                                             Efl_Animation_Instance_Group_Sequential_Data *pd EINA_UNUSED,
                                                                             double progress)
{
   if ((progress < 0.0) || (progress > 1.0)) return;

   Eina_List *instances = efl_animation_instance_group_instances_get(eo_obj);
   if (!instances) return;

   double group_total_duration =
      efl_animation_instance_total_duration_get(eo_obj);

   double elapsed_time = progress * group_total_duration;

   double sum_prev_total_duration = 0.0;
   Eina_List *l;
   Efl_Animation_Instance *inst;
   EINA_LIST_FOREACH(instances, l, inst)
     {
        //Current animation instance does not start
        if (sum_prev_total_duration > elapsed_time) break;

        //Sum the current total duration
        double total_duration = efl_animation_instance_total_duration_get(inst);

        double inst_progress;

        if (total_duration == 0.0)
          inst_progress = 1.0;
        else
          {
             inst_progress =
                (elapsed_time - sum_prev_total_duration) / total_duration;
             if (inst_progress > 1.0)
               inst_progress = 1.0;
          }

        //Update the sum of the previous instances' total durations
        sum_prev_total_duration += total_duration;

        if ((inst_progress == 1.0) &&
            !efl_animation_instance_final_state_keep_get(inst))
          continue;

        efl_animation_instance_progress_set(inst, inst_progress);
     }
}

/* Internal EO APIs */

#define EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_EXTRA_OPS \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_group_instance_add, _efl_animation_instance_group_sequential_efl_animation_instance_group_instance_add), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_group_instance_del, _efl_animation_instance_group_sequential_efl_animation_instance_group_instance_del), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_total_duration_get, _efl_animation_instance_group_sequential_efl_animation_instance_total_duration_get)

#include "efl_animation_instance_group_sequential.eo.c"
