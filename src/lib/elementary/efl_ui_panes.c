#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED
#define ELM_LAYOUT_PROTECTED
#define EFL_GFX_SIZE_HINT_PROTECTED

#include <Elementary.h>

#include "elm_priv.h"
#include "elm_widget_layout.h"
#include "efl_ui_panes_private.h"

#include "efl_ui_panes_part.eo.h"
#include "elm_part_helper.h"

#define MY_CLASS EFL_UI_PANES_CLASS
#define MY_CLASS_PFX efl_ui_panes

#define MY_CLASS_NAME "Efl.Ui.Panes"
#define MY_CLASS_NAME_LEGACY "elm_panes"
/**
 * TODO
 * Update the minimun height of the bar in the theme.
 * No minimun should be set in the vertical theme
 * Add events (move, start ...)
 */

static const char SIG_CLICKED[] = "clicked";
static const char SIG_PRESS[] = "press";
static const char SIG_UNPRESS[] = "unpress";
static const char SIG_DOUBLE_CLICKED[] = "clicked,double";
static const Evas_Smart_Cb_Description _smart_callbacks[] = {
   {SIG_CLICKED, ""},
   {SIG_PRESS, ""},
   {SIG_UNPRESS, ""},
   {SIG_DOUBLE_CLICKED, ""},
   {NULL, NULL}
};

static const Elm_Layout_Part_Alias_Description _content_aliases[] =
{
   //XXX: change elm.swallow.left/right to *.first/second in new theme.
   {"left", "elm.swallow.left"},
   {"right", "elm.swallow.right"},
   {"top", "elm.swallow.left"},
   {"bottom", "elm.swallow.right"},
   {"first", "elm.swallow.left"},
   {"second", "elm.swallow.right"},
   {NULL, NULL}
};

static void _set_min_size_new(void *data);

EOLIAN static Efl_Ui_Theme_Apply
_efl_ui_panes_elm_widget_theme_apply(Eo *obj, Efl_Ui_Panes_Data *sd)
{
   double size;
   Evas_Coord minw = 0, minh = 0;

   Efl_Ui_Theme_Apply int_ret = EFL_UI_THEME_APPLY_FAILED;
   EFL_UI_LAYOUT_DATA_GET(obj, ld);

   if (sd->dir == EFL_UI_DIR_HORIZONTAL)
     eina_stringshare_replace(&ld->group, "horizontal");
   else
     eina_stringshare_replace(&ld->group, "vertical");

   evas_object_hide(sd->event);
   elm_coords_finger_size_adjust(1, &minw, 1, &minh);
   evas_object_size_hint_min_set(sd->event, minw, minh);

   int_ret = elm_obj_widget_theme_apply(efl_super(obj, MY_CLASS));
   if (!int_ret) return EFL_UI_THEME_APPLY_FAILED;

   size = elm_panes_content_left_size_get(obj);

   if (sd->fixed)
     {
        elm_layout_signal_emit(obj, "elm,panes,fixed", "elm");

        //TODO: remove this signal on EFL 2.0.
        // I left this due to the backward compatibility.
        elm_layout_signal_emit(obj, "elm.panes.fixed", "elm");
     }

   elm_layout_sizing_eval(obj);

   elm_panes_content_left_size_set(obj, size);

   return int_ret;
}

static void
_on_clicked(void *data,
            Evas_Object *obj EINA_UNUSED,
            const char *emission EINA_UNUSED,
            const char *source EINA_UNUSED)
{
   efl_event_callback_legacy_call(data, EFL_UI_EVENT_CLICKED, NULL);
}

static void
_double_clicked(void *data,
                Evas_Object *obj EINA_UNUSED,
                const char *emission EINA_UNUSED,
                const char *source EINA_UNUSED)
{
   EFL_UI_PANES_DATA_GET(data, sd);

   sd->double_clicked = EINA_TRUE;
}

static void
_on_pressed(void *data,
            Evas_Object *obj EINA_UNUSED,
            const char *emission EINA_UNUSED,
            const char *source EINA_UNUSED)
{
   efl_event_callback_legacy_call(data, ELM_PANES_EVENT_PRESS, NULL);
}

