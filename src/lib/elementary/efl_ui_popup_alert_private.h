#ifndef EFL_UI_POPUP_ALERT_H
#define EFL_UI_POPUP_ALERT_H

#include "Elementary.h"
#include "elm_widget_layout.h"


typedef enum
{
   EFL_UI_POPUP_ALERT_BUTTON_ONE = 0,
   EFL_UI_POPUP_ALERT_BUTTON_TWO,
   EFL_UI_POPUP_ALERT_BUTTON_TRHEE,
   EFL_UI_POPUP_ALERT_BUTTON_MAX
} Efl_Ui_Popup_Alert_Button_Type;

typedef struct _Efl_Ui_Popup_Alert_Data Efl_Ui_Popup_Alert_Data;
struct _Efl_Ui_Popup_Alert_Data
{
   const char *title_text;
   Evas_Object *button[EFL_UI_POPUP_ALERT_BUTTON_MAX];
   Evas_Object *button_layout[EFL_UI_POPUP_ALERT_BUTTON_MAX];
};

#endif