#include "efl_animation_instance_group_parallel_private.h"

/* Add member instance data and append the data to the member instance data list.
 * The member instance data contains the repeated count of the member instance.
 */
static void
_member_inst_data_add(Efl_Animation_Instance_Group_Parallel_Data *pd,
                      Efl_Animation_Instance *inst,
                      int repeated_count)
{
   Member_Instance_Data *member_inst_data =
      calloc(1, sizeof(Member_Instance_Data));

   if (!member_inst_data) return;

   member_inst_data->inst = inst;
   member_inst_data->repeated_count = repeated_count;

   pd->member_inst_data_list =
      eina_list_append(pd->member_inst_data_list, member_inst_data);
}

/* Find the member instance data which contains the repeated count of the member
 * instance. */
static Member_Instance_Data *
_member_inst_data_find(Efl_Animation_Instance_Group_Parallel_Data *pd,
                       Efl_Animation_Instance *inst)
{
   Eina_List *l;
   Member_Instance_Data *member_inst_data = NULL;
   EINA_LIST_FOREACH(pd->member_inst_data_list, l, member_inst_data)
     {
        if (member_inst_data->inst == inst)
          break;
     }

   return member_inst_data;
}

/* Delete member instance data and remove the data from the member instance data
 * list.
 * The member instance data contains the repeated count of the member instance.
 */
static void
_member_inst_data_del(Efl_Animation_Instance_Group_Parallel_Data *pd,
                      Efl_Animation_Instance *inst)
{
   Member_Instance_Data *member_inst_data = _member_inst_data_find(pd, inst);
   if (member_inst_data)
     {
        pd->member_inst_data_list =
           eina_list_remove(pd->member_inst_data_list, member_inst_data);
        free(member_inst_data);
     }
}

EOLIAN static void
_efl_animation_instance_group_parallel_efl_animation_instance_group_instance_add(Eo *eo_obj,
                                                                                 Efl_Animation_Instance_Group_Parallel_Data *pd,
                                                                                 Efl_Animation_Instance *instance)
{
   EFL_ANIMATION_INSTANCE_GROUP_PARALLEL_CHECK_OR_RETURN(eo_obj);

   if (!instance) return;

   efl_animation_instance_group_instance_add(efl_super(eo_obj, MY_CLASS),
                                             instance);

   /* Add member instance data and append the data to the member instance data
    * list. */
   _member_inst_data_add(pd, instance, 0);

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
_efl_animation_instance_group_parallel_efl_animation_instance_group_instance_del(Eo *eo_obj,
                                                                                 Efl_Animation_Instance_Group_Parallel_Data *pd EINA_UNUSED,
                                                                                 Efl_Animation_Instance *instance)
{
   EFL_ANIMATION_INSTANCE_GROUP_PARALLEL_CHECK_OR_RETURN(eo_obj);

   if (!instance) return;

   efl_animation_instance_group_instance_del(efl_super(eo_obj, MY_CLASS),
                                             instance);

   /* Delete member instance data and remove the data from the member instance
    * data list. */
   _member_inst_data_del(pd, instance);

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
_efl_animation_instance_group_parallel_efl_animation_instance_total_duration_get(Eo *eo_obj,
                                                                                 Efl_Animation_Instance_Group_Parallel_Data *pd EINA_UNUSED)
{
   EFL_ANIMATION_INSTANCE_GROUP_PARALLEL_CHECK_OR_RETURN(eo_obj, 0.0);

   Eina_List *instances =
      efl_animation_instance_group_instances_get(eo_obj);
   if (!instances) return 0.0;

   double total_duration = 0.0;
   Eina_List *l;
   Efl_Animation *inst;
   EINA_LIST_FOREACH(instances, l, inst)
     {
        double child_total_duration =
           efl_animation_instance_total_duration_get(inst);

        int child_repeat_count = efl_animation_instance_repeat_count_get(inst);
        if (child_repeat_count > 0)
          child_total_duration *= (child_repeat_count + 1);

        if (child_total_duration > total_duration)
          total_duration = child_total_duration;
     }
   return total_duration;
}

//Set how many times the given instance has been repeated.
static void
_repeated_count_set(Efl_Animation_Instance_Group_Parallel_Data *pd,
                    Efl_Animation_Instance *inst,
                    int repeated_count)
{

   Member_Instance_Data *member_inst_data = _member_inst_data_find(pd, inst);
   if (!member_inst_data) return;

   member_inst_data->repeated_count = repeated_count;
}

//Get how many times the given instance has been repeated.
static int
_repeated_count_get(Efl_Animation_Instance_Group_Parallel_Data *pd,
                    Efl_Animation_Instance *inst)
{
   Member_Instance_Data *member_inst_data = _member_inst_data_find(pd, inst);
   if (!member_inst_data) return 0;

   return member_inst_data->repeated_count;
}

EOLIAN static void
_efl_animation_instance_group_parallel_efl_animation_instance_progress_set(Eo *eo_obj,
                                                                           Efl_Animation_Instance_Group_Parallel_Data *pd,
                                                                           double progress)
{
   if ((progress < 0.0) || (progress > 1.0)) return;

   Eina_List *instances = efl_animation_instance_group_instances_get(eo_obj);
   if (!instances) return;

   double group_total_duration =
      efl_animation_instance_total_duration_get(eo_obj);

   double elapsed_time = progress * group_total_duration;

   Eina_List *l;
   Efl_Animation_Instance *inst;
   EINA_LIST_FOREACH(instances, l, inst)
     {
        double total_duration = efl_animation_instance_total_duration_get(inst);
        double inst_progress;

        if (total_duration == 0.0)
          inst_progress = 1.0;
        else
          {
             //If instance is repeated, then recalculate progress.
             int repeated_count = _repeated_count_get(pd, inst);
             if (repeated_count > 0)
               inst_progress = (elapsed_time - (total_duration * repeated_count)) / total_duration;
             else
               inst_progress = elapsed_time / total_duration;

             if (inst_progress > 1.0)
               inst_progress = 1.0;

             //Animation has been finished.
             if (inst_progress == 1.0)
               {
                  /* If instance is finished and it should be repeated, then
                   * increate the repeated count to recalculate progress. */
                  int repeat_count =
                     efl_animation_instance_repeat_count_get(inst);
                  if (repeat_count > 0)
                    {
                       int repeated_count = _repeated_count_get(pd, inst);
                       if (repeated_count < repeat_count)
                         {
                            repeated_count++;
                            _repeated_count_set(pd, inst, repeated_count);
                         }
                    }
               }
          }

        efl_animation_instance_progress_set(inst, inst_progress);
     }
}

/* Internal EO APIs */

#define EFL_ANIMATION_INSTANCE_GROUP_PARALLEL_EXTRA_OPS \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_group_instance_add, _efl_animation_instance_group_parallel_efl_animation_instance_group_instance_add), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_group_instance_del, _efl_animation_instance_group_parallel_efl_animation_instance_group_instance_del), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_total_duration_get, _efl_animation_instance_group_parallel_efl_animation_instance_total_duration_get)

#include "efl_animation_instance_group_parallel.eo.c"
