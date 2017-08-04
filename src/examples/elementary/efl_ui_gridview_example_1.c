// gcc -o efl_ui_gridview_example_1 efl_ui_gridview_example_1.c `pkg-config --cflags --libs elementary`

#ifdef HAVE_CONFIG_H
# include "config.h"
#else
# define EFL_BETA_API_SUPPORT 1
# define EFL_EO_API_SUPPORT 1
#endif

#include <Elementary.h>
#include <Efl.h>
#include <Eio.h>
#include <stdio.h>

#define NUM_ITEMS 100
#define EFL_MODEL_TEST_FILENAME_PATH "/home/sh10233lee/Pictures/"

const char *styles[] = {
        "odd",
        "default"
   };

char edj_path[PATH_MAX];

struct _Private_Data
{
   Eo *model;
   Evas_Object *gview;
   Eo *imf;
};
typedef struct _Private_Data Private_Data;

static void
_cleanup_cb(void *data, Evas *e EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Private_Data *pd = data;
   efl_unref(pd->model);
   efl_unref(pd->gview);
}

static void
_realized_cb(void *data, const Efl_Event *event)
{
   Efl_Ui_List_Item_Event *ie = event->info;
   Private_Data *pd = data;

   //printf("[%d: %p: %p] Realize\n", ie->index, ie->layout, ie->child);

   //efl_ui_view_model_set(ie->layout, ie->child);
   //FIXME: Temporary using list theme.
   //elm_layout_theme_set(ie->layout, "genlist", "item", "default/default");
   elm_layout_theme_set(ie->layout, "gridview", "child", "default/default");

   efl_gfx_size_hint_weight_set(ie->layout, EFL_GFX_SIZE_HINT_EXPAND, EFL_GFX_SIZE_HINT_EXPAND);
   efl_gfx_size_hint_align_set(ie->layout, EFL_GFX_SIZE_HINT_FILL, EFL_GFX_SIZE_HINT_FILL);
   switch (ie->index % 4)
     {
      case 0:
         efl_gfx_size_hint_min_set(ie->layout, 50, 50);
         break;
      case 1:
         efl_gfx_size_hint_min_set(ie->layout, 70, 50);
         break;
      case 2:
         efl_gfx_size_hint_min_set(ie->layout, 30, 50);
         break;
      case 3: default :
         efl_gfx_size_hint_min_set(ie->layout, 60, 50);
         break;
     }

   //efl_gfx_size_set(ie->layout, 50, 50);
   elm_object_focus_allow_set(ie->layout, EINA_TRUE);

   //efl_ui_model_connect(ie->layout, "elm.text", "name");
   //efl_ui_model_connect(ie->layout, "signal/elm,state,%v", "odd_style");
   efl_ui_model_connect(ie->layout, "elm.text", "filename");
   efl_ui_model_factory_connect(ie->layout, "elm.swallow.icon", pd->imf);
}

static void
_unrealized_cb(void *data EINA_UNUSED, const Efl_Event *event)
{
   Efl_Ui_List_Item_Event *ie = event->info;

   //printf("[%d: %p: %p] Un-Realize\n", ie->index, ie->layout, ie->child);
   //efl_ui_view_model_set(ie->layout, NULL);
   //efl_del(ie->layout);
}

static Efl_Model*
_make_model()
{
   Eina_Value vtext, vstyle;
   Efl_Model_Item *model, *child;
   unsigned int i, s;
   char buf[256];
   strdup(buf);

   model = efl_add(EFL_MODEL_ITEM_CLASS, NULL);
   eina_value_setup(&vtext, EINA_VALUE_TYPE_STRING);
   eina_value_setup(&vstyle, EINA_VALUE_TYPE_STRING);

   for (i = 0; i < (NUM_ITEMS); i++)
     {
        s = i%2;
        child = efl_model_child_add(model);
        eina_value_set(&vstyle, styles[s]);
        efl_model_property_set(child, "odd_style", &vstyle);

        snprintf(buf, sizeof(buf), "Item # %i", i);
        eina_value_set(&vtext, buf);
        efl_model_property_set(child, "name", &vtext);
     }

   return model;
}

EAPI_MAIN int
elm_main(int argc, char **argv)
{
   Private_Data *priv;
   Evas_Object *win;
   char *dirname;

   priv = alloca(sizeof(Private_Data));
   memset(priv, 0, sizeof(Private_Data));

   if (argv[1] != NULL) dirname = argv[1];
   else dirname = EFL_MODEL_TEST_FILENAME_PATH;

   win = efl_add(EFL_UI_WIN_CLASS, NULL,
                 efl_ui_win_type_set(efl_added, EFL_UI_WIN_BASIC),
                 efl_ui_win_name_set(efl_added, "gridview"),
                 efl_text_set(efl_added, "GridView"),
                 efl_ui_win_autodel_set(efl_added, EINA_TRUE));
   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

//   priv->model = _make_model();
   priv->model = efl_add(EIO_MODEL_CLASS, NULL, eio_model_path_set(efl_added, dirname));
   priv->gview = efl_add(EFL_UI_GRIDVIEW_CLASS, win,
                         efl_ui_view_model_set(efl_added, priv->model),
                         efl_content_set(win, efl_added),
                         efl_gfx_size_hint_weight_set(efl_added,
                                                      EFL_GFX_SIZE_HINT_EXPAND,
                                                      EFL_GFX_SIZE_HINT_EXPAND),
                         efl_event_callback_add(efl_added,
                                                EFL_UI_GRIDVIEW_EVENT_ITEM_REALIZED,
                                                _realized_cb, priv),
                         efl_event_callback_add(efl_added,
                                                EFL_UI_GRIDVIEW_EVENT_ITEM_UNREALIZED,
                                                _unrealized_cb, priv),
                         efl_gfx_visible_set(efl_added, EINA_TRUE));

   priv->imf = efl_add(EFL_UI_IMAGE_FACTORY_CLASS, win);
   efl_ui_model_connect(priv->imf, "", "path"); //connect to "path" property

   evas_object_event_callback_add(win, EVAS_CALLBACK_DEL, _cleanup_cb, priv);
   //showall
   efl_gfx_size_set(win, 320, 320);
   efl_gfx_visible_set(win, EINA_TRUE);

   elm_run();
   elm_shutdown();
   ecore_shutdown();

   return 0;
}
ELM_MAIN()
