#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif
#include <Elementary.h>

typedef struct _App_Data
{
   Efl_Animation        *sequential_show_anim;
   Efl_Animation        *sequential_hide_anim;
   Efl_Animation_Object *anim_obj;

   Eina_Bool             is_btn_visible;
} App_Data;

static void
_anim_started_cb(void *data EINA_UNUSED, const Efl_Event *event EINA_UNUSED)
{
   printf("Animation has been started!\n");
}

static void
_anim_ended_cb(void *data, const Efl_Event *event EINA_UNUSED)
{
   App_Data *ad = data;

   printf("Animation has been ended!\n");

   ad->anim_obj = NULL;
}

static void
_anim_running_cb(void *data EINA_UNUSED, const Efl_Event *event)
{
   Efl_Animation_Object_Running_Event_Info *event_info = event->info;
   double progress = event_info->progress;
   printf("Animation is running! Current progress(%lf)\n", progress);
}

static void
_btn_clicked_cb(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   App_Data *ad = data;

   if (ad->anim_obj)
     efl_animation_object_cancel(ad->anim_obj);

   ad->is_btn_visible = !(ad->is_btn_visible);

   if (ad->is_btn_visible)
     {
        //Create Animation Object from Animation
        ad->anim_obj = efl_animation_object_create(ad->sequential_show_anim);
        elm_object_text_set(obj, "Start Sequential Group Animation to hide button");
     }
   else
     {
        //Create Animation Object from Animation
        ad->anim_obj = efl_animation_object_create(ad->sequential_hide_anim);
        elm_object_text_set(obj, "Start Sequential Group Animation to show button");
     }

   //Register callback called when animation starts
   efl_event_callback_add(ad->anim_obj, EFL_ANIMATION_OBJECT_EVENT_STARTED, _anim_started_cb, NULL);

   //Register callback called when animation ends
   efl_event_callback_add(ad->anim_obj, EFL_ANIMATION_OBJECT_EVENT_ENDED, _anim_ended_cb, ad);

   //Register callback called while animation is executed
   efl_event_callback_add(ad->anim_obj, EFL_ANIMATION_OBJECT_EVENT_RUNNING, _anim_running_cb, NULL);

   //Let Animation Object start animation
   efl_animation_object_start(ad->anim_obj);
}

static void
_win_del_cb(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   App_Data *ad = data;
   free(ad);
}