static void
_on_unpressed(void *data,
              Evas_Object *obj EINA_UNUSED,
              const char *emission EINA_UNUSED,
              const char *source EINA_UNUSED)
{
   EFL_UI_PANES_DATA_GET(data, sd);
   efl_event_callback_legacy_call(data, ELM_PANES_EVENT_UNPRESS, NULL);

   if (sd->double_clicked)
     {
        efl_event_callback_legacy_call(data, EFL_UI_EVENT_CLICKED_DOUBLE, NULL);
        sd->double_clicked = EINA_FALSE;
     }
}

EOLIAN static void
_efl_ui_panes_elm_layout_sizing_eval(Eo *obj, Efl_Ui_Panes_Data *sd)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   Eo *first_content, *second_content;
   Eina_Size2D min;

   first_content = efl_content_get(efl_part(obj, "first"));
   second_content = efl_content_get(efl_part(obj, "second"));

   if (first_content)
     {
        if (!sd->first_hint_min_allow)
          sd->first_min = efl_gfx_size_hint_combined_min_get(first_content);
        else
          sd->first_min = efl_gfx_size_hint_min_get(first_content);
     }

   if (second_content)
     {
        if (!sd->second_hint_min_allow)
          sd->second_min = efl_gfx_size_hint_combined_min_get(second_content);
        else
          sd->second_min = efl_gfx_size_hint_min_get(second_content);
     }

   if (sd->dir == EFL_UI_DIR_HORIZONTAL)
     {
        min.w = MAX(sd->first_min.w, sd->second_min.w);
        min.h = sd->first_min.h + sd->second_min.h;
     }
   else
     {
        min.w = sd->first_min.w + sd->second_min.w;
        min.h = MAX(sd->first_min.h, sd->second_min.h);
     }

   efl_gfx_size_hint_restricted_min_set(obj, min);
   _set_min_size_new(obj);
}

static void
_set_min_size_new(void *data)
{
   Eo *obj = data;
   EFL_UI_PANES_DATA_GET(obj, sd);
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   Eina_Size2D first_min = sd->first_min;
   Eina_Size2D second_min = sd->second_min;
   int w, h;
   double first_min_relative_size = 0.0, second_min_relative_size = 0.0;

   evas_object_geometry_get(wd->resize_obj, NULL, NULL, &w, &h);

   if (sd->dir == EFL_UI_DIR_HORIZONTAL)
     {
        if (first_min.h + second_min.h > h)
          {
             first_min_relative_size = first_min.h/(first_min.h + second_min.h);
             second_min_relative_size = second_min.h/(first_min.h + second_min.h);
          }
        else
          {
             if (h > 0)
               {
                  first_min_relative_size = first_min.h/(double)h;
                  second_min_relative_size = second_min.h/(double)h;
               }
          }

        first_min_relative_size = MAX(sd->first_min_split_ratio, first_min_relative_size);
        second_min_relative_size = MAX(sd->second_min_split_ratio, second_min_relative_size);

        edje_object_part_drag_value_set(wd->resize_obj, "right_constraint",
                                        0.0, 1.0 - second_min_relative_size);
        edje_object_part_drag_value_set(wd->resize_obj, "left_constraint",
                                        0.0, first_min_relative_size);
     }
   else
     {
        if (first_min.w + second_min.w > w)
          {
             first_min_relative_size = first_min.w/(first_min.w + second_min.w);
             second_min_relative_size = second_min.w/(first_min.w + second_min.w);
          }
        else
          {
             if (w > 0)
               {
                  first_min_relative_size = first_min.w/(double)w;
                  second_min_relative_size = second_min.w/(double)w;
               }
          }

        first_min_relative_size = MAX(sd->first_min_split_ratio, first_min_relative_size);
        second_min_relative_size = MAX(sd->second_min_split_ratio, second_min_relative_size);

        edje_object_part_drag_value_set(wd->resize_obj, "right_constraint",
                                        1.0 - second_min_relative_size, 0.0);
        edje_object_part_drag_value_set(wd->resize_obj, "left_constraint",
                                        first_min_relative_size, 0.0);
     }
}

