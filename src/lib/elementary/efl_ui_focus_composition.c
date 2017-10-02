#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#include <Elementary.h>
#include "elm_priv.h"
#include "efl_ui_focus_composition.eo.h"
#include "efl_ui_focus_composition_adapter.eo.h"

#define MY_CLASS EFL_UI_FOCUS_COMPOSITION_MIXIN

typedef struct {
   Eina_List *order, *targets_ordered;
   Eina_List *register_target, *registered_targets;
   Eina_List *adapters;
   Efl_Ui_Focus_Manager *registered;
} Efl_Ui_Focus_Composition_Data;

static void
_state_apply(Eo *obj, Efl_Ui_Focus_Composition_Data *pd)
{
   if (!pd->registered && pd->registered_targets)
     {
        Efl_Ui_Focus_Object *o;
        //remove all of them
        EINA_LIST_FREE(pd->registered_targets, o)
          {
             efl_ui_focus_manager_calc_unregister(pd->registered, o);
          }
     }
   else if (pd->registered)
     {
        Eina_List *n;
        Efl_Ui_Focus_Object *o;
        //remove all of them
        EINA_LIST_FREE(pd->registered_targets, o)
          {
             efl_ui_focus_manager_calc_unregister(pd->registered, o);
          }

        EINA_LIST_FOREACH(pd->register_target, n, o)
          {
             efl_ui_focus_manager_calc_register(pd->registered, o, obj, NULL);
             pd->registered_targets = eina_list_append(pd->registered_targets, o);
          }

        efl_ui_focus_manager_calc_update_order(pd->registered, obj, eina_list_clone(pd->targets_ordered));
     }
}

EOLIAN static void
_efl_ui_focus_composition_composition_elements_set(Eo *obj, Efl_Ui_Focus_Composition_Data *pd, Eina_List *logical_order)
{
   Efl_Ui_Focus_Composition_Adapter *adapter;
   Evas_Object *elem;
   Eina_List *n;

   pd->targets_ordered = eina_list_free(pd->targets_ordered);
   pd->register_target = eina_list_free(pd->register_target);

   pd->order = eina_list_free(pd->order);
   pd->order = logical_order;

   //get rid of all adapter objects
   EINA_LIST_FREE(pd->adapters, adapter)
     {
        efl_del(adapter);
     }

   //now build a composition_elements list
   EINA_LIST_FOREACH(logical_order, n, elem)
     {
        Efl_Ui_Focus_Object *o = elem;

        EINA_SAFETY_ON_NULL_GOTO(elem, cont);

        if (!efl_isa(elem, ELM_WIDGET_CLASS))
          {
             if (efl_isa(elem, EFL_UI_FOCUS_OBJECT_MIXIN))
               {
                  pd->register_target = eina_list_append(pd->register_target , o);
               }
             else if (efl_isa(elem, EFL_GFX_INTERFACE))
               {
                  o = efl_add(EFL_UI_FOCUS_COMPOSITION_ADAPTER_CLASS, NULL, efl_ui_focus_composition_adapter_canvas_object_set(efl_added, elem));

                  pd->adapters = eina_list_append(pd->adapters, o);
                  pd->register_target = eina_list_append(pd->register_target , o);
               }
             else
               {
                  EINA_SAFETY_ERROR("List contains element that is not EFL_UI_FOCUS_OBJECT_MIXIN or EFL_GFX_INTERFACE or ELM_WIDGET_CLASS");
                  continue;
               }
          }

        pd->targets_ordered = eina_list_append(pd->targets_ordered, o);
cont:
        continue;
     }
   _state_apply(obj, pd);
}

EOLIAN static Eina_List*
_efl_ui_focus_composition_composition_elements_get(Eo *obj EINA_UNUSED, Efl_Ui_Focus_Composition_Data *pd)
{
   return eina_list_clone(pd->order);
}

EOLIAN static Eina_Bool
_efl_ui_focus_composition_elm_widget_focus_state_apply(Eo *obj, Efl_Ui_Focus_Composition_Data *pd, Elm_Widget_Focus_State current_state, Elm_Widget_Focus_State *configured_state, Elm_Widget *redirect)
{
   Eina_Bool registered;

   configured_state->logical = EINA_TRUE;

   //shortcut for having the same configurations
   if (current_state.manager == configured_state->manager && !current_state.manager)
     return !!current_state.manager;

   if (configured_state->logical == current_state.logical &&
       configured_state->manager == current_state.manager &&
       configured_state->parent == current_state.parent)
     return !!current_state.manager;

   registered = elm_obj_widget_focus_state_apply(efl_super(obj, MY_CLASS), current_state, configured_state, redirect);

   if (registered)
     pd->registered = configured_state->manager;
   else
     pd->registered = NULL;

   _state_apply(obj, pd);

   return registered;
}

#include "efl_ui_focus_composition.eo.c"

typedef struct {
   Evas_Object *object;
}  Efl_Ui_Focus_Composition_Adapter_Data;

EOLIAN static void
_efl_ui_focus_composition_adapter_canvas_object_set(Eo *obj EINA_UNUSED, Efl_Ui_Focus_Composition_Adapter_Data *pd, Efl_Canvas_Object *v)
{
   pd->object = v;
}

EOLIAN static Efl_Canvas_Object*
_efl_ui_focus_composition_adapter_canvas_object_get(Eo *obj EINA_UNUSED, Efl_Ui_Focus_Composition_Adapter_Data *pd)
{
   return pd->object;
}

EOLIAN static Eina_Rect
_efl_ui_focus_composition_adapter_efl_ui_focus_object_focus_geometry_get(Eo *obj EINA_UNUSED, Efl_Ui_Focus_Composition_Adapter_Data *pd EINA_UNUSED)
{
   return efl_gfx_geometry_get(pd->object);
}

EOLIAN static void
_efl_ui_focus_composition_adapter_efl_ui_focus_object_focus_set(Eo *obj EINA_UNUSED, Efl_Ui_Focus_Composition_Adapter_Data *pd, Eina_Bool focus)
{
   efl_ui_focus_object_focus_set(efl_super(obj, EFL_UI_FOCUS_COMPOSITION_ADAPTER_CLASS), focus);

   evas_object_focus_set(pd->object, efl_ui_focus_object_focus_get(obj));
}

#include "efl_ui_focus_composition_adapter.eo.c"
