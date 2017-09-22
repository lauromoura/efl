#include "efl_animation_instance_group_private.h"

EOLIAN static void
_efl_animation_instance_group_instance_add(Eo *eo_obj,
                                           Efl_Animation_Instance_Group_Data *pd,
                                           Efl_Animation_Instance *instance)
{
   EFL_ANIMATION_INSTANCE_GROUP_CHECK_OR_RETURN(eo_obj);

   if (!instance) return;

   Efl_Canvas_Object *target = efl_animation_instance_target_get(eo_obj);
   if (target)
     efl_animation_instance_target_set(instance, target);

   double duration = efl_animation_instance_duration_get(eo_obj);
   /* if group animation instance duration is available value, then the duration
    * is propagated to its child. */
   if (duration != EFL_ANIMATION_INSTANCE_GROUP_DURATION_NONE)
     efl_animation_instance_duration_set(instance, duration);

   Eina_Bool state_keep = efl_animation_instance_final_state_keep_get(eo_obj);
   efl_animation_instance_final_state_keep_set(instance, state_keep);

   pd->instances = eina_list_append(pd->instances, instance);
}

EOLIAN static void
_efl_animation_instance_group_instance_del(Eo *eo_obj,
                                           Efl_Animation_Instance_Group_Data *pd,
                                           Efl_Animation_Instance *instance)
{
   EFL_ANIMATION_INSTANCE_GROUP_CHECK_OR_RETURN(eo_obj);

   if (!instance) return;

   pd->instances = eina_list_remove(pd->instances, instance);
}

EOLIAN static Eina_List *
_efl_animation_instance_group_instances_get(Eo *eo_obj,
                                            Efl_Animation_Instance_Group_Data *pd)
{
   EFL_ANIMATION_INSTANCE_GROUP_CHECK_OR_RETURN(eo_obj, NULL);

   return pd->instances;
}

EOLIAN static void
_efl_animation_instance_group_efl_animation_instance_target_set(Eo *eo_obj,
                                                                Efl_Animation_Instance_Group_Data *pd,
                                                                Efl_Canvas_Object *target)
{
   EFL_ANIMATION_INSTANCE_GROUP_CHECK_OR_RETURN(eo_obj);

   Eina_List *l;
   Efl_Animation_Instance *inst;

   EINA_LIST_FOREACH(pd->instances, l, inst)
     {
        efl_animation_instance_target_set(inst, target);
     }

   efl_animation_instance_target_set(efl_super(eo_obj, MY_CLASS), target);
}

EOLIAN static void
_efl_animation_instance_group_efl_animation_instance_duration_set(Eo *eo_obj,
                                                                  Efl_Animation_Instance_Group_Data *pd,
                                                                  double duration)
{
   EFL_ANIMATION_INSTANCE_GROUP_CHECK_OR_RETURN(eo_obj);

   if (duration == EFL_ANIMATION_INSTANCE_GROUP_DURATION_NONE) goto end;

   if (duration < 0.0) return;

   Eina_List *l;
   Efl_Animation_Instance *inst;

   EINA_LIST_FOREACH(pd->instances, l, inst)
     {
        efl_animation_instance_duration_set(inst, duration);
     }

end:
   efl_animation_instance_duration_only_set(eo_obj, duration);

   /* efl_animation_instance_total_duration_get() should calculate the new total
    * duration. */
   double total_duration = efl_animation_instance_total_duration_get(eo_obj);
   efl_animation_instance_total_duration_set(eo_obj, total_duration);
}

EOLIAN static void
_efl_animation_instance_group_efl_animation_instance_final_state_keep_set(Eo *eo_obj,
                                                                          Efl_Animation_Instance_Group_Data *pd,
                                                                          Eina_Bool state_keep)
{
   EFL_ANIMATION_INSTANCE_GROUP_CHECK_OR_RETURN(eo_obj);

   Eina_List *l;
   Efl_Animation_Instance *inst;

   EINA_LIST_FOREACH(pd->instances, l, inst)
     {
        efl_animation_instance_final_state_keep_set(inst, state_keep);
     }

   efl_animation_instance_final_state_keep_set(efl_super(eo_obj, MY_CLASS),
                                               state_keep);
}

