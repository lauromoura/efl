#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_WIDGET_PROTECTED
#define ELM_WIDGET_ITEM_PROTECTED
#define EFL_UI_POPUP_PROTECTED

#include <Elementary.h>

#include "elm_priv.h"
#include "efl_ui_popup.eo.h"
#include "efl_ui_popup_private.h"

#define MY_CLASS EFL_UI_POPUP_CLASS
#define MY_CLASS_NAME "Efl.Ui.Popup"
#define MY_CLASS_NAME_LEGACY "elm_popup"

static void
_bg_clicked_cb(void *data,
                  Evas_Object *o EINA_UNUSED,
                  const char *emission EINA_UNUSED,
                  const char *source EINA_UNUSED)
{
   Evas_Object *obj = data;
   efl_event_callback_legacy_call(obj, EFL_UI_POPUP_EVENT_BG_CLICKED, NULL);
}

EOLIAN static Efl_Object *
_efl_ui_popup_efl_object_constructor(Eo *obj, Efl_Ui_Popup_Data *pd)
{
   obj = efl_constructor(efl_super(obj, MY_CLASS));
   return obj;
}

EOLIAN static Elm_Theme_Apply
_efl_ui_popup_elm_widget_theme_apply(Eo *obj, Efl_Ui_Popup_Data *pd EINA_UNUSED)
{
   Elm_Theme_Apply int_ret = ELM_THEME_APPLY_FAILED;

   int_ret = elm_obj_widget_theme_apply(efl_super(obj, MY_CLASS));
   if (!int_ret) return ELM_THEME_APPLY_FAILED;

   return int_ret;
}

EOLIAN static void
_efl_ui_popup_efl_gfx_position_set(Eo *obj, Efl_Ui_Popup_Data *pd, int x, int y)
{
   pd->align = EFL_UI_POPUP_ALIGN_NONE;
   efl_gfx_position_set(efl_super(obj, MY_CLASS), x, y);
}

EOLIAN static void
_efl_ui_popup_efl_gfx_size_set(Eo *obj, Efl_Ui_Popup_Data *pd, int w, int h)
{
   efl_gfx_size_set(efl_super(obj, MY_CLASS), w, h);
}

static void
_calc_align(Evas_Object *obj)
{
   Efl_Ui_Popup_Data *pd = efl_data_scope_get(obj, MY_CLASS);

   Evas_Coord x, y, w, h;
   evas_object_geometry_get(pd->win_parent, &x, &y, &w, &h);

   x = 0;
   y = 0;

   evas_object_move(pd->event_bg, x, y);
   evas_object_resize(pd->event_bg, w, h);

   Evas_Coord pw, ph;
   evas_object_geometry_get(obj, NULL, NULL, &pw, &ph);

   switch (pd->align)
     {
        case EFL_UI_POPUP_ALIGN_CENTER:
           efl_gfx_position_set(efl_super(obj, MY_CLASS), x + ((w - pw ) / 2), y + ((h - ph) / 2));
        break;
        case EFL_UI_POPUP_ALIGN_LEFT:
           efl_gfx_position_set(efl_super(obj, MY_CLASS), x, y + ((h - ph) / 2));
        break;
        case EFL_UI_POPUP_ALIGN_RIGHT:
           efl_gfx_position_set(efl_super(obj, MY_CLASS), x + (w - pw), ((h - ph) / 2));
        break;
        case EFL_UI_POPUP_ALIGN_TOP:
           efl_gfx_position_set(efl_super(obj, MY_CLASS), x + ((w - pw) / 2), y);
        break;
        case EFL_UI_POPUP_ALIGN_BOTTOM:
           efl_gfx_position_set(efl_super(obj, MY_CLASS), x + ((w - pw) / 2), y + (h - ph));
        break;
        default:
        break;
     }
}

static void
_parent_geom_cb(void *data, const Efl_Event *ev)
{
   _calc_align(data);
}

EOLIAN static void
_efl_ui_popup_elm_widget_widget_parent_set(Eo *obj, Efl_Ui_Popup_Data *pd, Evas_Object *parent)
{
   Evas_Coord x, y, w, h;
   pd->win_parent = efl_provider_find(obj, EFL_UI_WIN_CLASS);
   if (!pd->win_parent)
     {
        ERR("Cannot find window parent");
        return;
     }

   evas_object_geometry_get(pd->win_parent, &x, &y, &w, &h);
   evas_object_move(pd->event_bg, x, y);
   evas_object_resize(pd->event_bg, w, h);

   efl_event_callback_add(pd->win_parent, EFL_GFX_EVENT_RESIZE, _parent_geom_cb, obj);
   efl_event_callback_add(pd->win_parent, EFL_GFX_EVENT_MOVE, _parent_geom_cb, obj);
}

