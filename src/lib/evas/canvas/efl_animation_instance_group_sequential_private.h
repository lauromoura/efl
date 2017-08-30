#define EFL_ANIMATION_INSTANCE_PROTECTED

#include "evas_common_private.h"

#define MY_CLASS EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_CLASS
#define MY_CLASS_NAME efl_class_name_get(MY_CLASS)

#define EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_CHECK_OR_RETURN(inst, ...) \
   do { \
      if (!inst) { \
         CRI("Efl_Animation_Instance " # inst " is NULL!"); \
         return __VA_ARGS__; \
      } \
      if (efl_animation_instance_is_deleted(inst)) { \
         ERR("Efl_Animation_Instance " # inst " has already been deleted!"); \
         return __VA_ARGS__; \
      } \
   } while (0)

#define EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_DATA_GET(o, pd) \
   Efl_Animation_Instance_Group_Sequential_Data *pd = efl_data_scope_get(o, EFL_ANIMATION_INSTANCE_GROUP_SEQUENTIAL_CLASS)

typedef struct _Efl_Animation_Instance_Group_Sequential_Data
{
   unsigned int current_index;

   Eina_List   *finished_inst_list;

   Eina_Bool    is_group_member : 1;
} Efl_Animation_Instance_Group_Sequential_Data;