EOLIAN static void
_efl_animation_instance_group_efl_animation_instance_interpolator_set(Eo *eo_obj,
                                                                      Efl_Animation_Instance_Group_Data *pd,
                                                                      Efl_Interpolator *interpolator)
{
   EFL_ANIMATION_INSTANCE_GROUP_CHECK_OR_RETURN(eo_obj);

   Eina_List *l;
   Efl_Animation_Instance *inst;

   EINA_LIST_FOREACH(pd->instances, l, inst)
     {
        efl_animation_instance_interpolator_set(inst, interpolator);
     }

   efl_animation_instance_interpolator_set(efl_super(eo_obj, MY_CLASS),
                                           interpolator);
}

EOLIAN static Efl_Object *
_efl_animation_instance_group_efl_object_constructor(Eo *eo_obj,
                                                     Efl_Animation_Instance_Group_Data *pd)
{
   eo_obj = efl_constructor(efl_super(eo_obj, MY_CLASS));

   pd->instances = NULL;

   //group animation instance does not affect its child duration by default.
   efl_animation_instance_duration_only_set(eo_obj,
                                            EFL_ANIMATION_INSTANCE_GROUP_DURATION_NONE);

   return eo_obj;
}

EOLIAN static void
_efl_animation_instance_group_efl_object_destructor(Eo *eo_obj,
                                                    Efl_Animation_Instance_Group_Data *pd)
{
   Efl_Animation_Instance *inst;

   EINA_LIST_FREE(pd->instances, inst)
     efl_del(inst);

   efl_destructor(efl_super(eo_obj, MY_CLASS));
}

EOLIAN static void
_efl_animation_instance_group_efl_animation_instance_target_state_save(Eo *eo_obj,
                                                                       Efl_Animation_Instance_Group_Data *pd)
{
   EFL_ANIMATION_INSTANCE_GROUP_CHECK_OR_RETURN(eo_obj);

   Eina_List *l;
   Efl_Animation_Instance *inst;

   EINA_LIST_FOREACH(pd->instances, l, inst)
     {
        efl_animation_instance_target_state_save(inst);
     }
}

EOLIAN static void
_efl_animation_instance_group_efl_animation_instance_target_state_reset(Eo *eo_obj,
                                                                        Efl_Animation_Instance_Group_Data *pd)
{
   EFL_ANIMATION_INSTANCE_GROUP_CHECK_OR_RETURN(eo_obj);

   Eina_List *l;
   Efl_Animation_Instance *inst;

   EINA_LIST_FOREACH(pd->instances, l, inst)
     {
        efl_animation_instance_target_state_reset(inst);
     }
}

EOLIAN static void
_efl_animation_instance_group_efl_animation_instance_target_map_reset(Eo *eo_obj,
                                                                      Efl_Animation_Instance_Group_Data *pd)
{
   EFL_ANIMATION_INSTANCE_GROUP_CHECK_OR_RETURN(eo_obj);

   Eina_List *l;
   Efl_Animation_Instance *inst;

   EINA_LIST_FOREACH(pd->instances, l, inst)
     {
        efl_animation_instance_target_map_reset(inst);
     }
}

/* Internal EO APIs */

EOAPI EFL_VOID_FUNC_BODYV(efl_animation_instance_group_instance_add, EFL_FUNC_CALL(animation), Efl_Animation_Instance *animation);

EOAPI EFL_VOID_FUNC_BODYV(efl_animation_instance_group_instance_del, EFL_FUNC_CALL(animation), Efl_Animation_Instance *animation);

EOAPI EFL_FUNC_BODY(efl_animation_instance_group_instances_get, Eina_List *, NULL);

#define EFL_ANIMATION_INSTANCE_GROUP_EXTRA_OPS \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_group_instance_add, _efl_animation_instance_group_instance_add), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_group_instance_del, _efl_animation_instance_group_instance_del), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_group_instances_get, _efl_animation_instance_group_instances_get), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_target_set, _efl_animation_instance_group_efl_animation_instance_target_set), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_duration_set, _efl_animation_instance_group_efl_animation_instance_duration_set), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_final_state_keep_set, _efl_animation_instance_group_efl_animation_instance_final_state_keep_set), \
   EFL_OBJECT_OP_FUNC(efl_animation_instance_interpolator_set, _efl_animation_instance_group_efl_animation_instance_interpolator_set)

#include "efl_animation_instance_group.eo.c"
