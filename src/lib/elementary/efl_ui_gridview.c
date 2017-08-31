#define EFL_UI_GRIDVIEW_PRIVATE
#define EFL_UI_GRIDVIEW_PROTECTED

#include "efl_ui_gridview_private.h"
#include <assert.h>
// Debug Macros : Should be removed before master merge
#include "sh_log.h"
// End

#define MY_CLASS          EFL_UI_GRIDVIEW_CLASS
#define MY_CLASS_NAME     "Efl.Ui.Gridview"
#define MY_PAN_CLASS      EFL_UI_GRIDVIEW_PAN_CLASS

#define SIG_CHILD_ADDED   "child,added"
#define SIG_CHILD_REMOVED "child,removed"
#define SELECTED_PROP     "selected"
#define AVERAGE_SIZE_INIT 30
#define SLICE_MIN         20

static const Evas_Smart_Cb_Description _smart_callbacks[] = {
   {SIG_CHILD_ADDED, ""},
   {SIG_CHILD_REMOVED, ""},
   {NULL, NULL}
};
static void                  _efl_ui_gridview_item_select_set(Efl_Ui_Gridview_Item *, Eina_Bool);
static Eina_Bool             _efl_ui_gridview_item_select_clear(Eo *);

static void                  _item_calc(Efl_Ui_Gridview_Data *, Efl_Ui_Gridview_Item *);
static void                  _layout_realize(Efl_Ui_Gridview_Data *, Efl_Ui_Gridview_Item *);
static void                  _layout_unrealize(Efl_Ui_Gridview_Data *, Efl_Ui_Gridview_Item *);
static Eina_Bool             _update_items(Eo *, Efl_Ui_Gridview_Data *);
static void                  _custom_layout_internal(Eo *);

static void                  _insert_at(Efl_Ui_Gridview_Data *pd, int index, Efl_Model *child);
static void                  _remove_at(Efl_Ui_Gridview_Data *pd, int index);

static Eina_Bool             _key_action_move(Evas_Object *obj, const char *params);
static Eina_Bool             _key_action_select(Evas_Object *obj, const char *params);
static Eina_Bool             _key_action_escape(Evas_Object *obj, const char *params);

static const Elm_Action key_actions[] = {
   {"move", _key_action_move},
   {"select", _key_action_select},
   {"escape", _key_action_escape},
   {NULL, NULL}
};

static inline Eina_Bool
_horiz(Efl_Orient dir)
{
   return dir % 180 == EFL_ORIENT_RIGHT;
}

EOLIAN static void
_efl_ui_gridview_pan_elm_pan_pos_set(Eo *obj,
                                     Efl_Ui_Gridview_Pan_Data *ppd,
                                     Evas_Coord x,
                                     Evas_Coord y)
{
   Evas_Coord ox, oy, ow, oh, cw;
   Efl_Ui_Gridview_Data *pd = ppd->wpd;
   Efl_Ui_Gridview_Item *gitem;

   SHINF("x:["_CR_"%d"_CR_"] y:["_CR_"%d"_CR_"]",
         CRBLD(CRRED, BGBLK), x, CRINF,
         CRBLD(CRRED, BGBLK), y, CRINF);

   EINA_SAFETY_ON_NULL_RETURN(pd);
   if (((x == pd->pan.x) && (y == pd->pan.y))) return;

   efl_gfx_geometry_get(pd->obj, &ox, &oy, &ow, &oh);
   if (_horiz(pd->direct))
     {
        pd->pan.move_diff += x - pd->pan.x;
        cw = ow / 4;
     }
   else
     {
        pd->pan.move_diff += y - pd->pan.y;
        cw = oh / 4;
     }

   pd->pan.x = x;
   pd->pan.y = y;

   if (abs(pd->pan.move_diff) > cw)
     {
//DEBUG-START
        SHCRI("move ["_CR_"%d"_CR_"] more then current width :["_CR_"%d"_CR_"]",
        CRBLD(CRRED, BGBLK), pd->pan.move_diff, CRINF,
        CRBLD(CRRED, BGBLK), cw, CRINF);
//DEBUG-END
        pd->pan.move_diff = 0;
        _update_items(obj, pd);
     }
   else
     {
        EINA_INARRAY_FOREACH(&pd->items.array, gitem)
          efl_gfx_position_set(gitem->layout, (gitem->geo.x + 0 - pd->pan.x), (gitem->geo.y + 0 - pd->pan.y));
     }
}

EOLIAN static void
_efl_ui_gridview_pan_elm_pan_pos_get(Eo *obj EINA_UNUSED,
                                     Efl_Ui_Gridview_Pan_Data *ppd,
                                     Evas_Coord *x,
                                     Evas_Coord *y)
{
   Efl_Ui_Gridview_Data *pd = ppd->wpd;

   if (x) *x = pd->pan.x;
   if (y) *y = pd->pan.y;
}


EOLIAN static void
_efl_ui_gridview_pan_elm_pan_pos_max_get(Eo *obj EINA_UNUSED,
                                         Efl_Ui_Gridview_Pan_Data *ppd,
                                         Evas_Coord *x,
                                         Evas_Coord *y)
{
   Evas_Coord ow, oh;

   elm_interface_scrollable_content_viewport_geometry_get
     (ppd->wobj, NULL, NULL, &ow, &oh);
   ow = ppd->wpd->minw - ow;
   if (ow < 0) ow = 0;
   oh = ppd->wpd->minh - oh;
   if (oh < 0) oh = 0;

   if (x) *x = ow;
   if (y) *y = oh;

   SHINF(" ---- [%d : %d]", ow, oh);
}

EOLIAN static void
_efl_ui_gridview_pan_elm_pan_pos_min_get(Eo *obj EINA_UNUSED,
                                         Efl_Ui_Gridview_Pan_Data * ppd EINA_UNUSED,
                                         Evas_Coord * x,
                                         Evas_Coord * y)
{
   if (x) *x = 0;
   if (y) *y = 0;
}

EOLIAN static void
_efl_ui_gridview_pan_elm_pan_content_size_get(Eo *obj EINA_UNUSED,
                                              Efl_Ui_Gridview_Pan_Data *ppd,
                                              Evas_Coord *w,
                                              Evas_Coord *h)
{
   Efl_Ui_Gridview_Data *pd = ppd->wpd;
   EINA_SAFETY_ON_NULL_RETURN(pd);

   if (w) *w = pd->minw;
   if (h) *h = pd->minh;

   SHINF(" ---- [%d : %d]", pd->minw, pd->minh);
}

EOLIAN static void
_efl_ui_gridview_pan_efl_object_destructor(Eo *obj,
                                           Efl_Ui_Gridview_Pan_Data *ppd)
{
   efl_data_unref(ppd->wobj, ppd->wpd);
   efl_destructor(efl_super(obj, MY_PAN_CLASS));
}

EOLIAN static void
_efl_ui_gridview_pan_efl_canvas_group_group_calculate(Eo *obj,
                                                      Efl_Ui_Gridview_Pan_Data *ppd EINA_UNUSED)
{
   Efl_Ui_Gridview_Data *pd = ppd->wpd;
   ELM_WIDGET_DATA_GET_OR_RETURN(pd->obj, wd);

   int w, h, rw, rh, cw, ch;
   efl_gfx_size_get(wd->resize_obj, &rw, &rh);
   elm_interface_scrollable_content_viewport_geometry_get(pd->obj, NULL, NULL, &w, &h);

   elm_interface_scrollable_content_size_get(pd->obj, &cw, &ch);

   SHDBG(" pan calculate! ["_CR_"%d, %d : %d, %d -- %d, %d"_CR_"]", CRBLD(CRRED,BGBLK), w, h, rw, rh, cw, ch, CRDBG);

   efl_canvas_group_calculate(efl_super(obj, MY_PAN_CLASS));
}


#include "efl_ui_gridview_pan.eo.c"

static Eina_Bool
_efl_model_properties_has(Efl_Model *model,
                          Eina_Stringshare *propfind)
{
   const Eina_Array *properties;
   Eina_Array_Iterator iter_prop;
   Eina_Stringshare *property;
   Eina_Bool ret = EINA_FALSE;
   unsigned i = 0;

   EINA_SAFETY_ON_NULL_RETURN_VAL(model, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(propfind, EINA_FALSE);

   properties = efl_model_properties_get(model);

   EINA_ARRAY_ITER_NEXT(properties, i, property, iter_prop)
     {
        if (property == propfind)
          {
             ret = EINA_TRUE;
             break;
          }
     }

   return ret;
}

static void
_child_added_cb(void *data,
                const Efl_Event *event EINA_UNUSED)
{
   Efl_Model_Children_Event *evt = event->info;
   Efl_Ui_Gridview *obj = data;
   EFL_UI_GRIDVIEW_DATA_GET_OR_RETURN(obj, pd);
   int index = evt->index - pd->realized.start;

   if (index >= 0 && index <= pd->realized.slice)
     _insert_at(pd, index, evt->child);

   efl_canvas_group_change(pd->obj);
}

static void
_child_removed_cb(void *data,
                  const Efl_Event *event)
{
   Efl_Model_Children_Event *evt = event->info;
   Efl_Ui_Gridview *obj = data;
   EFL_UI_GRIDVIEW_DATA_GET_OR_RETURN(obj, pd);
   int index = evt->index - pd->realized.start;

   SHINF("%d index", index);
   if (index >= 0 && index < pd->realized.slice)
     _remove_at(pd, index);

   efl_canvas_group_change(pd->obj);
   efl_ui_gridview_custom_layout(pd->obj, EINA_TRUE);
}

static void
_on_item_unfocused(void *data)//const Efl_Event *event EINA_UNUSED)
{
   Efl_Ui_Gridview_Item *item = data;
   EFL_UI_GRIDVIEW_DATA_GET_OR_RETURN(item->widget, pd);

   if (pd->focused != item) return;

   if (!_elm_config->item_select_on_focus_disable)
     _efl_ui_gridview_item_select_set(item, EINA_FALSE);

   pd->focused = NULL;
}

static void
_on_item_focused(void *data,
                 const Efl_Event *event EINA_UNUSED)
{
   Efl_Ui_Gridview_Item *item = data;

   EFL_UI_GRIDVIEW_DATA_GET_OR_RETURN(item->widget, pd);

   //FIXME: There are no Unfocused callback in Focus Manager!
   if (pd->focused) _on_item_unfocused((void *)pd->focused);

   if (!_elm_config->item_select_on_focus_disable)
     _efl_ui_gridview_item_select_set(item, EINA_TRUE);

   pd->focused = item;
}

static void
_long_press_cb(void *data,
               const Efl_Event *event)
{
   Efl_Ui_Gridview_Item *item = data;

   item->timer = NULL;
   item->longpressed = EINA_TRUE;
   if (item->layout)
     efl_event_callback_legacy_call(item->layout, EFL_UI_EVENT_LONGPRESSED, item);

   efl_del(event->object);
}

static void
_on_item_mouse_down(void *data,
                    Evas *evas EINA_UNUSED,
                    Evas_Object *o EINA_UNUSED,
                    void *event_info)
{
   Evas_Event_Mouse_Down *ev = event_info;
   Efl_Ui_Gridview_Item *item = data;

   SHINF("item: %p layout: %p model: %p list: %p evas: %p", item, item->layout, item->model, item->widget, evas);

   EFL_UI_GRIDVIEW_DATA_GET_OR_RETURN(item->widget, pd);

   if (ev->button != 1) return;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD) pd->on_hold = EINA_TRUE;
   else pd->on_hold = EINA_FALSE;
   if (pd->on_hold) return;

   item->down = EINA_TRUE;
   assert(item->longpressed == EINA_FALSE);

   efl_del(item->timer);
   item->timer = efl_add(EFL_LOOP_TIMER_CLASS, efl_loop_main_get(EFL_LOOP_CLASS),
                         efl_event_callback_add(efl_added, EFL_LOOP_TIMER_EVENT_TICK, _long_press_cb, item),
                         efl_loop_timer_interval_set(efl_added, _elm_config->longpress_timeout));
}