EOLIAN static Efl_Canvas_Object *
_efl_ui_popup_parent_window_get(Eo *obj, Efl_Ui_Popup_Data *pd)
{
   return pd->win_parent;
}

EOLIAN static void
_efl_ui_popup_align_set(Eo *obj, Efl_Ui_Popup_Data *pd, Efl_Ui_Popup_Align type)
{
   pd->align = type;
   _calc_align(obj);
}

EOLIAN static Efl_Ui_Popup_Align
_efl_ui_popup_align_get(Eo *obj, Efl_Ui_Popup_Data *pd)
{
   return pd->align;
}

EOLIAN static void
_efl_ui_popup_efl_canvas_group_group_add(Eo *obj, Efl_Ui_Popup_Data *pd)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   efl_canvas_group_add(efl_super(obj, MY_CLASS));
   elm_widget_sub_object_parent_add(obj);

   elm_widget_can_focus_set(obj, EINA_TRUE);
   elm_layout_theme_set(obj, "popup", "base", "view");

   pd->event_bg = edje_object_add(evas_object_evas_get(obj));
   elm_widget_theme_object_set(obj, pd->event_bg, "popup", "base", "event_bg");
   evas_object_smart_member_add(pd->event_bg, obj);
   evas_object_stack_below(pd->event_bg, wd->resize_obj);

   edje_object_signal_callback_add(pd->event_bg, "elm,action,clicked", "*", _bg_clicked_cb, obj);

   pd->align = EFL_UI_POPUP_ALIGN_CENTER;
}

EOLIAN static void
_efl_ui_popup_efl_canvas_group_group_del(Eo *obj, Efl_Ui_Popup_Data *pd)
{
   ELM_SAFE_FREE(pd->event_bg, evas_object_del);
   efl_event_callback_del(pd->win_parent, EFL_GFX_EVENT_RESIZE, _parent_geom_cb, obj);
   efl_event_callback_del(pd->win_parent, EFL_GFX_EVENT_MOVE, _parent_geom_cb, obj);

   efl_canvas_group_del(efl_super(obj, MY_CLASS));
}

EOLIAN static void
_efl_ui_popup_elm_layout_sizing_eval(Eo *obj, Efl_Ui_Popup_Data *pd)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);
   Evas_Coord minw = -1, minh = -1;

   elm_coords_finger_size_adjust(1, &minw, 1, &minh);
   edje_object_size_min_restricted_calc
     (wd->resize_obj, &minw, &minh, minw, minh);
   evas_object_size_hint_min_set(obj, minw, minh);

   Evas_Coord w, h;
   evas_object_geometry_get(obj, NULL, NULL, &w, &h);

   if ((minw > w) && (minh < h))
     evas_object_resize(obj, minw, h);
   else if ((minw < w) && (minh > h))
     evas_object_resize(obj, w, minh);
   else if ((minw >= w) && (minh >= h))
     evas_object_resize(obj, minw, minh);
}

EOLIAN static void
_efl_ui_popup_bg_set(Eo *obj, Efl_Ui_Popup_Data *pd, const char* file, const char* group)
{
   Evas_Object *prev_obj = edje_object_part_swallow_get(pd->event_bg, "elm.swallow.image");
   if (prev_obj)
   {
     edje_object_part_unswallow(pd->event_bg, prev_obj);
     evas_object_del(prev_obj);
   }

   Evas_Object *image = elm_image_add(obj);
   Eina_Bool ret = elm_image_file_set(image, file, group);
   if (!ret)
   {
      edje_object_signal_emit(pd->event_bg, "elm,state,image,hidden", "elm");
      evas_object_del(image);
      return;
   }
   edje_object_part_swallow(pd->event_bg, "elm.swallow.image", image);
   edje_object_signal_emit(pd->event_bg, "elm,state,image,visible", "elm");
}

EOLIAN static void
_efl_ui_popup_bg_repeat_events_set(Eo *obj EINA_UNUSED, Efl_Ui_Popup_Data *pd, Eina_Bool repeat)
{
   pd->bg_repeat_events = repeat;

   evas_object_repeat_events_set(pd->event_bg, repeat);
}

EOLIAN static Eina_Bool
_efl_ui_popup_bg_repeat_events_get(Eo *obj EINA_UNUSED, Efl_Ui_Popup_Data *pd)
{
  return pd->bg_repeat_events;
}

EOLIAN static void
_efl_ui_popup_class_constructor(Efl_Class *klass)
{
   evas_smart_legacy_type_register(MY_CLASS_NAME, klass);
}


#define EFL_UI_POPUP_EXTRA_OPS \
   EFL_CANVAS_GROUP_ADD_OPS(efl_ui_popup)

#include "efl_ui_popup.eo.c"