static void
_set_min_size(void *data)
{
   EFL_UI_PANES_DATA_GET(data, sd);
   ELM_WIDGET_DATA_GET_OR_RETURN(data, wd);

   double sizer = sd->right_min_relative_size;
   double sizel = sd->left_min_relative_size;
   if ((sd->left_min_relative_size + sd->right_min_relative_size) > 1)
     {
        double sum = sizer + sizel;
        sizer = sizer / sum;
        sizel = sizel / sum;
     }
   if (sd->dir == EFL_UI_DIR_HORIZONTAL)
     {
        edje_object_part_drag_value_set
           (wd->resize_obj, "right_constraint", 0.0, (1 - sizer));
        edje_object_part_drag_value_set
           (wd->resize_obj, "left_constraint", 0.0, sizel);
     }
   else
     {
        edje_object_part_drag_value_set
           (wd->resize_obj, "right_constraint", (1 - sizer), 0.0);
        edje_object_part_drag_value_set
           (wd->resize_obj, "left_constraint", sizel, 0.0);
     }
}

static void
_update_fixed_sides(void *data)
{
   EFL_UI_PANES_DATA_GET(data, sd);
   ELM_WIDGET_DATA_GET_OR_RETURN(data, wd);
   Evas_Coord w, h;
   evas_object_geometry_get(wd->resize_obj, NULL, NULL, &w, &h);

   if (sd->right_min_size_is_relative)
     {
        if (sd->dir == EFL_UI_DIR_HORIZONTAL)
           sd->right_min_size = (int)(h * sd->right_min_relative_size);
        else
           sd->right_min_size =(int)(w * sd->right_min_relative_size);
     }
   else
     {
        sd->right_min_relative_size = 0;
        if (sd->dir == EFL_UI_DIR_HORIZONTAL && (h > 0))
              sd->right_min_relative_size = sd->right_min_size / (double)h;
        if (sd->dir == EFL_UI_DIR_VERTICAL && (w > 0))
              sd->right_min_relative_size = sd->right_min_size / (double)w;
     }

   if(sd->left_min_size_is_relative)
     {
        if (sd->dir == EFL_UI_DIR_HORIZONTAL)
             sd->left_min_size = (int)(h * sd->left_min_relative_size);
        else
           sd->left_min_size = (int)(w * sd->left_min_relative_size);
     }
   else
     {
        sd->left_min_relative_size = 0;
        if (sd->dir == EFL_UI_DIR_HORIZONTAL && (h > 0))
           sd->left_min_relative_size = sd->left_min_size / (double)h;
        if (sd->dir == EFL_UI_DIR_VERTICAL && (w > 0))
           sd->left_min_relative_size = sd->left_min_size / (double)w;
     }

   _set_min_size(data);
}

static void
_on_resize(void *data,
           Evas *e EINA_UNUSED,
           Evas_Object *obj EINA_UNUSED,
           void *event_info EINA_UNUSED)
{
  if (elm_widget_is_legacy(data)) _update_fixed_sides(data);
  else _set_min_size_new(data);
}

EOLIAN static void
_efl_ui_panes_efl_canvas_group_group_add(Eo *obj, Efl_Ui_Panes_Data *_pd EINA_UNUSED)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);
   EFL_UI_PANES_DATA_GET(obj, sd);

   efl_canvas_group_add(efl_super(obj, MY_CLASS));
   elm_widget_sub_object_parent_add(obj);

   if (!elm_layout_theme_set
       (obj, "panes", "vertical", elm_widget_style_get(obj)))
     CRI("Failed to set layout!");

   elm_panes_content_left_size_set(obj, 0.5);

   edje_object_signal_callback_add
     (wd->resize_obj, "elm,action,click", "*",
     _on_clicked, obj);
   edje_object_signal_callback_add
     (wd->resize_obj, "elm,action,click,double", "*",
     _double_clicked, obj);
   edje_object_signal_callback_add
     (wd->resize_obj, "elm,action,press", "*",
     _on_pressed, obj);
   edje_object_signal_callback_add
     (wd->resize_obj, "elm,action,unpress", "*",
     _on_unpressed, obj);
   evas_object_event_callback_add
     (wd->resize_obj, EVAS_CALLBACK_RESIZE,
     _on_resize, obj);

   sd->dir = EFL_UI_DIR_VERTICAL;
   sd->right_min_size_is_relative = EINA_TRUE;
   sd->left_min_size_is_relative = EINA_TRUE;
   sd->right_min_size = 0;
   sd->left_min_size = 0;
   sd->right_min_relative_size = 0;
   sd->left_min_relative_size = 0;
   if (elm_widget_is_legacy(obj)) _update_fixed_sides(obj);
   else _set_min_size_new(obj);

   elm_widget_can_focus_set(obj, EINA_FALSE);

   sd->event = evas_object_rectangle_add(evas_object_evas_get(obj));
   evas_object_color_set(sd->event, 0, 0, 0, 0);
   evas_object_pass_events_set(sd->event, EINA_TRUE);
   if (edje_object_part_exists
       (wd->resize_obj, "elm.swallow.event"))
     {
        Evas_Coord minw = 0, minh = 0;

        elm_coords_finger_size_adjust(1, &minw, 1, &minh);
        evas_object_size_hint_min_set(sd->event, minw, minh);
        elm_layout_content_set(obj, "elm.swallow.event", sd->event);
     }
   elm_widget_sub_object_add(obj, sd->event);

   elm_layout_sizing_eval(obj);
}