void
test_efl_anim_group_sequential(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   App_Data *ad = calloc(1, sizeof(App_Data));
   if (!ad) return;

   Evas_Object *win = elm_win_add(NULL, "Efl Animation Group Sequential", ELM_WIN_BASIC);
   elm_win_title_set(win, "Efl Animation Group Sequential");
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_smart_callback_add(win, "delete,request", _win_del_cb, ad);

   //Button to be animated
   Evas_Object *btn = elm_button_add(win);
   elm_object_text_set(btn, "Target");
   evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_resize(btn, 150, 150);
   evas_object_move(btn, 125, 100);
   evas_object_show(btn);


   /* Animations to hide button */
   //Rotate from 0 to 45 degrees Animation
   Efl_Animation *cw_45_degrees_anim = efl_add(EFL_ANIMATION_ROTATE_CLASS, NULL);
   efl_animation_rotate_set(cw_45_degrees_anim, 0.0, 45.0, NULL, 0.5, 0.5);

   //Scale Animation to zoom in
   Efl_Animation *scale_double_anim = efl_add(EFL_ANIMATION_SCALE_CLASS, NULL);
   efl_animation_scale_set(scale_double_anim, 1.0, 1.0, 2.0, 2.0, NULL, 0.5, 0.5);

   //Hide Animation
   Efl_Animation *hide_anim = efl_add(EFL_ANIMATION_ALPHA_CLASS, NULL);
   efl_animation_alpha_set(hide_anim, 1.0, 0.0);

   //Hide Sequential Group Animation
   Efl_Animation *sequential_hide_anim = efl_add(EFL_ANIMATION_GROUP_SEQUENTIAL_CLASS, NULL);
   efl_animation_duration_set(sequential_hide_anim, 1.0);
   efl_animation_target_set(sequential_hide_anim, btn);
   efl_animation_final_state_keep_set(sequential_hide_anim, EINA_TRUE);

   //Add animations to group animation
   efl_animation_group_animation_add(sequential_hide_anim, cw_45_degrees_anim);
   efl_animation_group_animation_add(sequential_hide_anim, scale_double_anim);
   efl_animation_group_animation_add(sequential_hide_anim, hide_anim);


   /* Animations of initial state to show button */
   //Rotate from 0 to 45 degrees Animation
   Efl_Animation *cw_45_degrees_anim2 = efl_add(EFL_ANIMATION_ROTATE_CLASS, NULL);
   efl_animation_rotate_set(cw_45_degrees_anim2, 0.0, 45.0, NULL, 0.5, 0.5);

   //Scale Animation to zoom in
   Efl_Animation *scale_double_anim2 = efl_add(EFL_ANIMATION_SCALE_CLASS, NULL);
   efl_animation_scale_set(scale_double_anim2, 1.0, 1.0, 2.0, 2.0, NULL, 0.5, 0.5);

   //Hide Parallel Group Animation
   Efl_Animation *parallel_hide_anim = efl_add(EFL_ANIMATION_GROUP_PARALLEL_CLASS, NULL);
   efl_animation_duration_set(parallel_hide_anim, 0.0);
   efl_animation_target_set(parallel_hide_anim, btn);
   efl_animation_final_state_keep_set(parallel_hide_anim, EINA_TRUE);

   //Add animations to group animation
   efl_animation_group_animation_add(parallel_hide_anim, cw_45_degrees_anim2);
   efl_animation_group_animation_add(parallel_hide_anim, scale_double_anim2);


   /* Animations to show button */
   //Show Animation
   Efl_Animation *show_anim = efl_add(EFL_ANIMATION_ALPHA_CLASS, NULL);
   efl_animation_alpha_set(show_anim, 0.0, 1.0);
   efl_animation_duration_set(show_anim, 1.0);

   //Scale Animation to zoom out
   Efl_Animation *scale_half_anim = efl_add(EFL_ANIMATION_SCALE_CLASS, NULL);
   efl_animation_scale_set(scale_half_anim, 1.0, 1.0, 0.5, 0.5, NULL, 0.5, 0.5);
   efl_animation_duration_set(scale_half_anim, 1.0);

   //Rotate from 45 to 0 degrees Animation
   Efl_Animation *ccw_45_degrees_anim = efl_add(EFL_ANIMATION_ROTATE_CLASS, NULL);
   efl_animation_rotate_set(ccw_45_degrees_anim, 0.0, -45.0, NULL, 0.5, 0.5);
   efl_animation_duration_set(ccw_45_degrees_anim, 1.0);

   //Show Sequential Group Animation
   Efl_Animation *sequential_show_anim = efl_add(EFL_ANIMATION_GROUP_SEQUENTIAL_CLASS, NULL);
   efl_animation_target_set(sequential_show_anim, btn);
   efl_animation_final_state_keep_set(sequential_show_anim, EINA_TRUE);
   //efl_animation_duration_set() is called for each animation not to set the same duration

   //Add animations to group animation
   //First, parallel_hide_anim is added with duration 0 to set the initial state
   efl_animation_group_animation_add(sequential_show_anim, parallel_hide_anim);
   efl_animation_group_animation_add(sequential_show_anim, show_anim);
   efl_animation_group_animation_add(sequential_show_anim, scale_half_anim);
   efl_animation_group_animation_add(sequential_show_anim, ccw_45_degrees_anim);


   //Initialize App Data
   ad->sequential_show_anim = sequential_show_anim;
   ad->sequential_hide_anim = sequential_hide_anim;
   ad->anim_obj = NULL;
   ad->is_btn_visible = EINA_TRUE;

   //Button to start animation
   Evas_Object *btn2 = elm_button_add(win);
   elm_object_text_set(btn2, "Start Sequential Group Animation to hide button");
   evas_object_smart_callback_add(btn2, "clicked", _btn_clicked_cb, ad);
   evas_object_size_hint_weight_set(btn2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_resize(btn2, 300, 50);
   evas_object_move(btn2, 50, 300);
   evas_object_show(btn2);

   evas_object_resize(win, 400, 400);
   evas_object_show(win);
}