static void
_on_item_mouse_up(void *data,
                  Evas *evas EINA_UNUSED,
                  Evas_Object *o EINA_UNUSED,
                  void *event_info)
{
   Evas_Event_Mouse_Down *ev = event_info;
   Efl_Ui_Gridview_Item *item = data;

   SHINF("item: %p layout: %p model: %p list: %p evas: %p", item, item->layout, item->model, item->widget, evas);
   EFL_UI_GRIDVIEW_DATA_GET_OR_RETURN(item->widget, pd);

   if (ev->button != 1) return;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD) pd->on_hold = EINA_TRUE;
   else pd->on_hold = EINA_FALSE;
   if (pd->on_hold || !item->down) return;

   item->down = EINA_FALSE;
   if (item->timer) efl_del(item->timer);
   item->timer = NULL;

   if (!item->longpressed)
     {
        if (pd->select_mode != ELM_OBJECT_SELECT_MODE_ALWAYS && item->selected)
          return;

        _efl_ui_gridview_item_select_set(item, EINA_TRUE);
     }
   else
     item->longpressed = EINA_FALSE;
}

static void
_item_selected_then(void *data,
                    const Efl_Event *event)
{
   Efl_Ui_Gridview_Item *item = data;
   EINA_SAFETY_ON_NULL_RETURN(item);
   Eina_Stringshare *selected;
   const Eina_Value_Type *vtype;
   Eina_Value *value = (Eina_Value *)((Efl_Future_Event_Success *)event->info)->value;

   item->future = NULL;
   vtype = eina_value_type_get(value);

   if (vtype == EINA_VALUE_TYPE_STRING || vtype == EINA_VALUE_TYPE_STRINGSHARE)
     {
        eina_value_get(value, &selected);
        Eina_Bool s = (strcmp(selected, "selected") ? EINA_FALSE : EINA_TRUE);

        if (item->selected == s) return;
        item->selected = s;

        EFL_UI_GRIDVIEW_DATA_GET_OR_RETURN(item->widget, pd);

        if (item->selected)
          pd->selected_items = eina_list_append(pd->selected_items, item);
        else
          pd->selected_items = eina_list_remove(pd->selected_items, item);
     }
}

static void
_count_then(void *data,
            const Efl_Event *event)
{
   Efl_Ui_Gridview_Data *pd = data;
   EINA_SAFETY_ON_NULL_RETURN(pd);
   int *count = ((Efl_Future_Event_Success *)event->info)->value;

   pd->items.count = *count;
   _update_items(pd->obj, pd);
}

static void
_count_error(void *data,
             const Efl_Event *event EINA_UNUSED)
{
   Efl_Ui_Gridview_Data *pd = data;
   EINA_SAFETY_ON_NULL_RETURN(pd);
}

static void
_item_style_property_then(void *data,
                          const Efl_Event *event)
{
   Efl_Ui_Gridview_Item *item = data;
   EINA_SAFETY_ON_NULL_RETURN(item);
   Eina_Value *value = (Eina_Value *)((Efl_Future_Event_Success *)event->info)->value;
   const Eina_Value_Type *vtype = eina_value_type_get(value);
   char *style = NULL;

   item->future = NULL;
   if (vtype == EINA_VALUE_TYPE_STRING || vtype == EINA_VALUE_TYPE_STRINGSHARE)
     eina_value_get(value, &style);

   elm_object_style_set(item->layout, style);
}

static void
_item_property_error(void *data,
                     const Efl_Event *event EINA_UNUSED)
{
   Efl_Ui_Gridview_Item *item = data;
   EINA_SAFETY_ON_NULL_RETURN(item);

   item->future = NULL;
}

static void
_efl_model_properties_changed_cb(void *data,
                                 const Efl_Event *event)
{
   Efl_Ui_Gridview_Item *item = data;
   EINA_SAFETY_ON_NULL_RETURN(item);
   Efl_Model_Property_Event *evt = event->info;
   Eina_Array_Iterator it;
   Eina_Stringshare *prop, *sprop;
   unsigned int i;

   if (!evt->changed_properties) return;

   sprop = eina_stringshare_add(SELECTED_PROP);
   EINA_ARRAY_ITER_NEXT(evt->changed_properties, i, prop, it)
     {
        if (prop == sprop)
          {
             item->future = efl_model_property_get(item->model, sprop);
             efl_future_then(item->future, &_item_selected_then, &_item_property_error, NULL, item);
          }
     }
}

static GV_Item_Line *
_gv_item_line_item_add(Efl_Ui_Gridview_Data *pd,
                       Efl_Ui_Gridview_Item *item,
                       Evas_Coord w,
                       Evas_Coord h)
{
   GV_Item_Line *line = pd->lines.end;
   Efl_Ui_Gridview_Item *prev = line ?
     eina_list_data_get(eina_list_last(line->items)) : NULL;
   int vw, vh;
   int horiz = _horiz(pd->direct);

   //if (prev && eina_inarray_next(prev)

   efl_gfx_size_get(pd->pan.obj, &vw, &vh);

  if (!line ||
      (horiz && (vh < (h + line->geo.h))) ||
      (!horiz && (vw < (w + line->geo.w))))
    {
       line = calloc(1, sizeof(GV_Item_Line));
       if (!pd->lines.first) pd->lines.first = EINA_INLIST_GET(line);
       pd->lines.end = eina_inlist_append(pd->lines.end, EINA_INLIST_GET(line));
    }

  // NEED IMPLEMENT HERE!!


}

static void
_item_min_calc(Efl_Ui_Gridview_Data *pd,
               Efl_Ui_Gridview_Item *item,
               Evas_Coord w,
               Evas_Coord h)
{
   Efl_Ui_Gridview_Item *gitem;
   int vw, vh;

   efl_gfx_size_get(pd->pan.obj, &vw, &vh);

   if (_horiz(pd->direct))
     {
        pd->realized.w -= item->minw;
        pd->realized.w += w;
        if (pd->realized.h <= h) pd->realized.h = h;
        else if (pd->realized.h < item->minh)
          {
             pd->realized.h = h;
             EINA_INARRAY_FOREACH(&pd->items.array, gitem)
               {
                  if (pd->realized.h < gitem->minh)
                    pd->realized.h = gitem->minh;

                    if (gitem != item && gitem->minh == item->minh)
                      break;
               }
          }
     }
   else
     {
        pd->realized.h -= item->minh;
        pd->realized.h += h;
        if (pd->realized.w <= w) pd->realized.w = w;
        else if (pd->realized.w == item->minw)
          {
             pd->realized.w = w;
             EINA_INARRAY_FOREACH(&pd->items.array, gitem)
               {
                  if (pd->realized.w < gitem->minw)
                    pd->realized.w = gitem->minw;

                  if (gitem != item && gitem->minw == item->minw)
                    break;
               }
          }
     }

   item->minw = w;
   item->minh = h;
//DEBUG-START
   SHINF("min:["_CR_"%d, %d"_CR_"] realized:["_CR_"%d, %d"_CR_"]",
         CRBLD(CRRED, BGBLK), item->minw, item->minh, CRINF,
         CRBLD(CRGRN, BGBLK), pd->realized.w, pd->realized.h, CRINF);
//DEBUG-END
}

static void
_on_item_size_hint_change(void *data,
                          Evas *e EINA_UNUSED,
                          Evas_Object *obj EINA_UNUSED,
                          void *event_info EINA_UNUSED)
{
   Efl_Ui_Gridview_Item *item = data;
   EINA_SAFETY_ON_NULL_RETURN(item);
   EFL_UI_GRIDVIEW_DATA_GET_OR_RETURN(item->widget, pd);

   SHDBG("SIZE CHANGED ["_CR_"%p:%d"_CR_"]", CRBLD(CRGRN, BGBLK), item, item->index, CRDBG);

   _item_calc(pd, item);
   efl_canvas_group_change(pd->obj);
}

static void
_layout_realize(Efl_Ui_Gridview_Data *pd,
                Efl_Ui_Gridview_Item *item)
{
   Efl_Ui_Gridview_Child evt;

   efl_ui_view_model_set(item->layout, item->model);

   evt.view = item->layout;
   evt.model = item->model;
   evt.index = item->index;
   efl_event_callback_call(item->widget, EFL_UI_GRIDVIEW_EVENT_CHILD_REALIZED, &evt);

   efl_gfx_visible_set(item->layout, EINA_TRUE);
   _item_calc(pd, item);
}

static void
_layout_unrealize(Efl_Ui_Gridview_Data *pd,
                  Efl_Ui_Gridview_Item *item)
{
   Efl_Ui_Gridview_Child evt;
   EINA_SAFETY_ON_NULL_RETURN(item);

   if (item->future)
     {
        efl_future_cancel(item->future);
        item->future = NULL;
     }

   /* TODO:calculate new min */
   _item_min_calc(pd, item, 0, 0);

   evt.view = item->layout;
   evt.model = item->model;
   evt.index = item->index;
   efl_event_callback_call(item->widget, EFL_UI_GRIDVIEW_EVENT_CHILD_UNREALIZED, &evt);
   efl_ui_view_model_set(item->layout, NULL);

   efl_gfx_visible_set(item->layout, EINA_FALSE);
   efl_gfx_position_set(item->layout, -9999, -9999);
}

static void
_child_transient_setup(Efl_Ui_Gridview_Data *pd,
                       Efl_Ui_Gridview_Item *item)
{
   efl_event_callback_add(item->model, EFL_MODEL_EVENT_PROPERTIES_CHANGED, _efl_model_properties_changed_cb, item);

   efl_event_callback_add(item->layout, EFL_UI_FOCUS_MANAGER_EVENT_FOCUSED, _on_item_focused, item);
   //efl_event_callback_add(item->layout, EFL_UI_FOCUS_MANAGER_EVENT_UNFOCUSED, _on_item_unfocused, item);
   evas_object_event_callback_add(item->layout, EVAS_CALLBACK_MOUSE_DOWN, _on_item_mouse_down, item);
   evas_object_event_callback_add(item->layout, EVAS_CALLBACK_MOUSE_UP, _on_item_mouse_up, item);

   evas_object_event_callback_add(item->layout, EVAS_CALLBACK_CHANGED_SIZE_HINTS, _on_item_size_hint_change, item);
   if (item->selected)
     pd->selected_items = eina_list_append(pd->selected_items, item);
}