EOLIAN static Eo *
_efl_ui_panes_efl_object_constructor(Eo *obj, Efl_Ui_Panes_Data *_pd EINA_UNUSED)
{
   obj = efl_constructor(efl_super(obj, MY_CLASS));
   efl_canvas_object_type_set(obj, MY_CLASS_NAME_LEGACY);
   evas_object_smart_callbacks_descriptions_set(obj, _smart_callbacks);
   elm_interface_atspi_accessible_role_set(obj, ELM_ATSPI_ROLE_SPLIT_PANE);

   return obj;
}

EOLIAN static double
_efl_ui_panes_split_ratio_get(Eo *obj, Efl_Ui_Panes_Data *sd)
{
   double w, h;
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd, 0.0);

   edje_object_part_drag_value_get(wd->resize_obj, "elm.bar", &w, &h);

   if (sd->dir == EFL_UI_DIR_HORIZONTAL)
     return h;
   else return w;
}

EOLIAN static void
_efl_ui_panes_split_ratio_set(Eo *obj, Efl_Ui_Panes_Data *sd, double ratio)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   if (ratio < 0.0) ratio = 0.0;
   else if (ratio > 1.0) ratio = 1.0;

   if (sd->dir == EFL_UI_DIR_HORIZONTAL)
     edje_object_part_drag_value_set(wd->resize_obj, "elm.bar", 0.0, ratio);
   else
     edje_object_part_drag_value_set(wd->resize_obj, "elm.bar", ratio, 0.0);
}

EOLIAN static void
_efl_ui_panes_efl_ui_direction_direction_set(Eo *obj, Efl_Ui_Panes_Data *sd, Efl_Ui_Dir dir)
{
   double size = elm_panes_content_left_size_get(obj);
   if (efl_ui_dir_is_horizontal(dir, EINA_FALSE))
     dir = EFL_UI_DIR_HORIZONTAL;
   else
     dir = EFL_UI_DIR_VERTICAL;

   sd->dir = dir;
   elm_obj_widget_theme_apply(obj);
   if (elm_widget_is_legacy(obj)) _update_fixed_sides(obj);
   else _set_min_size_new(obj);

   elm_panes_content_left_size_set(obj, size);
}

EOLIAN static Efl_Ui_Dir
_efl_ui_panes_efl_ui_direction_direction_get(Eo *obj EINA_UNUSED, Efl_Ui_Panes_Data *sd)
{
   return sd->dir;
}

EOLIAN static void
_efl_ui_panes_fixed_set(Eo *obj, Efl_Ui_Panes_Data *sd, Eina_Bool fixed)
{
   sd->fixed = !!fixed;
   if (sd->fixed == EINA_TRUE)
     {
        elm_layout_signal_emit(obj, "elm,panes,fixed", "elm");

        //TODO: remove this signal on EFL 2.0.
        // I left this due to the backward compatibility.
        elm_layout_signal_emit(obj, "elm.panes.fixed", "elm");
     }
   else
     {
        elm_layout_signal_emit(obj, "elm,panes,unfixed", "elm");

        //TODO: remove this signal on EFL 2.0.
        // I left this due to the backward compatibility.
        elm_layout_signal_emit(obj, "elm.panes.unfixed", "elm");
     }
}

EOLIAN static Eina_Bool
_efl_ui_panes_fixed_get(Eo *obj EINA_UNUSED, Efl_Ui_Panes_Data *sd)
{
   return sd->fixed;
}

static void
_efl_ui_panes_class_constructor(Efl_Class *klass)
{
   evas_smart_legacy_type_register(MY_CLASS_NAME_LEGACY, klass);
}

/* Efl.Part begin */

ELM_PART_OVERRIDE_ONLY_ALIASES(efl_ui_panes, EFL_UI_PANES, Efl_Ui_Panes_Data, _content_aliases)

