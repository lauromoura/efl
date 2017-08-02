#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_WIDGET_PROTECTED
#define ELM_WIDGET_ITEM_PROTECTED

#include <Elementary.h>

#include "elm_priv.h"
#include "efl_ui_popup_scroll.eo.h"
#include "efl_ui_popup_scroll_private.h"

#define MY_CLASS EFL_UI_POPUP_SCROLL_CLASS
#define MY_CLASS_NAME "Efl.Ui.Popup.Scroll"


EOLIAN static Efl_Object *
_efl_ui_popup_scroll_efl_object_constructor(Eo *obj, Efl_Ui_Popup_Scroll_Data *pd)
{
   obj = efl_constructor(efl_super(obj, MY_CLASS));
   return obj;
}

EOLIAN static Elm_Theme_Apply
_efl_ui_popup_scroll_elm_widget_theme_apply(Eo *obj, Efl_Ui_Popup_Scroll_Data *pd EINA_UNUSED)
{
   Elm_Theme_Apply int_ret = ELM_THEME_APPLY_FAILED;

   int_ret = elm_obj_widget_theme_apply(efl_super(obj, MY_CLASS));
   if (!int_ret) return ELM_THEME_APPLY_FAILED;

   return int_ret;
}

EOLIAN static void
_efl_ui_popup_scroll_elm_layout_sizing_eval(Eo *obj, Efl_Ui_Popup_Scroll_Data *pd)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);
   Evas_Coord minw = -1, minh = -1;

   elm_coords_finger_size_adjust(1, &minw, 1, &minh);
   edje_object_size_min_restricted_calc
     (wd->resize_obj, &minw, &minh, minw, minh);
   evas_object_size_hint_min_set(obj, minw, minh);
}

EOLIAN static Eina_Bool
_efl_ui_popup_scroll_efl_container_content_set(Eo *obj, Efl_Ui_Popup_Scroll_Data *pd, Efl_Gfx *content)
{
   pd->content = content;
   
   //Content should have expand propeties since the scroller is not layout layer
   evas_object_size_hint_weight_set(pd->content, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(pd->content, EVAS_HINT_FILL, EVAS_HINT_FILL);

   efl_content_set(efl_part(pd->scroller, "default"), pd->content);
   return EINA_TRUE;
}

EOLIAN static void
_efl_ui_popup_scroll_efl_canvas_group_group_add(Eo *obj, Efl_Ui_Popup_Scroll_Data *pd)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   efl_canvas_group_add(efl_super(obj, MY_CLASS));
   elm_widget_sub_object_parent_add(obj);

   pd->scroller = elm_scroller_add(obj);
   elm_object_style_set(pd->scroller, "popup/no_inset_shadow");
   elm_scroller_policy_set(pd->scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   evas_object_show(pd->scroller);

   efl_content_set(efl_super(obj, MY_CLASS), pd->scroller);
}

EOLIAN static void
_efl_ui_popup_scroll_efl_canvas_group_group_del(Eo *obj, Efl_Ui_Popup_Scroll_Data *pd)
{
   efl_canvas_group_del(efl_super(obj, MY_CLASS));
}

EOLIAN static void
_efl_ui_popup_scroll_class_constructor(Efl_Class *klass)
{
   evas_smart_legacy_type_register(MY_CLASS_NAME, klass);
}

#define EFL_UI_POPUP_SCROLL_EXTRA_OPS \
   EFL_CANVAS_GROUP_ADD_DEL_OPS(efl_ui_popup_scroll)

#include "efl_ui_popup_scroll.eo.c"