static void
_child_setup(Efl_Ui_Gridview_Data *pd,
             Efl_Ui_Gridview_Item *item,
             Efl_Model *model,
             Eina_Inarray *recycle_layouts,
             int idx)
{
   EINA_SAFETY_ON_NULL_RETURN(pd);

   assert(model != NULL);
   assert(item->layout == NULL);
   assert(item->widget == NULL);

   item->widget = pd->obj;
   item->model = efl_ref(model);
   if (recycle_layouts && eina_inarray_count(recycle_layouts))
     item->layout = *(void **)eina_inarray_pop(recycle_layouts);
   else
     {
        Eina_Stringshare *selected = eina_stringshare_add("selected");
        item->layout = efl_ui_factory_create(pd->factory, item->model, pd->obj);
        if (pd->select_mode != ELM_OBJECT_SELECT_MODE_NONE && _efl_model_properties_has(item->model, selected))
          efl_ui_model_connect(item->layout, "signal/elm,state,%v", "selected");
        eina_stringshare_del(selected);
     }
   SHINF("using layout (new or not) %p", item->layout);
   item->future = NULL;
   item->index = idx + pd->realized.start;
   item->minw = item->minh = 0;

   elm_widget_sub_object_add(pd->obj, item->layout);
   efl_canvas_group_member_add(pd->pan.obj, item->layout);

//   FIXME: really need get it in model?
   Eina_Stringshare *style_prop = eina_stringshare_add("style");
   if (_efl_model_properties_has(item->model, style_prop))
     {
        item->future = efl_model_property_get(item->model, style_prop);
        efl_future_then(item->future, &_item_style_property_then, &_item_property_error, NULL, item);
     }
   eina_stringshare_del(style_prop);
//
   _layout_realize(pd, item);

   _child_transient_setup(pd, item);
}

static void
_child_transient_release(Efl_Ui_Gridview_Data *pd,
                         Efl_Ui_Gridview_Item *item)
{
   evas_object_event_callback_del_full(item->layout, EVAS_CALLBACK_CHANGED_SIZE_HINTS, _on_item_size_hint_change, item);
   efl_event_callback_del(item->model, EFL_MODEL_EVENT_PROPERTIES_CHANGED, _efl_model_properties_changed_cb, item);
   evas_object_event_callback_del_full(item->layout, EVAS_CALLBACK_MOUSE_DOWN, _on_item_mouse_down, item);
   evas_object_event_callback_del_full(item->layout, EVAS_CALLBACK_MOUSE_UP, _on_item_mouse_up, item);

   efl_event_callback_del(item->layout, EFL_UI_FOCUS_MANAGER_EVENT_FOCUSED, _on_item_focused, item);
   //efl_event_callback_del(item->layout, EFL_UI_FOCUS_MANAGER_EVENT_UNFOCUSED, _on_item_unfocused, item);
   if (item->selected)
     pd->selected_items = eina_list_remove(pd->selected_items, item);
}

static void
_child_release(Efl_Ui_Gridview_Data *pd,
               Efl_Ui_Gridview_Item *item,
               Eina_Inarray *recycle_layouts)
{
   _child_transient_release(pd, item);
   item->widget = NULL;

   SHINF("layout %p", item->layout);
   _layout_unrealize(pd, item);
   if (item->future)
     efl_future_cancel(item->future);
   efl_unref(item->model);
   // discard elm_layout to thrash to be able to reuse it
   assert(item->layout != NULL);
   if (recycle_layouts)
     eina_inarray_push(recycle_layouts, &item->layout);
   else
     efl_ui_factory_release(pd->factory, item->layout);
   item->layout = NULL;
   if (_horiz(pd->direct))
     pd->realized.w -= item->geo.w;
   else
     pd->realized.h -= item->geo.h;
}

static void
_remove_at(Efl_Ui_Gridview_Data *pd,
           int index)
{
   Efl_Ui_Gridview_Item *to_first, *from_first;
   int i, j;

   // fits, just move around
   to_first = pd->items.array.members;
   to_first += index;
   from_first = to_first + 1;

   _child_release(pd, to_first, NULL);

   for (i = index + 1, j = 0; i != (int)pd->items.array.len; ++i, ++j)
     _child_transient_release(pd, &from_first[j]);

   memmove(to_first, from_first, (sizeof(Efl_Ui_Gridview_Item) * (pd->items.array.len - index - 1)));

   --(pd->items.array.len);

   for (i = index, j = 0; i != (int)pd->items.array.len; ++i, ++j)
     {
        to_first[j].index--;
        _child_transient_setup(pd, &to_first[j]);
     }

   memset(&to_first[j], 0, sizeof(Efl_Ui_Gridview_Item));
}

static void
_insert_at(Efl_Ui_Gridview_Data *pd,
           int index,
           Efl_Model *child)
{
   Efl_Ui_Gridview_Item *to_first, *from_first;
   int i;

   if (pd->items.array.len != pd->items.array.max)
     {
        // fits, just move around
        from_first = pd->items.array.members;
        from_first += pd->items.array.len;
        to_first = from_first + 1;

        for (i = index; i != (int)pd->items.array.len; ++i)
          {
             _child_transient_release(pd, from_first);
             memcpy(to_first, from_first, sizeof(Efl_Ui_Gridview_Item));
             to_first->index++;
             _child_transient_setup(pd, to_first);
             --from_first, --to_first;
          }
        pd->items.array.len++;
     }
   else
     {
        // doesn't fit, need to allocate space
        void *members = calloc(1, (pd->items.array.max + pd->items.array.step) * sizeof(Efl_Ui_Gridview_Item));
        // copy before index
        to_first = members;
        from_first = pd->items.array.members;
        memcpy(to_first, from_first, sizeof(Efl_Ui_Gridview_Item) * index);
        for (i = 0; i != index; ++i)
          {
             _child_transient_release(pd, from_first);
             _child_transient_setup(pd, to_first);
             to_first++, from_first++;
          }
        // copy after index
        to_first++;
        memcpy(to_first, from_first, sizeof(Efl_Ui_Gridview_Item) * (pd->items.array.len - index));
        for (i = index; i != (int)pd->items.array.len; ++i)
          {
             _child_transient_release(pd, from_first);
             _child_transient_setup(pd, to_first);
             to_first->index++;
             to_first++, from_first++;
          }
        free(pd->items.array.members);
        pd->items.array.members = members;
        pd->items.array.max += pd->items.array.step;
        pd->items.array.len++;
     }

   // init child
   to_first = pd->items.array.members;
   memset(&to_first[index], 0, sizeof(Efl_Ui_Gridview_Item));
   _child_setup(pd, &to_first[index], child, NULL, index);
   pd->realized.slice++;
}

static void
_resize_children(Efl_Ui_Gridview_Data *pd,
                 int removing_before,
                 int removing_after,
                 Eina_Accessor *acc)
{
   Efl_Ui_Gridview_Item *item;
   Eina_Inarray recycle_layouts;
   unsigned to_begin, from_begin, copy_size;
   unsigned idx;

   eina_inarray_setup(&recycle_layouts, sizeof(Elm_Layout *), 0);

   SHINF("_children_then removing_before %d removing_after %d", removing_before, removing_after);
   EINA_SAFETY_ON_NULL_RETURN(pd);

   pd->future = NULL;
   EINA_SAFETY_ON_NULL_RETURN(acc);

   ELM_WIDGET_DATA_GET_OR_RETURN(pd->obj, wd);

   assert(pd->realized.slice == (int)eina_inarray_count(&pd->items.array));
   // received slice start is after older slice start
   if (removing_before > 0)
     {
        int i = 0;
        while (i != removing_before)
          {
             Efl_Ui_Gridview_Item *members = pd->items.array.members;
             item = &members[i];
             SHINF("uninitializing item (idx %d) %d %p", i, i + pd->realized.start, item);
             _child_release(pd, item, &recycle_layouts);
             SHINF("uninitialized item (idx %d) %d %p", i, i + pd->realized.start, item);
             ++i;
          }
        to_begin = 0;
        from_begin = removing_before;
     }
   else
     {
        to_begin = -removing_before;
        from_begin = 0;
     }

   if (removing_after > 0)
     {
        int i = pd->realized.slice - removing_after;
        while (i != pd->realized.slice)
          {
             Efl_Ui_Gridview_Item *members = pd->items.array.members;
             item = &members[i];
             SHINF("uninitializing item (idx %d) %d %p", i, i + pd->realized.start, item);
             _child_release(pd, item, &recycle_layouts);
             SHINF("uninitialized item (idx %d) %d %p", i, i + pd->realized.start, item);
             ++i;
          }
        copy_size = eina_inarray_count(&pd->items.array)
          - (from_begin + removing_after);
     }
   else
     {
        copy_size = eina_inarray_count(&pd->items.array) - from_begin;
     }

   SHINF("from_begin %d to_begin: %d copy_size %d", from_begin, to_begin, copy_size);

   if (removing_after + removing_before >= 0)
     {
        if (from_begin != to_begin)
          {
             SHINF("moving from: %d to %d with size %d", from_begin, to_begin, copy_size);
             Efl_Ui_Gridview_Item *from_first = pd->items.array.members;
             Efl_Ui_Gridview_Item *to_first = pd->items.array.members;
             from_first += from_begin;
             to_first += to_begin;
             Efl_Ui_Gridview_Item *from_last = from_first + copy_size;
             Efl_Ui_Gridview_Item *to_last = to_first + copy_size;
             Efl_Ui_Gridview_Item *from_first_t = from_first;
             for (; from_first != from_last; ++from_first)
               {
                  _child_transient_release(pd, from_first);
               }
             memmove(to_first, from_first_t, copy_size * sizeof(Efl_Ui_Gridview_Item));
             for (; to_first != to_last; ++to_first)
               {
                  _child_transient_setup(pd, to_first);
               }
          }
        SHINF("old len: %d", pd->items.array.len);
        SHINF("zeoring %p %d size %d",
              &((Efl_Ui_Gridview_Item *)pd->items.array.members)[copy_size + to_begin],
              copy_size + to_begin, eina_inarray_count(&pd->items.array) - (copy_size + to_begin));
        memset(&((Efl_Ui_Gridview_Item *)pd->items.array.members)[copy_size + to_begin], 0, (eina_inarray_count(&pd->items.array) - (copy_size + to_begin)) * sizeof(Efl_Ui_Gridview_Item));
        SHINF("zeoring %p %d size %d",
              &((Efl_Ui_Gridview_Item *)pd->items.array.members)[0],
              0, to_begin);
        memset(&((Efl_Ui_Gridview_Item *)pd->items.array.members)[0], 0, to_begin * sizeof(Efl_Ui_Gridview_Item));
        pd->items.array.len -= removing_before + removing_after;
        SHINF("new len: %d", pd->items.array.len);
     }
   else
     {
        Efl_Ui_Gridview_Item *data = calloc(1, pd->outstanding_slice.slice * sizeof(Efl_Ui_Gridview_Item));
        SHINF("new members allocated: %p", data);
        Efl_Ui_Gridview_Item *from_first = pd->items.array.members;
        from_first += from_begin;
        Efl_Ui_Gridview_Item *to_first = data + to_begin;
        Efl_Ui_Gridview_Item *from_last = from_first + copy_size;
        Efl_Ui_Gridview_Item *to_last = to_first + copy_size;
        Efl_Ui_Gridview_Item *from_first_t = from_first;
        for (; from_first != from_last; ++from_first)
          {
             _child_transient_release(pd, from_first);
          }
        memcpy(to_first, from_first_t, copy_size * sizeof(Efl_Ui_Gridview_Item));
        for (; to_first != to_last; ++to_first)
          {
             _child_transient_setup(pd, to_first);
          }
        free(pd->items.array.members);
        pd->items.array.members = data;
        pd->items.array.len = pd->items.array.max = pd->outstanding_slice.slice;
     }

   pd->realized.start = pd->outstanding_slice.slice_start;
   pd->realized.slice = pd->outstanding_slice.slice;

   idx = 0;
   while (removing_before < 0)
     {
        Efl_Ui_Gridview_Item *members = pd->items.array.members;
        Efl_Ui_Gridview_Item *item = &members[idx];
        SHINF("initializing item [%d %p] layout [%p]", idx + pd->realized.start, item, item->layout);

        // initialize item
        void *model = NULL;
        int r = eina_accessor_data_get(acc, idx + pd->realized.start - pd->outstanding_slice.slice_start, &model);
        assert(r != EINA_FALSE);
        assert(model != NULL);

        _child_setup(pd, item, model, &recycle_layouts, idx);

        SHINF("initializing item [%d %p] layout [%p]", idx + pd->realized.start, item, item->layout);

        idx++;
        removing_before++;
     }

   idx = copy_size + to_begin;
   while (removing_after < 0)
     {
        Efl_Ui_Gridview_Item *members = pd->items.array.members;
        Efl_Ui_Gridview_Item *item = &members[idx];

        SHINF("initializing item (idx %d) [%d %p] layout [%p]", idx, idx + pd->realized.start, item, item->layout);

        // initialize item
        void *model = NULL;
        int r = eina_accessor_data_get(acc, idx + pd->realized.start - pd->outstanding_slice.slice_start, &model);
        assert(r != EINA_FALSE);
        assert(model != NULL);

        _child_setup(pd, item, model, &recycle_layouts, idx);

        SHINF("initializing item (idx %d) [%d %p] layout [%p]", idx, idx + pd->realized.start, item, item->layout);
        idx++;
        removing_after++;
     }

   {
      Elm_Layout **layout;
      EINA_INARRAY_FOREACH(&recycle_layouts, layout)
        {
           efl_ui_factory_release(pd->factory, *layout);
        }
      free(recycle_layouts.members);
   }
}

