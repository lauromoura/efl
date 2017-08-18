/* This header is not to be installed and should only be used by EFL internals */
#ifndef _EVAS_INTERNAL_H
#define _EVAS_INTERNAL_H

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_EVAS_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_EVAS_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif /* ! _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Efl_Canvas_Output Efl_Canvas_Output;

EAPI Efl_Canvas_Output *efl_canvas_output_add(Evas *canvas);
EAPI void efl_canvas_output_del(Efl_Canvas_Output *output);
EAPI void efl_canvas_output_view_set(Efl_Canvas_Output *output,
                                     Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h);
EAPI void efl_canvas_output_view_get(Efl_Canvas_Output *output,
                                     Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h);
EAPI Eina_Bool efl_canvas_output_engine_info_set(Efl_Canvas_Output *output,
                                                 Evas_Engine_Info *info);
EAPI Evas_Engine_Info *efl_canvas_output_engine_info_get(Efl_Canvas_Output *output);
EAPI Eina_Bool efl_canvas_output_lock(Efl_Canvas_Output *output);
EAPI Eina_Bool efl_canvas_output_unlock(Efl_Canvas_Output *output);

EAPI void efl_input_pointer_finalize(Efl_Input_Pointer *obj);

/* Internal EO APIs */
EOAPI void efl_canvas_object_legacy_ctor(Eo *obj);
EOAPI void efl_canvas_object_type_set(Eo *obj, const char *type);
EOAPI void efl_canvas_group_add(Eo *obj);
EOAPI void efl_canvas_group_del(Eo *obj);

EOAPI void *efl_input_legacy_info_get(const Eo *obj);
EOAPI Eo *efl_input_instance_get(const Eo *obj, Efl_Object *owner, void **priv);

EWAPI extern const Efl_Event_Description _EVAS_CANVAS_EVENT_RENDER_FLUSH_PRE;
#define EVAS_CANVAS_EVENT_RENDER_FLUSH_PRE (&(_EVAS_CANVAS_EVENT_RENDER_FLUSH_PRE))

EWAPI extern const Efl_Event_Description _EVAS_CANVAS_EVENT_RENDER_FLUSH_POST;
#define EVAS_CANVAS_EVENT_RENDER_FLUSH_POST (&(_EVAS_CANVAS_EVENT_RENDER_FLUSH_POST))

EWAPI extern const Efl_Event_Description _EVAS_CANVAS_EVENT_AXIS_UPDATE;
#define EVAS_CANVAS_EVENT_AXIS_UPDATE (&(_EVAS_CANVAS_EVENT_AXIS_UPDATE))

EWAPI extern const Efl_Event_Description _EVAS_CANVAS_EVENT_VIEWPORT_RESIZE;
#define EVAS_CANVAS_EVENT_VIEWPORT_RESIZE (&(_EVAS_CANVAS_EVENT_VIEWPORT_RESIZE))

#define EFL_CANVAS_GROUP_DEL_OPS(kls) EFL_OBJECT_OP_FUNC(efl_canvas_group_del, _##kls##_efl_canvas_group_group_del)
#define EFL_CANVAS_GROUP_ADD_OPS(kls) EFL_OBJECT_OP_FUNC(efl_canvas_group_add, _##kls##_efl_canvas_group_group_add)
#define EFL_CANVAS_GROUP_ADD_DEL_OPS(kls) EFL_CANVAS_GROUP_ADD_OPS(kls), EFL_CANVAS_GROUP_DEL_OPS(kls)

EOAPI Eina_Bool efl_animation_is_deleted(Eo *obj);

EWAPI extern const Efl_Event_Description _EFL_ANIMATION_INSTANCE_EVENT_PRE_START;
#define EFL_ANIMATION_INSTANCE_EVENT_PRE_START (&(_EFL_ANIMATION_INSTANCE_EVENT_PRE_START))

EWAPI extern const Efl_Event_Description _EFL_ANIMATION_INSTANCE_EVENT_PRE_ANIMATE;
#define EFL_ANIMATION_INSTANCE_EVENT_PRE_ANIMATE (&(_EFL_ANIMATION_INSTANCE_EVENT_PRE_ANIMATE))

EWAPI extern const Efl_Event_Description _EFL_ANIMATION_INSTANCE_EVENT_POST_END;
#define EFL_ANIMATION_INSTANCE_EVENT_POST_END (&(_EFL_ANIMATION_INSTANCE_EVENT_POST_END))

EOAPI void efl_animation_instance_target_set(Eo *obj, Efl_Canvas_Object *target);
EOAPI Efl_Canvas_Object *efl_animation_instance_target_get(const Eo *obj);

