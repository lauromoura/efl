#ifndef EFL_UI_WIDGET_POPUP_H
#define EFL_UI_WIDGET_POPUP_H

#include "Elementary.h"
#include "elm_widget_layout.h"

typedef struct _Efl_Ui_Popup_Data Efl_Ui_Popup_Data;
struct _Efl_Ui_Popup_Data
{
   Evas_Object *win_parent;
   Evas_Object *event_bg;
};

#endif
