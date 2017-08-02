#ifndef EFL_UI_POPUP_SCROLL_H
#define EFL_UI_POPUP_SCROLL_H

#include "Elementary.h"
#include "elm_widget_layout.h"

typedef struct _Efl_Ui_Popup_Scroll_Data Efl_Ui_Popup_Scroll_Data;
struct _Efl_Ui_Popup_Scroll_Data
{
   Evas_Object *scroller;
   Evas_Object *content;
};

#endif