EOLIAN static void
_efl_ui_panes_part_hint_min_allow_set(Eo *obj, void *_pd EINA_UNUSED, Eina_Bool allow)
{
   Elm_Part_Data *pd = efl_data_scope_get(obj, EFL_UI_WIDGET_PART_CLASS);
   Efl_Ui_Panes_Data *sd = efl_data_scope_get(pd->obj, EFL_UI_PANES_CLASS);

   if (!strcmp(pd->part, "first"))
     {
        if (sd->first_hint_min_allow == allow) return;
        sd->first_hint_min_allow = allow;
        elm_layout_sizing_eval(pd->obj);
     }
   else if (!strcmp(pd->part, "second"))
     {
        if (sd->second_hint_min_allow == allow) return;
        sd->second_hint_min_allow = allow;
        elm_layout_sizing_eval(pd->obj);
     }
}

EOLIAN static Eina_Bool
_efl_ui_panes_part_hint_min_allow_get(Eo *obj, void *_pd EINA_UNUSED)
{
   Eina_Bool ret = EINA_FALSE;
   Elm_Part_Data *pd = efl_data_scope_get(obj, EFL_UI_WIDGET_PART_CLASS);
   Efl_Ui_Panes_Data *sd = efl_data_scope_get(pd->obj, EFL_UI_PANES_CLASS);

   if (!strcmp(pd->part, "first"))
     {
        ret = sd->first_hint_min_allow;
     }
   else if (!strcmp(pd->part, "second"))
     {
        ret = sd->second_hint_min_allow;
     }

   return ret;
}

EOLIAN static double
_efl_ui_panes_part_split_ratio_min_get(Eo *obj, void *_pd EINA_UNUSED)
{
   Elm_Part_Data *pd = efl_data_scope_get(obj, EFL_UI_WIDGET_PART_CLASS);
   Efl_Ui_Panes_Data *sd = efl_data_scope_get(pd->obj, EFL_UI_PANES_CLASS);
   double ret = 0.0;

   if (!strcmp(pd->part, "first"))
     ret = sd->first_min_split_ratio;
   else if (!strcmp(pd->part, "second"))
     ret = sd->second_min_split_ratio;

   return ret;
}

EOLIAN static void
_efl_ui_panes_part_split_ratio_min_set(Eo *obj, void *_pd EINA_UNUSED, double ratio)
{
   Elm_Part_Data *pd = efl_data_scope_get(obj, EFL_UI_WIDGET_PART_CLASS);
   Efl_Ui_Panes_Data *sd = efl_data_scope_get(pd->obj, EFL_UI_PANES_CLASS);

   if (!strcmp(pd->part, "first"))
     {
        sd->first_min_split_ratio = ratio;
        if (sd->first_min_split_ratio < 0) sd->first_min_split_ratio = 0;
        _set_min_size_new(pd->obj);
     }
   else if (!strcmp(pd->part, "second"))
     {
        sd->second_min_split_ratio = ratio;
        if (sd->second_min_split_ratio < 0) sd->second_min_split_ratio = 0;
        _set_min_size_new(pd->obj);
     }
}

#include "efl_ui_panes_part.eo.c"

/* Efl.Part end */

/* Internal EO APIs and hidden overrides */

ELM_LAYOUT_CONTENT_ALIASES_IMPLEMENT(efl_ui_panes)

#define EFL_UI_PANES_EXTRA_OPS \
   EFL_CANVAS_GROUP_ADD_OPS(efl_ui_panes), \
   ELM_LAYOUT_CONTENT_ALIASES_OPS(efl_ui_panes), \
   ELM_LAYOUT_SIZING_EVAL_OPS(efl_ui_panes)

/* Legacy APIs */

EAPI Evas_Object *
elm_panes_add(Evas_Object *parent)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(parent, NULL);
   return efl_add(MY_CLASS, parent, efl_canvas_object_legacy_ctor(efl_added));
}

EAPI void
elm_panes_content_left_min_size_set(Evas_Object *obj, int size)
{
   EFL_UI_PANES_DATA_GET(obj, sd);

   sd->left_min_size = size;
   if (sd->left_min_size < 0) sd->left_min_size = 0;
   sd->left_min_size_is_relative = EINA_FALSE;
   _update_fixed_sides(obj);
}