static void
_children_then(void *data,
               const Efl_Event *event)
{
   Efl_Ui_Gridview_Data *pd = data;
   Eina_Accessor *acc = (Eina_Accessor *)((Efl_Future_Event_Success *)event->info)->value;
   int removing_before = -pd->realized.start + pd->outstanding_slice.slice_start;
   int removing_after = pd->realized.start + pd->realized.slice
     - (pd->outstanding_slice.slice_start + pd->outstanding_slice.slice);

   // If current slice doesn't reach new slice
   if (pd->realized.start + pd->realized.slice < pd->outstanding_slice.slice_start
       || pd->outstanding_slice.slice_start + pd->outstanding_slice.slice < pd->realized.start)
     {
        removing_before = pd->realized.slice;
        removing_after = -pd->outstanding_slice.slice;
     }

   _resize_children(pd, removing_before, removing_after, acc);
   efl_canvas_group_change(pd->obj);
}

static void
_efl_ui_gridview_children_free(Eo *obj EINA_UNUSED,
                               Efl_Ui_Gridview_Data *pd)
{
   Eina_Inarray recycle_layouts;

   Efl_Ui_Gridview_Item *item;
   Elm_Layout **layout;

   EINA_SAFETY_ON_NULL_RETURN(pd);
   eina_inarray_setup(&recycle_layouts, sizeof(Elm_Layout *), 0);

   EINA_INARRAY_FOREACH(&pd->items.array, item)
     {
        _child_release(pd, item, &recycle_layouts);
     }

   eina_inarray_resize(&pd->items.array, 0);
   EINA_INARRAY_FOREACH(&recycle_layouts, layout)
     {
        efl_ui_factory_release(pd->factory, *layout);
     }
   free(recycle_layouts.members);
}

static void
_children_error(void *data EINA_UNUSED,
                const Efl_Event *event EINA_UNUSED)
{
   /*
    * Efl_Ui_Gridview_Slice *sd = data;
    * Efl_Ui_Gridview_Data *pd = sd->pd;
    * pd->future = NULL;
    * free(data);
    */
}

static void
_show_region_hook(void *data EINA_UNUSED,
                  Evas_Object *obj)
{
   Evas_Coord x, y, w, h;
   EFL_UI_GRIDVIEW_DATA_GET_OR_RETURN(obj, pd);
   elm_widget_show_region_get(obj, &x, &y, &w, &h);
   x += pd->pan.x;
   y += pd->pan.y;
   elm_interface_scrollable_content_region_show(obj, x, y, w, h);
}

EOLIAN static void
_efl_ui_gridview_select_mode_set(Eo *obj EINA_UNUSED,
                                 Efl_Ui_Gridview_Data *pd,
                                 Elm_Object_Select_Mode mode)
{
   /*
    * Eina_Array_Iterator iterator;
    * Efl_Ui_Gridview_Item *item;
    * unsigned int i;
    */

   if (pd->select_mode == mode) return;

   /*
    * if (pd->select_mode == ELM_OBJECT_SELECT_MODE_NONE)
    *   {
    *      EINA_ARRAY_ITER_NEXT(pd->items.array, i, item, iterator)
    *        {
    *           if (item->selected)
    *             elm_layout_signal_emit(item->layout, "elm,state,selected", "elm");
    *
    *           efl_ui_model_connect(item->layout, "signal/elm,state,%v", "selected");
    *        }
    *   }
    * else if (mode == ELM_OBJECT_SELECT_MODE_NONE)
    *   {
    *      EINA_ARRAY_ITER_NEXT(pd->items.array, i, item, iterator)
    *        {
    *           if (item->selected)
    *             elm_layout_signal_emit(item->layout, "elm,state,unselected", "elm");
    *
    *           efl_ui_model_connect(item->layout, "signal/elm,state,%v", NULL);
    *        }
    *   }
    */
   pd->select_mode = mode;
}

EOLIAN static Elm_Object_Select_Mode
_efl_ui_gridview_select_mode_get(Eo *obj EINA_UNUSED,
                                 Efl_Ui_Gridview_Data *pd)
{
   return pd->select_mode;
}

EOLIAN static void
_efl_ui_gridview_default_style_set(Eo *obj EINA_UNUSED,
                                   Efl_Ui_Gridview_Data *pd,
                                   Eina_Stringshare *style)
{
   eina_stringshare_replace(&pd->style, style);
}

EOLIAN static Eina_Stringshare *
_efl_ui_gridview_default_style_get(Eo *obj EINA_UNUSED,
                                   Efl_Ui_Gridview_Data *pd)
{
   return pd->style;
}

//FIXME update layout
EOLIAN static void
_efl_ui_gridview_homogeneous_set(Eo *obj EINA_UNUSED,
                                 Efl_Ui_Gridview_Data *pd,
                                 Eina_Bool homogeneous)
{
   pd->on_homogeneous = homogeneous;
}

EOLIAN static Eina_Bool
_efl_ui_gridview_homogeneous_get(Eo *obj EINA_UNUSED,
                                 Efl_Ui_Gridview_Data *pd)
{
   return pd->on_homogeneous;
}

EOLIAN static void
_efl_ui_gridview_layout_factory_set(Eo *obj EINA_UNUSED,
                                    Efl_Ui_Gridview_Data *pd,
                                    Efl_Ui_Factory *factory)
{
   //TODO: clean all current layouts??
   if (pd->factory)
     efl_unref(pd->factory);

   pd->factory = efl_ref(factory);
}

EOLIAN static Eina_Bool
_efl_ui_gridview_elm_widget_on_focus(Eo *obj,
                                     Efl_Ui_Gridview_Data *pd EINA_UNUSED,
                                     Elm_Object_Item *item)
{
   //TODO: with no focused focus first visible elementr

   if (!efl_ui_focus_manager_focused(pd->manager))
     {
        printf(">>>> no item focused");
        efl_ui_focus_manager_focus(pd->manager, obj);
     }

   return elm_obj_widget_on_focus(efl_super(obj, MY_CLASS), item);
}

EOLIAN static Elm_Theme_Apply
_efl_ui_gridview_elm_widget_theme_apply(Eo *obj,
                                        Efl_Ui_Gridview_Data *pd EINA_UNUSED)
{
   return elm_obj_widget_theme_apply(efl_super(obj, MY_CLASS));
}

EOLIAN static void
_efl_ui_gridview_efl_canvas_group_group_calculate(Eo *obj,
                                                  Efl_Ui_Gridview_Data *pd)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(pd->obj, wd);
   if (pd->need_recalc) return;

   //FIMXE: Mostly This Group Calculate not necessary
   efl_ui_gridview_custom_layout(obj, EINA_TRUE);
}

EOLIAN static void
_efl_ui_gridview_efl_gfx_position_set(Eo *obj,
                                      Efl_Ui_Gridview_Data *pd,
                                      Evas_Coord x,
                                      Evas_Coord y)
{
   if (_evas_object_intercept_call(obj, EVAS_OBJECT_INTERCEPT_CB_MOVE, 0, x, y))
     return;

   efl_gfx_position_set(efl_super(obj, MY_CLASS), x, y);

   efl_gfx_position_set(pd->hit_rect, x, y);
   efl_gfx_position_set(pd->pan.obj, x - pd->pan.x, y - pd->pan.y);
   efl_canvas_group_change(pd->obj);
}

EOLIAN static void
_efl_ui_gridview_efl_gfx_size_set(Eo *obj,
                                  Efl_Ui_Gridview_Data *pd,
                                  Evas_Coord w,
                                  Evas_Coord h)
{
   Evas_Coord oldw, oldh;
   Eina_Bool load = EINA_FALSE;
   if (_evas_object_intercept_call(obj, EVAS_OBJECT_INTERCEPT_CB_RESIZE, 0, w, h))
     return;

   efl_gfx_geometry_get(obj, NULL, NULL, &oldw, &oldh);

   efl_gfx_size_set(efl_super(obj, MY_CLASS), w, h);
   efl_gfx_size_set(pd->hit_rect, w, h);

   if (_horiz(pd->direct))
     {
        if (w != oldw) load = EINA_TRUE;
     }
   else
     {
        if (h != oldh) load = EINA_TRUE;
     }

   if (load && _update_items(obj, pd))
     return;

   efl_canvas_group_change(pd->obj);
}

EOLIAN static void
_efl_ui_gridview_efl_canvas_group_group_member_add(Eo *obj,
                                                   Efl_Ui_Gridview_Data *pd,
                                                   Evas_Object *member)
{
   efl_canvas_group_member_add(efl_super(obj, MY_CLASS), member);

   if (pd->hit_rect)
     efl_gfx_stack_raise(pd->hit_rect);
}

EOLIAN static void
_efl_ui_gridview_elm_layout_sizing_eval(Eo *obj,
                                        Efl_Ui_Gridview_Data *pd EINA_UNUSED)
{
   Evas_Coord minw = -1, minh = -1, maxw = -1, maxh = -1;
   Evas_Coord vmw = 0, vmh = 0;

   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   efl_gfx_size_hint_combined_min_get(obj, &minw, &minh);
   efl_gfx_size_hint_max_get(obj, &maxw, &maxh);
   edje_object_size_min_calc(wd->resize_obj, &vmw, &vmh);

   minw = vmw;
   minh = vmh;

   if ((maxw > 0) && (minw > maxw))
     minw = maxw;
   if ((maxh > 0) && (minh > maxh))
     minh = maxh;

   efl_gfx_size_hint_min_set(obj, minw, minh);
   efl_gfx_size_hint_max_set(obj, maxw, maxh);
}

