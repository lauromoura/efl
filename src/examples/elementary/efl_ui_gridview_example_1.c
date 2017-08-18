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

#define NUM_ITEMS 1000
//#define EFL_MODEL_TEST_FILENAME_PATH "/home/sh10233lee/Pictures/"
#define IMAGE_FILE_PATH "/usr/local/share/elementary/images/"

static void
_realized_cb(void *data, const Efl_Event *event)
{
   Efl_Ui_Gridview_Child *child = event->info;
   Efl_Ui_Image_Factory *imgfac = data;

   //printf("[%d: %p: %p] Realize\n", child->index, child->view, child->model);

   //efl_ui_view_model_set(child->view, child->model);
   //FIXME: Temporary using list theme.
   elm_layout_theme_set(child->view, "gridview", "child", "default/default");

   efl_gfx_size_hint_weight_set(child->view, EFL_GFX_SIZE_HINT_EXPAND, EFL_GFX_SIZE_HINT_EXPAND);
   efl_gfx_size_hint_align_set(child->view, EFL_GFX_SIZE_HINT_FILL, EFL_GFX_SIZE_HINT_FILL);
   efl_gfx_size_hint_min_set(child->view, 55, 75);

/* Case of heterogeneous size
   switch (child->index % 4)
     {
      case 0:
         efl_gfx_size_hint_min_set(child->view, 50, 50);
         break;
      case 1:
         efl_gfx_size_hint_min_set(child->view, 70, 50);
         break;
      case 2:
         efl_gfx_size_hint_min_set(child->view, 30, 50);
         break;
      case 3: default :
         efl_gfx_size_hint_min_set(child->view, 60, 50);
         break;
     }
*/
   elm_object_focus_allow_set(child->view, EINA_TRUE);

  //shouldn't it be connect before layout craeted?
   efl_ui_model_factory_connect(child->view, "elm.swallow.icon", imgfac);  //efl_connect_factory

}

static void
_unrealized_cb(void *data EINA_UNUSED, const Efl_Event *event)
{
   //Efl_Ui_Gridview_Child *child = event->info;
}

static Efl_Model*
_make_model(char *dirname)
{
   Eina_Value vtext, vpath;
   Efl_Model_Item *model, *child;
   unsigned int i, s;
   char buf[256];
   strdup(buf);

   model = efl_add(EFL_MODEL_ITEM_CLASS, NULL);
   eina_value_setup(&vtext, EINA_VALUE_TYPE_STRING);
   eina_value_setup(&vpath, EINA_VALUE_TYPE_STRING);

   for (i = 0; i < (NUM_ITEMS); i++)
     {
        int n = i % 24;
        child = efl_model_child_add(model);

        snprintf(buf, sizeof(buf), "[%d]", i);
        eina_value_set(&vtext, buf);
        efl_model_property_set(child, "name", &vtext);

        if (n < 10)
          snprintf(buf, sizeof(buf), "%sicon_0%d.png", dirname, n);
        else
          snprintf(buf, sizeof(buf), "%sicon_%d.png", dirname, n);
        //printf("image path: %s\n", buf);
        eina_value_set(&vpath, buf);
        efl_model_property_set(child, "path", &vpath);
     }

   return model;
}

EAPI_MAIN int
elm_main(int argc, char **argv)
{
   Efl_Ui_Layout_Factory *lyfac;
   Efl_Ui_Image_Factory *imgfac;
   Eo *win, *model, *gview;
   char *dirname;

   if (argv[1] != NULL) dirname = argv[1];
   else dirname = IMAGE_FILE_PATH;

   win = efl_add(EFL_UI_WIN_CLASS, NULL,
                 efl_ui_win_type_set(efl_added, EFL_UI_WIN_BASIC),
                 efl_ui_win_name_set(efl_added, "gridview"),
                 efl_text_set(efl_added, "GridView"),
                 efl_ui_win_autodel_set(efl_added, EINA_TRUE));
   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

   model = _make_model(dirname);
   //model = efl_add(EIO_MODEL_CLASS, NULL, eio_model_path_set(efl_added, dirname));

   lyfac = efl_add(EFL_UI_LAYOUT_FACTORY_CLASS, win);
   efl_ui_model_connect(lyfac, "elm.text", "name");
   efl_ui_layout_factory_theme_config(lyfac, "gridview", "item", "default");

   imgfac = efl_add(EFL_UI_IMAGE_FACTORY_CLASS, win);
   efl_ui_model_connect(imgfac, "", "path"); // looks better to be efl_connect_model_property

   gview = efl_add(EFL_UI_GRIDVIEW_CLASS, win,
                   efl_ui_view_model_set(efl_added, model),
                   efl_ui_gridview_layout_factory_set(efl_added, lyfac),
                   efl_gfx_size_hint_weight_set(efl_added,
                                                EFL_GFX_SIZE_HINT_EXPAND,
                                                EFL_GFX_SIZE_HINT_EXPAND),
                   efl_gfx_visible_set(efl_added, EINA_TRUE));

   efl_content_set(win, gview);

   efl_event_callback_add(gview, EFL_UI_GRIDVIEW_EVENT_CHILD_REALIZED, _realized_cb, imgfac);
   efl_event_callback_add(gview, EFL_UI_GRIDVIEW_EVENT_CHILD_UNREALIZED, _unrealized_cb, imgfac);

   //showall
   efl_gfx_size_set(win, 320, 320);
   efl_gfx_visible_set(win, EINA_TRUE);

   elm_run();
   elm_shutdown();
   ecore_shutdown();

   return 0;
}
ELM_MAIN()
