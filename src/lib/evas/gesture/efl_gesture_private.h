#ifndef EFL_GESTURE_PRIVATE_H_
#define EFL_GESTURE_PRIVATE_H_

#include "evas_common_private.h"
#include "evas_private.h"
#define EFL_INTERNAL_UNSTABLE
#include "interfaces/efl_common_internal.h"

#include <Ecore.h>

//private gesture classes
#include "efl_gesture_recognizer_tap.eo.h"

typedef struct _Efl_Gesture_Recognizer_Data             Efl_Gesture_Recognizer_Data;
typedef struct _Efl_Gesture_Data                        Efl_Gesture_Data;
typedef struct _Efl_Gesture_Tap_Data                    Efl_Gesture_Tap_Data;
typedef struct _Efl_Gesture_Long_Tap_Data               Efl_Gesture_Long_Tap_Data;

struct _Efl_Gesture_Recognizer_Data
{
   Eo *manager; // keeps a reference of the manager
};

struct _Efl_Gesture_Data
{
   const Efl_Event_Description    *type;
   Efl_Gesture_State               state;
   Eina_Position2D                 hotspot;
};

struct _Efl_Gesture_Tap_Data
{
   Eina_Position2D   pos;
};

struct _Efl_Gesture_Long_Tap_Data
{
   Eina_Position2D   pos;
   Ecore_Timer      *timeout;
   Eina_Bool         is_timeout;
};

#endif