EOLIAN static void
_efl_ui_gridview_efl_canvas_group_group_add(Eo *obj,
                                            Efl_Ui_Gridview_Data *pd EINA_UNUSED)
{
   Efl_Ui_Gridview_Pan_Data *pan_data;
   Evas_Coord minw, minh;

   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   efl_canvas_group_add(efl_super(obj, MY_CLASS));
   elm_widget_sub_object_parent_add(obj);

   elm_widget_can_focus_set(obj, EINA_TRUE);

  pd->hit_rect = efl_add(EFL_CANVAS_RECTANGLE_CLASS, efl_provider_find(obj, EVAS_CANVAS_CLASS),
                         efl_key_data_set(efl_added, "_elm_leaveme", obj),
                         efl_canvas_group_member_add(obj, efl_added),
                         elm_widget_sub_object_add(obj, efl_added),
                         efl_canvas_object_repeat_events_set(efl_added, EINA_TRUE),
                         efl_gfx_color_set(efl_added, 0, 0, 0, 0),
                         efl_gfx_visible_set(efl_added, EINA_TRUE));

   elm_widget_on_show_region_hook_set(obj, _show_region_hook, NULL);

   if (!elm_layout_theme_set(obj, "gridview", "base", elm_widget_style_get(obj)))
     CRI("Failed to set layout!");

   pd->mode = ELM_LIST_COMPRESS;
   //pd->average_item_size = AVERAGE_SIZE_INIT;
   pd->direct = efl_orientation_get(obj);
   SHDBG("ORIENT ["_CR_"%d"_CR_"]", CRBLD(CRRED,BGBLK), pd->direct, CRDBG);

   pd->pan.obj = efl_add(MY_PAN_CLASS, obj);
   pan_data = efl_data_scope_get(pd->pan.obj, MY_PAN_CLASS);
   efl_data_ref(obj, MY_CLASS);
   pan_data->wobj = obj;
   pan_data->wpd = pd;
   pd->pan.x = 0;
   pd->pan.y = 0;
   efl_gfx_visible_set(pd->pan.obj, EINA_TRUE);

   SHDBG("PAN OBJECT ADDED "_CR_"[%p]"_CR_, CRDEF(CRWHT, BGRED), pd->pan.obj, CRINF);

   elm_interface_scrollable_objects_set(obj, wd->resize_obj, pd->hit_rect);
   elm_interface_scrollable_extern_pan_set(obj, pd->pan.obj);
   elm_interface_scrollable_bounce_allow_set(obj, EINA_FALSE,
                                             _elm_config->thumbscroll_bounce_enable);
   elm_interface_atspi_accessible_type_set(obj, ELM_ATSPI_TYPE_DISABLED);

   edje_object_size_min_calc(wd->resize_obj, &minw, &minh);
   efl_gfx_size_hint_min_set(obj, minw,minh);

   elm_layout_sizing_eval(obj);
}

EOLIAN static void
_efl_ui_gridview_efl_canvas_group_group_del(Eo *obj,
                                            Efl_Ui_Gridview_Data *pd)
{
   _efl_ui_gridview_children_free(obj, pd);

   ELM_SAFE_FREE(pd->pan.obj, efl_del);
   efl_canvas_group_del(efl_super(obj, MY_CLASS));
}

EOLIAN static Efl_Ui_Focus_Manager *
_efl_ui_gridview_elm_widget_focus_manager_factory(Eo *obj EINA_UNUSED,
                                                  Efl_Ui_Gridview_Data *pd EINA_UNUSED,
                                                  Efl_Ui_Focus_Object *root)
{
   if (!pd->manager)
     pd->manager = efl_add(EFL_UI_FOCUS_MANAGER_CLASS, obj,
                           efl_ui_focus_manager_root_set(efl_added, root));

   SHINF("new focus manager %p", pd->manager);
   return pd->manager;
}

EOLIAN static Eo *
_efl_ui_gridview_efl_object_constructor(Eo *obj,
                                        Efl_Ui_Gridview_Data *pd)
{
   Efl_Ui_Focus_Manager *manager;

   obj = efl_constructor(efl_super(obj, MY_CLASS));
   pd->obj = obj;
   efl_canvas_object_type_set(obj, MY_CLASS_NAME);
   evas_object_smart_callbacks_descriptions_set(obj, _smart_callbacks);
   elm_interface_atspi_accessible_role_set(obj, ELM_ATSPI_ROLE_LIST);

   eina_inarray_setup(&pd->items.array, sizeof(Efl_Ui_Gridview_Item), 0);

   manager = elm_obj_widget_focus_manager_factory(obj, obj);
   efl_composite_attach(obj, manager);
   _efl_ui_focus_manager_redirect_events_add(manager, obj);

   pd->style = eina_stringshare_add(elm_widget_style_get(obj));

   pd->factory = NULL;
   pd->direct = EFL_ORIENT_DOWN;
   pd->align.h = 0;
   pd->align.v = 0;

   return obj;
}

EOLIAN static void
_efl_ui_gridview_efl_object_destructor(Eo *obj,
                                       Efl_Ui_Gridview_Data *pd)
{
   efl_unref(pd->model);
   eina_stringshare_del(pd->style);

   /* efl_event_callback_del(obj, EFL_UI_FOCUS_MANAGER_EVENT_FOCUSED, _focused_element, pd); */
   efl_destructor(efl_super(obj, MY_CLASS));
}

EOLIAN static void
_efl_ui_gridview_efl_ui_view_model_set(Eo *obj,
                                       Efl_Ui_Gridview_Data *pd,
                                       Efl_Model *model)
{
   if (pd->model == model)
     return;

   if (pd->future) efl_future_cancel(pd->future);

   if (pd->model)
     {
        efl_event_callback_del(pd->model, EFL_MODEL_EVENT_CHILD_ADDED, _child_added_cb, obj);
        efl_event_callback_del(pd->model, EFL_MODEL_EVENT_CHILD_REMOVED, _child_removed_cb, obj);
        efl_unref(pd->model);
        pd->model = NULL;
     }

   _efl_ui_gridview_children_free(obj, pd);

   if (!pd->factory)
     pd->factory = efl_add(EFL_UI_LAYOUT_FACTORY_CLASS, obj);

   if (model)
     {
        pd->model = model;
        efl_ref(pd->model);
        efl_event_callback_add(pd->model, EFL_MODEL_EVENT_CHILD_ADDED, _child_added_cb, obj);
        efl_event_callback_add(pd->model, EFL_MODEL_EVENT_CHILD_REMOVED, _child_removed_cb, obj);
        efl_future_then(efl_model_children_count_get(pd->model), &_count_then, &_count_error, NULL, pd);
     }
}

EOLIAN static Efl_Model *
_efl_ui_gridview_efl_ui_view_model_get(Eo *obj EINA_UNUSED,
                                       Efl_Ui_Gridview_Data *pd)
{
   return pd->model;
}

EOLIAN static const Elm_Atspi_Action *
_efl_ui_gridview_elm_interface_atspi_widget_action_elm_actions_get(Eo *obj EINA_UNUSED,
                                                                   Efl_Ui_Gridview_Data *pd EINA_UNUSED)
{
   static Elm_Atspi_Action atspi_actions[] = {
      { "move,prior", "move", "prior", _key_action_move},
      { "move,next", "move", "next", _key_action_move},
      { "move,up", "move", "up", _key_action_move},
      { "move,up,multi", "move", "up_multi", _key_action_move},
      { "move,down", "move", "down", _key_action_move},
      { "move,down,multi", "move", "down_multi", _key_action_move},
      { "move,first", "move", "first", _key_action_move},
      { "move,last", "move", "last", _key_action_move},
      { "select", "select", NULL, _key_action_select},
      { "select,multi", "select", "multi", _key_action_select},
      { "escape", "escape", NULL, _key_action_escape},
      { NULL, NULL, NULL, NULL }
   };
   return &atspi_actions[0];
}

EOLIAN static Eina_List *
_efl_ui_gridview_elm_interface_atspi_accessible_children_get(Eo *obj,
                                                             Efl_Ui_Gridview_Data *pd)
{
   Efl_Ui_Gridview_Item *gitem;
   Eina_List *ret = NULL, *ret2 = NULL;

   EINA_INARRAY_FOREACH(&pd->items.array, gitem)
     ret = eina_list_append(ret, gitem->layout);

   ret2 = elm_interface_atspi_accessible_children_get(efl_super(obj, MY_CLASS));

   return eina_list_merge(ret, ret2);
}

EOLIAN static int
_efl_ui_gridview_elm_interface_atspi_selection_selected_children_count_get(Eo *obj EINA_UNUSED,
                                                                           Efl_Ui_Gridview_Data *pd)
{
   return eina_list_count(pd->selected_items);
}

EOLIAN static Eo *
_efl_ui_gridview_elm_interface_atspi_selection_selected_child_get(Eo *obj EINA_UNUSED,
                                                                  Efl_Ui_Gridview_Data *pd,
                                                                  int child_index)
{
   if (child_index < (int)eina_list_count(pd->selected_items))
     {
        Efl_Ui_Gridview_Item *items = eina_list_nth(pd->selected_items, child_index);
        return items[child_index].layout;
     }
   else
     return NULL;
}

EOLIAN static Eina_Bool
_efl_ui_gridview_elm_interface_atspi_selection_child_select(Eo *obj EINA_UNUSED,
                                                            Efl_Ui_Gridview_Data *pd EINA_UNUSED,
                                                            int child_index EINA_UNUSED)
{
   /*
   if (pd->select_mode != ELM_OBJECT_SELECT_MODE_NONE)
     {
        if (child_index < eina_inlist_count(&pd->items.array))
          {
             Efl_Ui_Gridview_Item* items = pd->items.array.members;
             _efl_ui_gridview_item_select_set(&items[child_index], EINA_TRUE);
             return EINA_TRUE;
          }
     }
   */
   return EINA_FALSE;
}

EOLIAN static Eina_Bool
_efl_ui_gridview_elm_interface_atspi_selection_selected_child_deselect(Eo *obj EINA_UNUSED,
                                                                       Efl_Ui_Gridview_Data *pd EINA_UNUSED,
                                                                       int child_index EINA_UNUSED)
{
   /*
   Efl_Ui_Gridview_Item *item = eina_list_nth(pd->selected, child_index);
   if (item)
     {
        _efl_ui_gridview_item_select_set(pd, item->layout, EINA_FALSE);
        return EINA_TRUE;
     }
   */
   return EINA_FALSE;
}

EOLIAN static Eina_Bool
_efl_ui_gridview_elm_interface_atspi_selection_is_child_selected(Eo *obj EINA_UNUSED,
                                                                 Efl_Ui_Gridview_Data *pd,
                                                                 int child_index)
{
   Efl_Ui_Gridview_Item *item = eina_inarray_nth(&pd->items.array, child_index);
   EINA_SAFETY_ON_NULL_RETURN_VAL(item, EINA_FALSE);
   return item->selected;
}

