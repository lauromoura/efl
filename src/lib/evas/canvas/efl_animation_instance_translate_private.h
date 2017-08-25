#define EFL_ANIMATION_INSTANCE_PROTECTED

#include "evas_common_private.h"

#define MY_CLASS EFL_ANIMATION_INSTANCE_TRANSLATE_CLASS
#define MY_CLASS_NAME efl_class_name_get(MY_CLASS)

#define EFL_ANIMATION_INSTANCE_TRANSLATE_CHECK_OR_RETURN(inst, ...) \
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

#define EFL_ANIMATION_INSTANCE_TRANSLATE_DATA_GET(o, pd) \
   Efl_Animation_Instance_Translate_Data *pd = efl_data_scope_get(o, EFL_ANIMATION_INSTANCE_TRANSLATE_CLASS)

typedef struct _Efl_Animation_Instance_Translate_Property
{
   Evas_Coord move_x, move_y;
   Evas_Coord x, y;
} Efl_Animation_Instance_Translate_Property;

typedef struct _Efl_Animation_Instance_Translate_Data
{
   Efl_Animation_Instance_Translate_Property from;
   Efl_Animation_Instance_Translate_Property to;

   Evas_Coord start_x, start_y;

   Eina_Bool use_rel_move;
} Efl_Animation_Instance_Translate_Data;