EAPI int
elm_panes_content_left_min_size_get(const Evas_Object *obj)
{
   EFL_UI_PANES_DATA_GET(obj, sd);
   return sd->left_min_size;
}

EAPI void
elm_panes_content_right_min_size_set(Evas_Object *obj, int size)
{
   EFL_UI_PANES_DATA_GET(obj, sd);

   sd->right_min_size = size;
   if (sd->right_min_size < 0) sd->right_min_size = 0;
   sd->right_min_size_is_relative = EINA_FALSE;
   _update_fixed_sides(obj);
}

EAPI int
elm_panes_content_right_min_size_get(const Evas_Object *obj)
{
   EFL_UI_PANES_DATA_GET(obj, sd);
   return sd->right_min_size;
}

EAPI double
elm_panes_content_left_size_get(const Evas_Object *obj)
{
   return efl_ui_panes_split_ratio_get(obj);
}

EAPI void
elm_panes_content_left_size_set(Evas_Object *obj, double size)
{
   efl_ui_panes_split_ratio_set(obj, size);
}

EAPI double
elm_panes_content_right_size_get(const Evas_Object *obj)
{
   return 1.0 - elm_panes_content_left_size_get(obj);
}

EAPI void
elm_panes_content_right_size_set(Evas_Object *obj, double size)
{
   elm_panes_content_left_size_set(obj, (1.0 - size));
}

EAPI void
elm_panes_content_left_min_relative_size_set(Evas_Object *obj, double size)
{
   EFL_UI_PANES_DATA_GET(obj, sd);
   sd->left_min_relative_size = size;
   if (sd->left_min_relative_size < 0) sd->left_min_relative_size = 0;
   sd->left_min_size_is_relative = EINA_TRUE;
   _update_fixed_sides(obj);
}

EAPI double
elm_panes_content_left_min_relative_size_get(const Evas_Object *obj)
{
   EFL_UI_PANES_DATA_GET(obj, sd);
   return sd->left_min_relative_size;
}

EAPI void
elm_panes_content_right_min_relative_size_set(Evas_Object *obj, double size)
{
   EFL_UI_PANES_DATA_GET(obj, sd);

   sd->right_min_relative_size = size;
   if (sd->right_min_relative_size < 0) sd->right_min_relative_size = 0;
   sd->right_min_size_is_relative = EINA_TRUE;
   _update_fixed_sides(obj);
}

EAPI double
elm_panes_content_right_min_relative_size_get(const Evas_Object *obj)
{
   EFL_UI_PANES_DATA_GET(obj, sd);
   return sd->right_min_relative_size;
}

EAPI void
elm_panes_horizontal_set(Evas_Object *obj, Eina_Bool horizontal)
{
   Efl_Ui_Dir dir;

   if (horizontal)
     dir = EFL_UI_DIR_HORIZONTAL;
   else
     dir = EFL_UI_DIR_VERTICAL;

   efl_ui_direction_set(obj, dir);
}

EAPI Eina_Bool
elm_panes_horizontal_get(const Evas_Object *obj)
{
   Efl_Ui_Dir dir = efl_ui_direction_get(obj);

   if (dir == EFL_UI_DIR_HORIZONTAL)
     return EINA_TRUE;

   return EINA_FALSE;
}

EINA_DEPRECATED EAPI void
elm_panes_content_left_set(Evas_Object *obj,
                           Evas_Object *content)
{
   elm_layout_content_set(obj, "left", content);
}

EINA_DEPRECATED EAPI void
elm_panes_content_right_set(Evas_Object *obj,
                            Evas_Object *content)
{
   elm_layout_content_set(obj, "right", content);
}

EINA_DEPRECATED EAPI Evas_Object *
elm_panes_content_left_get(const Evas_Object *obj)
{
   return elm_layout_content_get(obj, "left");
}

EINA_DEPRECATED EAPI Evas_Object *
elm_panes_content_right_get(const Evas_Object *obj)
{
   return elm_layout_content_get(obj, "right");
}

EINA_DEPRECATED EAPI Evas_Object *
elm_panes_content_left_unset(Evas_Object *obj)
{
   return elm_layout_content_unset(obj, "left");
}

EINA_DEPRECATED EAPI Evas_Object *
elm_panes_content_right_unset(Evas_Object *obj)
{
   return elm_layout_content_unset(obj, "right");
}

/* Legacy APIs end  */

#include "efl_ui_panes.eo.c"