EOLIAN static Eina_Bool
_efl_ui_gridview_elm_interface_atspi_selection_all_children_select(Eo *obj EINA_UNUSED,
                                                                   Efl_Ui_Gridview_Data *pd)
{
   Efl_Ui_Gridview_Item *item;

   if (pd->select_mode == ELM_OBJECT_SELECT_MODE_NONE)
     return EINA_FALSE;

   EINA_INARRAY_FOREACH(&pd->items.array, item)
     _efl_ui_gridview_item_select_set(item, EINA_TRUE);

   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_efl_ui_gridview_elm_interface_atspi_selection_clear(Eo *obj EINA_UNUSED,
                                                     Efl_Ui_Gridview_Data *pd)
{
   Efl_Ui_Gridview_Item *item;
   Eina_List *l;

   if (pd->select_mode == ELM_OBJECT_SELECT_MODE_NONE)
     return EINA_FALSE;

   EINA_LIST_FOREACH (pd->selected_items, l, item)
     _efl_ui_gridview_item_select_set(item, EINA_FALSE);

   return EINA_TRUE;
}

EOLIAN static Eina_Bool
_efl_ui_gridview_elm_interface_atspi_selection_child_deselect(Eo *obj EINA_UNUSED,
                                                              Efl_Ui_Gridview_Data *pd EINA_UNUSED,
                                                              int child_index EINA_UNUSED)
{
   /*
   Efl_Ui_Gridview_Item *item = eina_array_data_get(pd->items.array, child_index);
   if (item)
     {
        _efl_ui_gridview_item_select_set(item, EINA_FALSE);
        return EINA_TRUE;
     }
   */

   return EINA_FALSE;
}

static Eina_Bool
_key_action_move(Evas_Object *obj,
                 const char *params)
{
   EFL_UI_GRIDVIEW_DATA_GET_OR_RETURN_VAL(obj, pd, EINA_FALSE);
   const char *dir = params;

   Evas_Coord page_x, page_y;
   Evas_Coord v_w, v_h;
   Evas_Coord x, y;

   elm_interface_scrollable_content_pos_get(obj, &x, &y);
   elm_interface_scrollable_page_size_get(obj, &page_x, &page_y);
   elm_interface_scrollable_content_viewport_geometry_get(obj, NULL, NULL, &v_w, &v_h);

/*
   Efl_Ui_Gridview_Item *item = NULL;
   Elm_Object_Item *oitem = NULL;
   Elm_Layout *eoit = NULL;
   if (!strcmp(dir, "up") || !strcmp(dir, "up_multi"))
     {
        if (!elm_widget_focus_next_get(obj, ELM_FOCUS_UP, &eoit, &oitem))
          return EINA_FALSE;
     }
   else if (!strcmp(dir, "down") || !strcmp(dir, "down_multi"))
     {
        if (!elm_widget_focus_next_get(obj, ELM_FOCUS_DOWN, &eoit, &oitem))
          return EINA_FALSE;
     }
   else if (!strcmp(dir, "first"))
     {
        item = eina_list_data_get(pd->items);
        x = 0;
        y = 0;
        elm_widget_focus_next_object_set(obj, item->layout, ELM_FOCUS_UP);
     }
   else if (!strcmp(dir, "last"))
     {
        item = eina_list_data_get(eina_list_last(pd->items));
        elm_obj_pan_pos_max_get(pd->pan.obj, &x, &y);
     }
   else
*/
   if (!strcmp(dir, "prior"))
     {
        if (_horiz(pd->direct))
          {
             if (page_x < 0)
               x -= -(page_x * v_w) / 100;
             else
               x -= page_x;
          }
        else
          {
             if (page_y < 0)
               y -= -(page_y * v_h) / 100;
             else
               y -= page_y;
          }
     }
   else if (!strcmp(dir, "next"))
     {
        if (_horiz(pd->direct))
          {
             if (page_x < 0)
               x += -(page_x * v_w) / 100;
             else
               x += page_x;
          }
        else
          {
             if (page_y < 0)
               y += -(page_y * v_h) / 100;
             else
               y += page_y;
          }
     }
   else return EINA_FALSE;

   elm_interface_scrollable_content_pos_set(obj, x, y, EINA_TRUE);

   return EINA_TRUE;
}

static Eina_Bool
_key_action_select(Evas_Object *obj,
                   const char *params EINA_UNUSED)
{
   EFL_UI_GRIDVIEW_DATA_GET_OR_RETURN_VAL(obj, pd, EINA_FALSE);

   Eo *focused = efl_ui_focus_manager_focused(pd->manager);

   if (focused)
     {
        //_efl_ui_gridview_item_select_set(focused, EINA_TRUE);
     }

   return EINA_FALSE;
}

static Eina_Bool
_key_action_escape(Evas_Object *obj,
                   const char *params EINA_UNUSED)
{
   return _efl_ui_gridview_item_select_clear(obj);
}

EOLIAN static Eina_Bool
_efl_ui_gridview_elm_widget_widget_event(Eo *obj,
                                         Efl_Ui_Gridview_Data *pd,
                                         Evas_Object *src,
                                         Evas_Callback_Type type,
                                         void *event_info)
{
   (void)src;
   Evas_Event_Key_Down *ev = event_info;

   if (type != EVAS_CALLBACK_KEY_DOWN) return EINA_FALSE;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD) return EINA_FALSE;
   if (!eina_inarray_count(&pd->items.array)) return EINA_FALSE;

   if (!_elm_config_key_binding_call(obj, MY_CLASS_NAME, ev, key_actions))
     return EINA_FALSE;

   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
   return EINA_TRUE;
}

static Eina_Bool
_efl_ui_gridview_item_select_clear(Eo *obj)
{
   Eina_List *li;
   Efl_Ui_Gridview_Item *item;
   EFL_UI_GRIDVIEW_DATA_GET_OR_RETURN_VAL(obj, pd, EINA_FALSE);

   EINA_LIST_FOREACH(pd->selected_items, li, item)
     _efl_ui_gridview_item_select_set(item, EINA_FALSE);

   return EINA_TRUE;
}

static void
_select_cb(void *data,
               const Efl_Event *event)
{
   Efl_Ui_Gridview_Item *item = data;
   Efl_Ui_Gridview_Child evt;

   item->timer = NULL;

   evt.view = item->layout; //can be nullptr?
   evt.model = item->model;
   evt.index = item->index;
   /* //TODO I need call this event or catch only by model connect event? */
   if (item->selected)
     {
        efl_event_callback_legacy_call(item->layout, EFL_UI_EVENT_SELECTED, item);
        efl_event_callback_call(item->widget, EFL_UI_GRIDVIEW_EVENT_CHILD_SELECTED, &evt);
     }
   else
     {
        efl_event_callback_legacy_call(item->layout, EFL_UI_EVENT_UNSELECTED, item);
        efl_event_callback_call(item->widget, EFL_UI_GRIDVIEW_EVENT_CHILD_UNSELECTED, &evt);
     }

   efl_del(event->object);
}

static void
_efl_ui_gridview_item_select_set(Efl_Ui_Gridview_Item *item,
                                 Eina_Bool selected)
{
   Eina_Stringshare *sprop, *svalue;

   SHDBG("item: %p layout: %p model: %p list: %p", item, item->layout, item->model, item->widget);

   EFL_UI_GRIDVIEW_DATA_GET_OR_RETURN(item->widget, pd);

   if ((pd->select_mode == ELM_OBJECT_SELECT_MODE_NONE) ||
       (pd->select_mode == ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY))
     return;

   selected = !!selected;
   /* if (!item->model || item->selected == selected) return; */
   item->selected = selected;

   sprop = eina_stringshare_add(SELECTED_PROP);
   svalue = (selected ? eina_stringshare_add("selected") : eina_stringshare_add("unselected"));

   if (_efl_model_properties_has(item->model, sprop))
     {
        Eina_Value v;
        eina_value_setup(&v, EINA_VALUE_TYPE_STRINGSHARE);
        eina_value_set(&v, svalue);
        efl_model_property_set(item->model, sprop, &v);
        eina_value_flush(&v);
     }
   eina_stringshare_del(sprop);
   eina_stringshare_del(svalue);

   efl_del(item->timer);
   item->timer = efl_add(EFL_LOOP_TIMER_CLASS, efl_loop_main_get(EFL_LOOP_CLASS),
                         efl_event_callback_add(efl_added, EFL_LOOP_TIMER_EVENT_TICK, _select_cb, item),
                         efl_loop_timer_interval_set(efl_added, _elm_config->longpress_timeout));
}

static void
_item_calc(Efl_Ui_Gridview_Data *pd, Efl_Ui_Gridview_Item *item)
{
   Evas_Coord minw, minh;
   int pad[4];

   efl_gfx_size_hint_combined_min_get(item->layout, &minw, &minh);
   efl_gfx_size_hint_margin_get(item->layout, &pad[0], &pad[1], &pad[2], &pad[3]);
   efl_gfx_size_hint_weight_get(item->layout, &item->wx, &item->wy);

   if (item->wx < 0) item->wx = 0;
   if (item->wy < 0) item->wy = 0;

   minw += pad[0] + pad[1];
   minh += pad[2] + pad[3];

   pd->weight.x += item->wx;
   pd->weight.y += item->wy;

   // intitialize average line items
   if (!pd->lines.mean && (minw && minh))
   {
      int vw, vh;
      efl_gfx_size_get(pd->pan.obj, &vw, &vh);
      pd->lines.mean = _horiz(pd->direct) ? (vh / minh) : (vw / minw);
//DEBUG-START
      SHCRI("Average Line Init ["_CR_"%d"_CR_"]",
             CRBLD(CRGRN, BGBLK), pd->lines.mean, CRCRI);
//DEBUG-END
   }
//DEBUG-START
   //fprintf(stderr, "\n"); fflush(stderr);
   SH_IFDBG SHPRT_ENDL
   SHDBG("--- BEFORE MIN["_CR_"%d,%d"_CR_"]"_DEF_"\n",
         CRBLD(CRRED, BGBLK), minw , minh , CRDBG, DEF);
//DEBUG-END
   _item_min_calc(pd, item, minw, minh);
//DEBUG-START
   //fprintf(stderr, "\n"); fflush(stderr);
   SH_IFDBG SHPRT_ENDL
   SHDBG("+++  AFTER MIN["_CR_"%d,%d"_CR_"]"_DEF_"\n",
          CRBLD(CRBLU, BGBLK), minw , minh , CRDBG, DEF);
//DEBUG-END
   efl_gfx_size_hint_min_set(item->layout, minw, minh);
}

static Eina_Bool
_update_items(Eo *obj,
              Efl_Ui_Gridview_Data *pd)
{
   int want_slice, want_slice_start = 0, avg_item_w = 0, avg_item_h = 0;
   unsigned int count;
   Evas_Coord w = 0, h = 0;
   Eina_Bool horz = _horiz(pd->direct);

   /* assert(!pd->future); */
   if (pd->future)
     efl_future_cancel(pd->future);

   /*
   if (pd->items)
     count = eina_array_count(pd->items);
   */

   /*
   if ((!recalc && count == pd->items.count) || pd->average_item_size < 1)
     return EINA_FALSE;
   */
   count = eina_inarray_count(&pd->items.array);
/* Gridview guess every item have homogenous.
   if (count && pd->lines.count && pd->lines.mean)
     {
       avg_item_w = pd->realized.w / (horiz ? pd->lines.count : pd->lines.mean);
       avg_item_h = pd->realized.h / (horiz ? pd->lines.mean : pd->lines.count);
     }
   else
*/
   if (count) // Set first item size
     {
       Efl_Ui_Gridview_Item *first = pd->items.array.members;
       avg_item_w = first->minw;
       avg_item_h = first->minh;
     }

   if (!avg_item_w) avg_item_w = AVERAGE_SIZE_INIT;
   if (!avg_item_h) avg_item_h = AVERAGE_SIZE_INIT;

   efl_gfx_geometry_get(obj, NULL, NULL, &w, &h); //viewport geomentry
   if (horz)
     {
        int lines = h / avg_item_h;
        want_slice = (w / avg_item_w) * lines * 2;
        want_slice_start = ((pd->pan.x / avg_item_w) * lines) - (want_slice / 4);
        if (want_slice_start < 0)
          want_slice_start = 0;
     }
   else
     {
        int lines = w / avg_item_w;
        want_slice = (h / avg_item_h) * lines * 2;
        want_slice_start = ((pd->pan.y / avg_item_h) * lines) - (want_slice / 4);
        if (want_slice_start < 0)
          want_slice_start = 0;
     }

   assert(want_slice_start >= 0);
   if (want_slice < SLICE_MIN)
     want_slice = SLICE_MIN;
   if (want_slice_start + want_slice > pd->items.count)
     {
        if (want_slice > pd->items.count)
          want_slice = pd->items.count;
        want_slice_start = pd->items.count - want_slice;
        assert(want_slice_start >= 0);
     }

   if (want_slice != 0)
     {
        pd->outstanding_slice.slice_start = want_slice_start;
        pd->outstanding_slice.slice = want_slice;
        SHINF("slice_start: %d slice_count: %d", want_slice_start, want_slice);
        pd->future = efl_model_children_slice_get(pd->model, want_slice_start, want_slice);
        efl_future_then(pd->future, &_children_then, &_children_error, NULL, pd);
     }

   return EINA_TRUE;
}

