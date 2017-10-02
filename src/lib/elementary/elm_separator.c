#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED

#include <Elementary.h>

#include "elm_priv.h"
#include "elm_separator.eo.h"
#include "elm_widget_separator.h"
#include "elm_widget_layout.h"

#define MY_CLASS ELM_SEPARATOR_CLASS

#define MY_CLASS_NAME "Elm_Separator"
#define MY_CLASS_NAME_LEGACY "elm_separator"

EOLIAN static Efl_Ui_Theme_Apply
_elm_separator_elm_widget_theme_apply(Eo *obj, Elm_Separator_Data *sd EINA_UNUSED)
{
   Efl_Ui_Theme_Apply int_ret = EFL_UI_THEME_APPLY_FAILED;
   EFL_UI_LAYOUT_DATA_GET(obj, ld);

   if (sd->horizontal)
     eina_stringshare_replace(&ld->group, "horizontal");
   else
     eina_stringshare_replace(&ld->group, "vertical");

   int_ret = elm_obj_widget_theme_apply(efl_super(obj, MY_CLASS));
   if (!int_ret) return EFL_UI_THEME_APPLY_FAILED;

   return int_ret;
}

EOLIAN static void
_elm_separator_elm_layout_sizing_eval(Eo *obj, Elm_Separator_Data *sd EINA_UNUSED)
{
   Evas_Coord minw = -1, minh = -1;
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   edje_object_size_min_calc(wd->resize_obj, &minw, &minh);
   evas_object_size_hint_min_set(obj, minw, minh);
   evas_object_size_hint_max_set(obj, -1, -1);
   evas_object_size_hint_align_set(obj, EVAS_HINT_FILL, EVAS_HINT_FILL);
}

EOLIAN static void
_elm_separator_efl_canvas_group_group_add(Eo *obj, Elm_Separator_Data *sd EINA_UNUSED)
{
   efl_canvas_group_add(efl_super(obj, MY_CLASS));
   elm_widget_sub_object_parent_add(obj);
   elm_widget_can_focus_set(obj, EINA_FALSE);

   if (!elm_layout_theme_set
       (obj, "separator", "vertical", elm_widget_style_get(obj)))
     CRI("Failed to set layout!");

   elm_layout_sizing_eval(obj);
}

EAPI Evas_Object *
elm_separator_add(Evas_Object *parent)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(parent, NULL);
   return efl_add(MY_CLASS, parent, efl_canvas_object_legacy_ctor(efl_added));
}

EOLIAN static Eo *
_elm_separator_efl_object_constructor(Eo *obj, Elm_Separator_Data *sd EINA_UNUSED)
{
   obj = efl_constructor(efl_super(obj, MY_CLASS));
   efl_canvas_object_type_set(obj, MY_CLASS_NAME_LEGACY);
   elm_interface_atspi_accessible_role_set(obj, ELM_ATSPI_ROLE_SEPARATOR);

   return obj;
}

EOLIAN static void
_elm_separator_horizontal_set(Eo *obj, Elm_Separator_Data *sd, Eina_Bool horizontal)
{
   horizontal = !!horizontal;
   if (sd->horizontal == horizontal) return;

   sd->horizontal = horizontal;

   elm_obj_widget_theme_apply(obj);
}

EOLIAN static Eina_Bool
_elm_separator_horizontal_get(Eo *obj EINA_UNUSED, Elm_Separator_Data *sd)
{
   return sd->horizontal;
}

EOLIAN static void
_elm_separator_class_constructor(Efl_Class *klass)
{
   evas_smart_legacy_type_register(MY_CLASS_NAME_LEGACY, klass);
}

/* Internal EO APIs and hidden overrides */

#define ELM_SEPARATOR_EXTRA_OPS \
   ELM_LAYOUT_SIZING_EVAL_OPS(elm_separator), \
   EFL_CANVAS_GROUP_ADD_OPS(elm_separator)

#include "elm_separator.eo.c"