EOAPI void efl_animation_instance_final_state_keep_set(Eo *obj, Eina_Bool state_keep);
EOAPI Eina_Bool efl_animation_instance_final_state_keep_get(const Eo *obj);

EOAPI void efl_animation_instance_duration_set(Eo *obj, double duration);
EOAPI double efl_animation_instance_duration_get(const Eo *obj);

EOAPI void efl_animation_instance_start_delay_set(Eo *obj, double delay_time);
EOAPI double efl_animation_instance_start_delay_get(const Eo *obj);

typedef enum
{
  EFL_ANIMATION_INSTANCE_REPEAT_MODE_RESTART = 0,
  EFL_ANIMATION_INSTANCE_REPEAT_MODE_REVERSE
} Efl_Animation_Instance_Repeat_Mode;

EOAPI void efl_animation_instance_repeat_mode_set(Eo *obj, Efl_Animation_Instance_Repeat_Mode mode);
EOAPI Efl_Animation_Instance_Repeat_Mode efl_animation_instance_repeat_mode_get(const Eo *obj);

EOAPI void efl_animation_instance_repeat_count_set(Eo *obj, int count);
EOAPI int efl_animation_instance_repeat_count_get(const Eo *obj);

EOAPI Eina_Bool efl_animation_instance_is_deleted(Eo *obj);

EOAPI void efl_animation_instance_member_start(Eo *obj);

EOAPI void efl_animation_instance_reverse_member_start(Eo *obj);
EOAPI void efl_animation_instance_reverse_initial_state_set(Eo *obj);

EOAPI void efl_animation_instance_map_reset(Eo *obj);

EOAPI void efl_animation_instance_final_state_show(Eo *obj);

EOAPI void efl_animation_instance_alpha_set(Eo *obj, double from_alpha, double to_alpha);

EOAPI void efl_animation_instance_rotate_set(Eo *obj, double from_degree, double to_degree, Efl_Canvas_Object *pivot, double cx, double cy);
EOAPI void efl_animation_instance_rotate_get(const Eo *obj, double *from_degree, double *to_degree, Efl_Canvas_Object **pivot, double *cx, double *cy);

EOAPI void efl_animation_instance_rotate_absolute_set(Eo *obj, double from_degree, double to_degree, int cx, int cy);
EOAPI void efl_animation_instance_rotate_absolute_get(const Eo *obj, double *from_degree, double *to_degree, int *cx, int *cy);

EOAPI void efl_animation_instance_scale_set(Eo *obj, double from_scale_x, double from_scale_y, double to_scale_x, double to_scale_y, Efl_Canvas_Object *pivot, double cx, double cy);
EOAPI void efl_animation_instance_scale_get(const Eo *obj, double *from_scale_x, double *from_scale_y, double *to_scale_x, double *to_scale_y, Efl_Canvas_Object **pivot, double *cx, double *cy);

EOAPI void efl_animation_instance_scale_absolute_set(Eo *obj, double from_scale_x, double from_scale_y, double to_scale_x, double to_scale_y, int cx, int cy);
EOAPI void efl_animation_instance_scale_absolute_get(const Eo *obj, double *from_scale_x, double *from_scale_y, double *to_scale_x, double *to_scale_y, int *cx, int *cy);

EOAPI void efl_animation_instance_translate_set(Eo *obj, int from_x, int from_y, int to_x, int to_y);
EOAPI void efl_animation_instance_translate_get(const Eo *obj, int *from_x, int *from_y, int *to_x, int *to_y);

EOAPI void efl_animation_instance_translate_absolute_set(Eo *obj, int from_x, int from_y, int to_x, int to_y);
EOAPI void efl_animation_instance_translate_absolute_get(const Eo *obj, int *from_x, int *from_y, int *to_x, int *to_y);

EOAPI void efl_animation_instance_group_instance_add(Eo *obj, Efl_Animation_Instance *instance);

EOAPI void efl_animation_instance_group_instance_del(Eo *obj, Efl_Animation_Instance *instance);

EOAPI Eina_List *efl_animation_instance_group_instances_get(Eo *obj);

EWAPI extern const Efl_Event_Description _EFL_ANIMATION_INSTANCE_GROUP_EVENT_MEMBER_POST_END;
#define EFL_ANIMATION_INSTANCE_GROUP_EVENT_MEMBER_POST_END (&(_EFL_ANIMATION_INSTANCE_GROUP_EVENT_MEMBER_POST_END))

#ifdef __cplusplus
}
#endif

#undef EAPI
#define EAPI

#endif