typedef struct _Item_Calc Item_Calc;

struct _Item_Calc {
  Efl_Ui_Gridview_Item *item;
  int w, h;
};

static void
_item_position_set(Efl_Ui_Gridview_Item *item, Efl_Ui_Gridview_Data *pd, int x, int y, int w, int h)
{
   item->geo.x = x;
   item->geo.y = y;
   item->geo.w = w;
   item->geo.h = h;
   efl_gfx_geometry_set(item->layout, x - pd->pan.x, y - pd->pan.y, w, h);
}

static void
_item_position_get(Efl_Ui_Gridview_Item *item, Efl_Ui_Gridview_Data *pd, int *x, int *y, int *w, int *h)
{
   *x = item->geo.x;
   *y = item->geo.y;
   *w = item->geo.w;
   *h = item->geo.h;
}



static Eina_List *
_line_position_update(Eina_List **items, Eina_List **cache, Efl_Ui_Gridview_Data *pd)
{
   Item_Calc *calc = NULL;
   Efl_Ui_Gridview_Item *item = NULL;
   int count = eina_list_count(*items);

   EINA_LIST_FREE(*items, calc)
     {
        if (!calc || !calc->item) continue;
        item = calc->item;

        //DEBUG
        SHDBG("["_CR_"%d"_CR_"]["_CR_"%d,%d"_CR_"] [%d, %d, %d, %d]",
               CRBLD(CRBLK, BGRED), item->index, CRDBG,
               CRBLD(CRBLK, BGGRN), item->matrix[0], item->matrix[1], CRCRI,
               item->geo.x, item->geo.y, calc->w, calc->h);

        efl_gfx_geometry_set(item->layout, (item->geo.x - pd->pan.x), (item->geo.y - pd->pan.y), calc->w, calc->h);
        if (cache)
          {
             calc->item = NULL;
             *cache = eina_list_append(*cache, calc);
          }
        else free(calc);
     }
   return NULL;
}

EOLIAN static void
_efl_ui_gridview_show_child(Eo *obj,
                            Efl_Ui_Gridview_Data *pd,
                            int index,
                            double align,
                            Eina_Bool /*Efl_Ui_Gridview_Child_Show_Animation*/ animation)
{
   int show_x = 0, show_y = 0, aligned_w = 0, aligned_h = 0, ow = 0, oh = 0;
   int count = eina_inarray_count(&pd->items.array);
   int horiz = _horiz(pd->direct);
   if (!count) return;
   Efl_Ui_Gridview_Item *start = pd->items.array.members;
   Efl_Ui_Gridview_Item *last = eina_inarray_nth(&(pd->items.array), (count - 1));
   if (!start || !last) return;
   int start_idx = start->index;
   int last_idx = last->index;

   if ((align < 0.0 || align > 1.0 )&& align != -1.0)
     {
        SHCRI("input value align["_CR_"%.2f"_CR_"] should be around 0.0 to 1.0 or INNER", CRBLD(CRRED, BGBLK), align, CRCRI);
     }
   if (index > pd->items.count || 0 > index) return;

   elm_interface_scrollable_content_viewport_geometry_get(obj, NULL, NULL, &ow, &oh);

   if (index > last_idx || index < start_idx)
    {
       SHDBG("index cannot find in item array [%d~%d]", start_idx, last_idx);
      if (!pd->lines.mean) return;

       int line_index = index / pd->lines.mean;
       line_index = ((index % pd->lines.mean)? line_index + 1 : line_index);

       if (horiz)
       {
           show_x = line_index * start->minw;
           show_y = (index % pd->lines.mean) * start->minh;
       }
       else
       {
           show_x = (index % pd->lines.mean) * start->minw;
           show_y = line_index * start->minh;
       }
       aligned_w = start->minw;
       aligned_h = start->minh;
    }
  else
    {
       SHDBG("index can find in item array [%d~%d]", start_idx, last_idx);
       Efl_Ui_Gridview_Item *item = eina_inarray_nth(&pd->items.array, index);
       if (!item) return;
       show_x = item->geo.x;
       show_y = item->geo.y;
       aligned_w = item->minw;
       aligned_h = item->minh;
    }

   if (0.0 <= align <= 1.0)
     {
        aligned_w = ow;
        aligned_h = oh;
        show_x = horiz ? (show_x - (align * (ow - show_x))): show_x;
        show_y = horiz ? show_y : (show_y - (align * (oh - show_y)));
     }

   SHDBG("Index:[%d] Items:[%d] Slice:[%d, "_CR_"%d:%d"_CR_"] Dest:["_CR_"%d, %d, %d, %d"_CR_"]",
          index, pd->items.count, count,
          CRBLD(CRRED, BGBLK), start_idx, last_idx, CRDBG,
          CRBLD(CRGRN, BGBLK), show_x, show_y, aligned_w, aligned_h, CRDBG);
   if (animation)
     elm_interface_scrollable_region_bring_in(obj, show_x, show_y, aligned_w, aligned_h);
   else
     elm_interface_scrollable_content_region_set(obj, show_x, show_y, aligned_w, aligned_h);
}

EOLIAN static void
_efl_ui_gridview_custom_layout(Eo *obj,
                               Efl_Ui_Gridview_Data *pd,
                               Eina_Bool sync EINA_UNUSED)
{
   //pd->need_update = EINA_TRUE;
   //if (sync)
   _custom_layout_internal(obj);
   efl_canvas_group_change(pd->pan.obj);
}

