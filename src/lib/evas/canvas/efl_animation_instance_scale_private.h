#define EFL_ANIMATION_INSTANCE_PROTECTED
#define EFL_ANIMATION_INSTANCE_SCALE_PROTECTED

#include "evas_common_private.h"

#define MY_CLASS EFL_ANIMATION_INSTANCE_SCALE_CLASS
#define MY_CLASS_NAME efl_class_name_get(MY_CLASS)

#define EFL_ANIMATION_INSTANCE_SCALE_CHECK_OR_RETURN(inst, ...) \
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

#define EFL_ANIMATION_INSTANCE_SCALE_DATA_GET(o, pd) \
   Efl_Animation_Instance_Scale_Data *pd = efl_data_scope_get(o, EFL_ANIMATION_INSTANCE_SCALE_CLASS)

typedef struct _Efl_Animation_Instance_Scale_Property
{
   double scale_x, scale_y;
} Efl_Animation_Instance_Scale_Property;

typedef struct _Efl_Animation_Instance_Scale_Absolute_Pivot
{
   Evas_Coord cx, cy;
} Efl_Animation_Instance_Scale_Absolute_Pivot;

typedef struct _Efl_Animation_Instance_Scale_Relative_Pivot
{
   Efl_Canvas_Object *obj;
   double             cx, cy;
} Efl_Animation_Instance_Scale_Relative_Pivot;

typedef struct _Efl_Animation_Instance_Scale_Data
{
   Efl_Animation_Instance_Scale_Property from;
   Efl_Animation_Instance_Scale_Property to;

   Efl_Animation_Instance_Scale_Absolute_Pivot abs_pivot;
   Efl_Animation_Instance_Scale_Relative_Pivot rel_pivot;

   Eina_Bool use_rel_pivot;
} Efl_Animation_Instance_Scale_Data;
