#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_WIDGET_PROTECTED
#define ELM_WIDGET_ITEM_PROTECTED

#include <Elementary.h>

#include "elm_priv.h"
#include "efl_ui_popup_alert_text.eo.h"
#include "efl_ui_popup_alert_text_private.h"

#define MY_CLASS EFL_UI_POPUP_ALERT_TEXT_CLASS
#define MY_CLASS_NAME "Efl.Ui.Popup.Alert.Text"


EOLIAN static Efl_Object *
_efl_ui_popup_alert_text_efl_object_constructor(Eo *obj, Efl_Ui_Popup_Alert_Text_Data *pd)
{
   obj = efl_constructor(efl_super(obj, MY_CLASS));
   return obj;
}

EOLIAN static Elm_Theme_Apply
_efl_ui_popup_alert_text_elm_widget_theme_apply(Eo *obj, Efl_Ui_Popup_Alert_Text_Data *pd EINA_UNUSED)
{
   Elm_Theme_Apply int_ret = ELM_THEME_APPLY_FAILED;

   int_ret = elm_obj_widget_theme_apply(efl_super(obj, MY_CLASS));
   if (!int_ret) return ELM_THEME_APPLY_FAILED;

   return int_ret;
}

static void
_scroller_sizing_eval(Eo *obj, Efl_Ui_Popup_Alert_Text_Data *pd, int minh)
{
   int w, h;
   evas_object_geometry_get(obj, NULL, NULL, &w, &h);

   if (pd->is_expandable_h)
     {
       if ((pd->max_scroll_h > -1) && (minh > pd->max_scroll_h))
         {
            elm_scroller_content_min_limit(pd->scroller, EINA_FALSE, EINA_FALSE);
            evas_object_resize(obj, w, pd->max_scroll_h);
         }
     }
}

EOLIAN static void
_efl_ui_popup_alert_text_elm_layout_sizing_eval(Eo *obj, Efl_Ui_Popup_Alert_Text_Data *pd)
{
   elm_layout_sizing_eval(efl_super(obj, MY_CLASS));

   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);
   Evas_Coord minw = -1, minh = -1;

   elm_coords_finger_size_adjust(1, &minw, 1, &minh);
   edje_object_size_min_restricted_calc(wd->resize_obj, &minw, &minh, minw, minh);
   evas_object_size_hint_min_set(obj, minw, minh);

   _scroller_sizing_eval(obj, pd, minh);
}

EOLIAN static Eina_Bool
_efl_ui_popup_alert_text_efl_container_content_set(Eo *obj, Efl_Ui_Popup_Alert_Text_Data *pd, Efl_Gfx *content)
{
   return EINA_TRUE;
}

Efl_Gfx *
_efl_ui_popup_alert_text_efl_container_content_get(Eo *obj, Efl_Ui_Popup_Alert_Text_Data *pd)
{
   return NULL;
}

EOLIAN static void
_efl_ui_popup_alert_text_efl_text_text_set(Eo *obj, Efl_Ui_Popup_Alert_Text_Data *pd, const char *text)
{
   if (!pd->message)
     {
        pd->message = elm_label_add(obj);
        elm_label_line_wrap_set(pd->message, ELM_WRAP_MIXED);
        efl_content_set(efl_part(pd->scroller, "default"), pd->message);
     }
   elm_object_text_set(pd->message, text);
   elm_layout_sizing_eval(obj);
}

EOLIAN static const char *
_efl_ui_popup_alert_text_efl_text_text_get(Eo *obj, Efl_Ui_Popup_Alert_Text_Data *pd)
{
   if (pd->message)
     return elm_object_text_get(pd->message);
   return NULL;
}

static void
_efl_ui_popup_alert_text_expandable_set(Eo *obj, Efl_Ui_Popup_Alert_Text_Data *pd, Eina_Bool is_expandable_h)
{
   if (is_expandable_h)
     {
        pd->is_expandable_h = EINA_TRUE;
        elm_scroller_content_min_limit(pd->scroller, EINA_FALSE, EINA_TRUE);
     } 
   else
     {
        pd->is_expandable_h = EINA_FALSE;
        elm_scroller_content_min_limit(pd->scroller, EINA_FALSE, EINA_FALSE);
     }
}

static void
_efl_ui_popup_alert_text_efl_gfx_size_hint_hint_max_set(Eo *obj, Efl_Ui_Popup_Alert_Text_Data *pd, int w, int h)
{
   efl_gfx_size_hint_max_set(efl_super(obj, MY_CLASS), w, h);
   pd->max_scroll_h = h;
   elm_layout_sizing_eval(obj);
}

EOLIAN static void
_efl_ui_popup_alert_text_efl_canvas_group_group_add(Eo *obj, Efl_Ui_Popup_Alert_Text_Data *pd)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   efl_canvas_group_add(efl_super(obj, MY_CLASS));
   elm_widget_sub_object_parent_add(obj);

   pd->scroller = elm_scroller_add(obj);
   elm_object_style_set(pd->scroller, "popup/no_inset_shadow");
   elm_scroller_policy_set(pd->scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);

   efl_content_set(efl_super(obj, MY_CLASS), pd->scroller);

   pd->max_scroll_h = -1;
}

EOLIAN static void
_efl_ui_popup_alert_text_efl_canvas_group_group_del(Eo *obj, Efl_Ui_Popup_Alert_Text_Data *pd)
{
   efl_canvas_group_del(efl_super(obj, MY_CLASS));
}

EOLIAN static void
_efl_ui_popup_alert_text_class_constructor(Efl_Class *klass)
{
   evas_smart_legacy_type_register(MY_CLASS_NAME, klass);
}

#define EFL_UI_POPUP_ALERT_TEXT_EXTRA_OPS \
   EFL_CANVAS_GROUP_ADD_DEL_OPS(efl_ui_popup_alert_text)

#include "efl_ui_popup_alert_text.eo.c"