static void
_custom_layout_internal(Eo *obj)
{
   EFL_UI_GRIDVIEW_DATA_GET_OR_RETURN(obj, pd);
   Efl_Ui_Gridview_Item *gitem = NULL;
   Eina_Bool horiz = _horiz(pd->direct), zeroweight = EINA_FALSE;
   int ow = 0, oh = 0, want;
   int boxx, boxy, boxw, boxh, length, pad, extra = 0, rounding = 0;
   int boxl = 0, boxr = 0, boxt = 0, boxb = 0;
   double cur_pos = 0, scale, box_align[2], weight[2] = { 0, 0 }, lsum = 0;
   Eina_Bool box_fill[2] = { EINA_FALSE, EINA_FALSE };
   int count = 0, row = 0, col = 0;
   int avg_item_w = 0, avg_item_h = 0;
   int oldminw, oldminh, lcount = 0, lmean = 0;
   double maxw = 0, maxh = 0;
   Eina_List *order = NULL;

   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   efl_gfx_geometry_get(obj, &boxx, &boxy, &boxw, &boxh);
   efl_gfx_size_hint_margin_get(obj, &boxl, &boxr, &boxt, &boxb);

   //FIXME result is wrong!!!
   elm_interface_scrollable_content_viewport_geometry_get(obj, NULL, NULL, &ow, &oh);

   oldminw = pd->minw;
   oldminh = pd->minh;

   scale = efl_ui_scale_get(obj);

   SHDBG("[%s] --- geo[%d, %d, %d, %d] view[%d,%d] margin[%d, %d, %d, %d] scale[%.2f]",
         SHNAME(obj), boxx, boxy, boxw, boxh, ow, oh, boxl, boxr, boxt, boxb, scale);

   // Box align: used if "item has max size and fill" or "no item has a weight"
   // Note: cells always expand on the orthogonal direction
   box_align[0] = pd->align.h;
   box_align[1] = pd->align.v;
   if (box_align[0] < 0)
     {
        box_fill[0] = EINA_TRUE;
        box_align[0] = 0.5;
     }
   if (box_align[1] < 0)
     {
        box_fill[1] = EINA_TRUE;
        box_align[1] = 0.5;
     }

   count = eina_inarray_count(&pd->items.array);

   // box outer margin
   boxw -= boxl + boxr;
   boxh -= boxt + boxb;
   boxx += boxl;
   boxy += boxt;

   if (!ow) ow = boxw;
   if (!oh) oh = boxh;

   //average_item_size = count ? (/*horz*/ EINA_FALSE ? pd->realized.w : pd->realized.h) / count : AVERAGE_SIZE_INIT;
/* Gridview guess every item have homogenous.
   if (count && pd->lines.count && pd->lines.mean)
     {
       avg_item_w = pd->realized.w / (horiz ? pd->lines.count : pd->lines.mean);
       avg_item_h = pd->realized.h / (horiz ? pd->lines.mean : pd->lines.count);
     }
   else
*/
   if (count) // Set first item size
     {
       Efl_Ui_Gridview_Item *first = pd->items.array.members;
       avg_item_w = first->minw;
       avg_item_h = first->minh;
     }

   if (!avg_item_w) avg_item_w = AVERAGE_SIZE_INIT;
   if (!avg_item_h) avg_item_h = AVERAGE_SIZE_INIT;

   // total space & available space
   if (horiz)
     {

        length = boxw;
        //FIXME
        want = pd->realized.w / avg_item_w;
        pad = pd->pad.scalable ? (pd->pad.h * scale) : pd->pad.h;

        // padding can not be squeezed (note: could make it an option)
        length -= pad * (count - 1);
        // available space. if <0 we overflow
        extra = length - want;

        pd->minw = want + boxl + boxr + pad * (pd->lines.count - 1);
        pd->minh = pd->realized.h + boxt + boxb;
        if ((pd->items.count > count) && pd->lines.count)
          pd->minw = pd->lines.count * avg_item_w;
     }
   else
     {
        length = boxh;
        //FIXME
        want = pd->realized.h / avg_item_h;
        pad = pd->pad.scalable ? (pd->pad.v * scale) : pd->pad.v;

        // padding can not be squeezed (note: could make it an option)
        length -= pad * (count - 1);
        // available space. if <0 we overflow
        extra = length - want;

        pd->minw = pd->realized.w + boxl + boxr;
        pd->minh = want + pad * (pd->lines.count - 1) + boxt + boxb;
        if ((pd->items.count > count) && pd->lines.count)
          pd->minh = pd->lines.count * avg_item_h;
     }

   //efl_gfx_size_hint_min_set(wd->resize_obj, pd->minw, pd->minh);

   if (extra < 0) extra = 0;

   weight[0] = pd->weight.x;
   weight[1] = pd->weight.y;
   if (EINA_DBL_EQ(weight[!horiz], 0))
     {
        if (box_fill[!horiz])
          {
             // box is filled, set all weights to be equal
             zeroweight = EINA_TRUE;
          }
        else
          {
             // move bounding box according to box align
             cur_pos = extra * box_align[!horiz];
          }
        weight[!horiz] = count;
     }

   if (horiz)
     {
        int lines = boxh / avg_item_h;
        if (lines == 0) lines = 1;
        cur_pos += (avg_item_w * (pd->realized.start / lines));
        cur_pos = (pd->realized.start % lines) ? cur_pos + avg_item_w : cur_pos;
     }
   else
     {
        int lines = boxw / avg_item_w;
        if (lines == 0) lines = 1;
        cur_pos += (avg_item_h * (pd->realized.start / lines));
        cur_pos = (pd->realized.start % lines) ? cur_pos + avg_item_h : cur_pos;
     }

   Eina_List *line_items = NULL;
   Eina_List *line_cache = NULL;

   int start_pos = cur_pos;
   // scan all items, get their properties, calculate total weight & min size
   EINA_INARRAY_FOREACH(&pd->items.array, gitem)
     {
        double cx, cy, cw, ch, x, y, w, h;
        double align[2];
        int item_pad[4], max[2];

        //_item_calc(pd, gitem);
        //SHINF("item %p layout %p", gitem, gitem->layout);
        assert(gitem->layout != NULL);
        efl_gfx_size_hint_align_get(gitem->layout, &align[0], &align[1]);
        efl_gfx_size_hint_max_get(gitem->layout, &max[0], &max[1]);
        efl_gfx_size_hint_margin_get(gitem->layout, &item_pad[0], &item_pad[1], &item_pad[2], &item_pad[3]);

        //SHINF("align[0] %f align[1] %f max[0] %d max[1] %d item_pad[0] %d item_pad[1] %d item_pad[2] %d item_pad[3] %d", (float)align[0], (float)align[1], max[0], max[1], item_pad[0], item_pad[1], item_pad[2], item_pad[3]);

        if (align[0] < 0) align[0] = -1;
        if (align[1] < 0) align[1] = -1;
        if (align[0] > 1) align[0] = 1;
        if (align[1] > 1) align[1] = 1;

        if (max[0] <= 0) max[0] = INT_MAX;
        if (max[1] <= 0) max[1] = INT_MAX;
        if (max[0] < gitem->minw) max[0] = gitem->minw;
        if (max[1] < gitem->minh) max[1] = gitem->minh;

        /* // extra rounding up (compensate cumulative error) */
        /* if ((id == (count - 1)) && (cur_pos - floor(cur_pos) >= 0.5)) */
        /*   rounding = 1; */

        cw = gitem->minw;// + rounding + (zeroweight ? 1.0 : gitem->wx) * extra / weight[0];
        ch = gitem->minh;// + rounding + (zeroweight ? 1.0 : gitem->wy) * extra / weight[1];

        if (horiz)
          {
             if (oh < lsum + ch)
               {
                  line_items = _line_position_update(&line_items, &line_cache, pd);
                  cur_pos += cw + pad;
                  cx = boxx + cur_pos;
                  cy = boxy;
                  maxw = cw;
                  maxh = ch;
                  lsum = ch;
                  row = 0;
                  col++;
               }
             else
               {
                  if (row == 0) lcount = col + 1;
                  cx = boxx + cur_pos;
                  cy = boxy + lsum;
                  maxw = MAX(maxw, cw);
                  maxh = MAX(maxh, ch);
                  lsum += ch;
                  row++;
               }
          }
        else
          {
             if (ow < lsum + cw)
               {
                  cur_pos += ch + pad;
                  cx = boxx;
                  cy = boxy + cur_pos;
                  maxw = cw;
                  maxh = ch;
                  lsum = cw;
                  col = 0;
                  row++;
               }
             else
               {
                  if (col == 0) lcount = row + 1;
                  cx = boxx + lsum;
                  cy = boxy + cur_pos;
                  maxw = MAX(maxw, cw);
                  maxh = MAX(maxh, ch);
                  lsum += cw;
                  col++;
               }
          }

        // horizontally
        if (max[0] < INT_MAX)
          {
             w = MIN(MAX(gitem->minw - item_pad[0] - item_pad[1], max[0]), cw);
             if (align[0] < 0)
               {
                  // bad case: fill+max are not good together
                  x = cx + ((cw - w) * box_align[0]) + item_pad[0];
               }
             else
               x = cx + ((cw - w) * align[0]) + item_pad[0];
          }
        else if (align[0] < 0)
          {
             // fill x
             w = cw - item_pad[0] - item_pad[1];
             x = cx + item_pad[0];
          }
        else
          {
             w = gitem->minw - item_pad[0] - item_pad[1];
             x = cx + ((cw - w) * align[0]) + item_pad[0];
          }

        // vertically
        if (max[1] < INT_MAX)
          {
             h = MIN(MAX(gitem->minh - item_pad[2] - item_pad[3], max[1]), ch);
             if (align[1] < 0)
               {
                  // bad case: fill+max are not good together
                  y = cy + ((ch - h) * box_align[1]) + item_pad[2];
               }
             else
               y = cy + ((ch - h) * align[1]) + item_pad[2];
          }
        else if (align[1] < 0)
          {
             // fill y
             h = ch - item_pad[2] - item_pad[3];
             y = cy + item_pad[2];
          }
        else
          {
             h = gitem->minh - item_pad[2] - item_pad[3];
             y = cy + ((ch - h) * align[1]) + item_pad[2];
          }

        if (horiz)
          {
             if (h < pd->minh) h = pd->minh;
             if (h > oh) h = oh;
          }
        else
          {
             if (w < pd->minw) w = pd->minw;
             if (w > ow) w = ow;
          }


        gitem->geo.x = x;
        gitem->geo.y = y;
        gitem->matrix[0] = row;
        gitem->matrix[1] = col;


        Item_Calc *calc = NULL;
        if (line_cache)
          {
             calc = eina_list_data_get(line_cache);
             line_cache = eina_list_remove(line_cache, calc);
          }
        else  calc = malloc(sizeof(Item_Calc));

        calc->item = gitem;
        calc->w = w; calc->h = h;
        line_items = eina_list_append(line_items, calc);

        //last
        if (gitem->index + 1 == (int)count)
          {
             if (line_items)
               {
                   line_items = _line_position_update(&line_items, &line_cache, pd);
                   eina_list_free(line_cache);
                   line_cache = NULL;
                   lmean = lmean + 1 + (horiz ? row : col);
               }

          }

        SHDBG("["_CR_"%d"_CR_"]["_CR_"%d,%d"_CR_"] CNT[%d] GFX[%.1f,%.1f,%.1f,%.1f : %.1f] line[%.1f], line_list[%p, %p]",
               CRBLD(CRBLK, BGRED), gitem->index, CRINF, CRBLD(CRBLK, BGGRN), row, col, CRINF, count, x, y, w, h, cur_pos, lsum, line_items, line_cache);

//        efl_gfx_geometry_set(gitem->layout, (x - pd->pan.x), (y - pd->pan.y), w, h);

        order = eina_list_append(order, gitem->layout);
     }
    //FIMXME TEMP CODE!!!/
    lmean = lcount ? (count / lcount) : 0;

    if (!pd->lines.mean) pd->lines.mean = lmean;
    else if (lcount)
      {
         int line_count = pd->lines.count ? pd->lines.count : lcount + 1;
         SHCRI("LINECOUNT%d, LCOUNT%d, LMEAN%d, MEAN%d", line_count, lcount, lmean, pd->lines.mean);
         pd->lines.mean =
           (((lmean * lcount) + (pd->lines.mean * (line_count - lcount))) / line_count);
      }

    if (!pd->lines.count) pd->lines.count = lcount;
    else pd->lines.count = pd->lines.mean ? pd->items.count / pd->lines.mean : lcount;
    int avg_size;

    if (horiz)
      {
         avg_size = lcount ? (cur_pos + maxw - start_pos) / (lcount) : cur_pos + maxw - start_pos;
         pd->minw = (int)(cur_pos + maxw);
         if (pd->lines.mean)
         pd->minw = avg_size * (pd->items.count / pd->lines.mean);
         pd->minh = oh;//boxh;
      }
    else
      {
         avg_size = lcount ? (cur_pos + maxh - start_pos) / (lcount) : cur_pos + maxh - start_pos;
         pd->minw = ow;//boxw;
         pd->minh = (int)(cur_pos + maxh);
         if (pd->lines.mean)
         pd->minh = avg_size * (pd->items.count / pd->lines.mean);
      }

   //efl_gfx_size_hint_min_set(wd->resize_obj, pd->minw, pd->minh);

   elm_layout_sizing_eval(pd->obj);
   efl_canvas_group_change(pd->pan.obj);

   SHDBG("MIN["_CR_"%d,%d"_CR_"] LINE["_CR_"%d %d, %d, %d"_CR_"], count %d, realcount %d avgsize %d",
         CRBLD(CRRED, BGBLK), pd->minw, pd->minh, CRDBG,
         CRBLD(CRBLU, BGBLK), lcount, pd->lines.count, lmean, pd->lines.mean, CRDBG, count, pd->items.count, avg_size);

   efl_ui_focus_manager_update_order(pd->manager, pd->obj, order);
   }


EOLIAN static void
_efl_ui_gridview_efl_orientation_orientation_set(Eo *obj EINA_UNUSED,
                                                 Efl_Ui_Gridview_Data *pd,
                                                 Efl_Orient direct)
{
   /*
   if (direct == EFL_ORIENT_LEFT) direct = EFL_OREINT_RIGHT;
   if (direct == EFL_ORIENT_VERTICAL ||
       direct == EFL_ORIENT_UP) direct = EFL_OREINT_DOWN;
   */
   pd->direct = direct;
}

EOLIAN static Efl_Orient
_efl_ui_gridview_efl_orientation_orientation_get(Eo *obj EINA_UNUSED,
                                                 Efl_Ui_Gridview_Data *pd)
{
   return pd->direct;
}

/*
EOLIAN static Efl_Ui_View_Child_Data *
_efl_ui_gridview_child_new(Eo *obj EINA_UNUSED,
                           Efl_Ui_Gridview_Data *pd EINA_UNUSED,
                           Efl_Model *model EINA_UNUSED)
{
   return NULL;
}

EOLIAN static void
_efl_ui_gridview_child_remove(Eo *obj EINA_UNUSED,
                              Efl_Ui_Gridview_Data *pd EINA_UNUSED,
                              Efl_Ui_View_Child_Data *item EINA_UNUSED)
{
}

EOLIAN static Elm_Layout *
_efl_ui_gridview_child_realize(Eo *obj EINA_UNUSED,
                               Efl_Ui_Gridview_Data *pd EINA_UNUSED,
                               Efl_Ui_View_Child_Data *item EINA_UNUSED)
{
   return NULL;
}

EOLIAN static Elm_Layout *
_efl_ui_gridview_child_unrealize(Eo *obj EINA_UNUSED,
                                 Efl_Ui_Gridview_Data *pd EINA_UNUSED,
                                 Efl_Ui_View_Child_Data *item EINA_UNUSED)
{
   return NULL;
}

EOLIAN static void
_efl_ui_gridview_child_calculate(Eo *obj EINA_UNUSED,
                                 Efl_Ui_Gridview_Data *pd EINA_UNUSED,
                                 Efl_Ui_View_Child_Data *item EINA_UNUSED)
{
}
*/


/* Internal EO APIs and hidden overrides */
#define EFL_UI_GRIDVIEW_EXTRA_OPS \
  EFL_CANVAS_GROUP_ADD_DEL_OPS(efl_ui_gridview)

#include "efl_ui_gridview.eo.c"