#include "edje_private.h"
//#define DEBUG_TEXT 1

#ifdef DEBUG_TEXT
# define TPRN(...) do { printf(__VA_ARGS__); } while(0)
#else
# define TPRN(...)
#endif

static double
_edje_part_recalc_single_textblock_scale_range_adjust(Edje_Part_Description_Text *chosen_desc, double base_scale, double scale)
{
   double size, min, max;

   if (chosen_desc->text.size == 0)
     return scale;

   min = base_scale * chosen_desc->text.size_range_min;
   max = chosen_desc->text.size_range_max * base_scale;
   size = chosen_desc->text.size * scale;

   if ((size > max) && (max > 0))
     scale = max / (double)chosen_desc->text.size;
   else if (size < min)
     scale = min / (double)chosen_desc->text.size;

   return scale;
}

static void
_edje_part_recalc_textblock_font_get(Edje *ed, Edje_Real_Part *ep,
      Edje_Part_Description_Text *chosen_desc,
      const char *text,
      const char **ret_font_source,
      const char **ret_font, int *ret_size)

{
   Edje_Real_Part *source, *text_source;
   const char *font;
   int size;
   Eina_Bool inlined_font = EINA_FALSE;
   const char *font_source = NULL;
   char *sfont, *font2;

   sfont = font2 = NULL;

   text_source = ep->typedata.text->text_source;
   source = ep->typedata.text->source;

   if (source)
      font = _edje_text_class_font_get(ed,
            _edje_real_part_text_source_description_get(ep,
               &source), &size, &sfont);
   else
      font = _edje_text_class_font_get(ed, chosen_desc, &size, &sfont);

   if (!font) font = "";

   if (text_source)
     {
        if (text_source->typedata.text->text)
          {
             text = text_source->typedata.text->text;
          }
     }
   else
     {
        if (ep->typedata.text->text) text = ep->typedata.text->text;
     }

   if (source)
     {
        if (source->typedata.text->font) font = source->typedata.text->font;
        if (source->typedata.text->size > 0) size = source->typedata.text->size;
     }
   else
     {
        if (ep->typedata.text->font) font = ep->typedata.text->font;
        if (ep->typedata.text->size > 0) size = ep->typedata.text->size;
     }
   if (!text) text = "";

   if (ed->file->fonts)
     {
        Edje_Font_Directory_Entry *fnt;

        fnt = eina_hash_find(ed->file->fonts, font);

        if (fnt)
          {
             size_t len = strlen(font) + sizeof("edje/fonts/") + 1;
             font2 = alloca(len);
             sprintf(font2, "edje/fonts/%s", font);
             font = font2;
             inlined_font = 1;
             font2 = NULL; // so it is not freed at the end of the function
          }
     }
   if (inlined_font)
     {
        font_source = ed->path;
     }

   if ((_edje_fontset_append) && (font))
     {
        font2 = malloc(strlen(font) + 1 + strlen(_edje_fontset_append) + 1);
        if (font2)
          {
             strcpy(font2, font);
             strcat(font2, ",");
             strcat(font2, _edje_fontset_append);
             font = font2; //font2 needs to be freed at the end of the
             // function.
          }
     }
   if (ret_font) *ret_font = font;
   if (ret_size) *ret_size = size;
   if (ret_font_source) *ret_font_source = font_source;

   if (font2) free(font2);
   if (sfont) free(sfont);
}

/*
 * Legacy function for min/max calculation of textblock part.
 * It can't calculate min/max properly in many cases.
 *
 * To keep backward compatibility, it will be used for old version of EDJ files.
 * You can't see proper min/max result accroding to documents with this function.
 */
static void
_edje_part_recalc_single_textblock_min_max_calc_legacy(Edje_Real_Part *ep,
                                                       Edje_Part_Description_Text *chosen_desc,
                                                       Edje_Calc_Params *params,
                                                       int *minw, int *minh,
                                                       int *maxw, int *maxh)
{
   Evas_Coord tw, th, ins_l, ins_r, ins_t, ins_b;

   /* Legacy code for Textblock min/max calculation */
   if ((chosen_desc->text.min_x) || (chosen_desc->text.min_y))
     {
        int mw = 0, mh = 0;

        tw = th = 0;
        if (!chosen_desc->text.min_x)
          {
             efl_gfx_size_set(ep->object, EINA_SIZE2D(TO_INT(params->eval.w),  TO_INT(params->eval.h)));
             efl_canvas_text_size_formatted_get(ep->object, &tw, &th);
          }
        else
          evas_object_textblock_size_native_get(ep->object, &tw, &th);
        evas_object_textblock_style_insets_get(ep->object, &ins_l,
                                               &ins_r, &ins_t, &ins_b);
        mw = ins_l + tw + ins_r;
        mh = ins_t + th + ins_b;
        if (minw && chosen_desc->text.min_x)
          {
             if (mw > *minw) *minw = mw;
          }
        if (minh && chosen_desc->text.min_y)
          {
             if (mh > *minh) *minh = mh;
          }
     }

   if ((chosen_desc->text.max_x) || (chosen_desc->text.max_y))
     {
        int mw = 0, mh = 0;

        tw = th = 0;
        if (!chosen_desc->text.max_x)
          {
             efl_gfx_size_set(ep->object, EINA_SIZE2D(TO_INT(params->eval.w),  TO_INT(params->eval.h)));
             efl_canvas_text_size_formatted_get(ep->object, &tw, &th);
          }
        else
          evas_object_textblock_size_native_get(ep->object, &tw, &th);
        evas_object_textblock_style_insets_get(ep->object, &ins_l, &ins_r,
                                               &ins_t, &ins_b);
        mw = ins_l + tw + ins_r;
        mh = ins_t + th + ins_b;
        if (maxw && chosen_desc->text.max_x)
          {
             if (mw > *maxw) *maxw = mw;
             if (minw && (*maxw < *minw)) *maxw = *minw;
          }
        if (maxh && chosen_desc->text.max_y)
          {
             if (mh > *maxh) *maxh = mh;
             if (minh && (*maxh < *minh)) *maxh = *minh;
          }
     }
}

static void
_edje_part_recalc_single_textblock_min_max_calc(Edje_Real_Part *ep,
                                                Edje_Part_Description_Text *chosen_desc,
                                                Edje_Calc_Params *params,
                                                int *minw, int *minh,
                                                int *maxw, int *maxh)
{
   Evas_Coord tw, th, ins_l, ins_r, ins_t, ins_b;
   Evas_Coord min_calc_w = 0, min_calc_h = 0;
   unsigned char dmin_x, dmin_y;

   /* min_calc_* values need to save calculated minumum size
    * for maximum size calculation */
   if (minw) min_calc_w = *minw;
   if (minh) min_calc_h = *minh;

   dmin_x = chosen_desc->text.min_x;
   dmin_y = chosen_desc->text.min_y;

   if (dmin_x || dmin_y)
     {
        evas_object_textblock_style_insets_get(ep->object, &ins_l,
                                               &ins_r, &ins_t, &ins_b);

        tw = th = 0;
        if (!dmin_x)
          {
             /* text.min: 0 1
              * text.max: X X */
             int temp_h = TO_INT(params->eval.h);
             int temp_w = TO_INT(params->eval.w);

             if (min_calc_w > temp_w)
               temp_w = min_calc_w;
             if ((!chosen_desc->text.max_x) &&
                 maxw && (*maxw > -1) && (*maxw < temp_w))
               temp_w = *maxw;

             if (chosen_desc->text.max_y)
               {
                  /* text.min: 0 1
                   * text.max: X 1 */
                  temp_h = INT_MAX / 10000;
               }
             else if (maxh && (*maxh > TO_INT(params->eval.h)))
               {
                  /* text.min: 0 1
                   * text.max: X 0
                   * And there is a limit for height. */
                  temp_h = *maxh;
               }

             /* If base width for calculation is 0,
              * don't get meaningless height for multiline */
             if (temp_w > 0)
               {
                  efl_gfx_size_set(ep->object, EINA_SIZE2D(temp_w,  temp_h));
                  efl_canvas_text_size_formatted_get(ep->object, &tw, &th);

                  tw += ins_l + ins_r;
                  th += ins_t + ins_b;
               }
             else
               {
                  efl_canvas_text_size_native_get(ep->object, NULL, &th);

                  th += ins_t + ins_b;
               }
          }
        else
          {
             /* text.min: 1 X
              * text.max: X X */
             if (dmin_y && (!chosen_desc->text.max_x) &&
                 maxw && (*maxw > -1))
               {
                  /* text.min: 1 1
                   * text.max: 0 X */
                  int temp_w, temp_h;

                  temp_w = *maxw;
                  temp_h = INT_MAX / 10000;

                  if (min_calc_w > temp_w)
                    temp_w = min_calc_w;

                  if ((!chosen_desc->text.max_y) && maxh && (*maxh > -1))
                    {
                       /* text.min: 1 1
                        * text.max: 0 0
                        * There is limit for height. */
                       temp_h = *maxh;
                    }

                  efl_gfx_size_set(ep->object, EINA_SIZE2D(temp_w,  temp_h));
                  efl_canvas_text_size_formatted_get(ep->object, &tw, &th);

                  tw += ins_l + ins_r;
                  th += ins_t + ins_b;

                  /* If base width for calculation is 0,
                   * don't get meaningless height for multiline */
                  if (temp_w <= 0)
                    {
                       efl_canvas_text_size_native_get(ep->object, NULL, &th);

                       th += ins_t + ins_b;
                    }
               }
             else
               {
                  /* text.min: 1 X
                   * text.max: 1 X
                   * Or,
                   * text.min: 1 X
                   * text.max: 0 X without max width.
                   * It is a singleline Textblock. */
                  efl_canvas_text_size_native_get(ep->object, &tw, &th);

                  tw += ins_l + ins_r;
                  th += ins_t + ins_b;

                  if (!chosen_desc->text.max_x &&
                      (maxw && (*maxw > -1) && (*maxw < tw)))
                    {
                       /* text.min: 1 0
                        * text.max: 0 X */
                       tw = *maxw;
                    }
               }
          }

        if (tw > min_calc_w) min_calc_w = tw;
        if (th > min_calc_h) min_calc_h = th;
        if (dmin_x && minw) *minw = min_calc_w;
        if (dmin_y && minh) *minh = min_calc_h;
     }

   if ((chosen_desc->text.max_x) || (chosen_desc->text.max_y))
     {
        evas_object_textblock_style_insets_get(ep->object, &ins_l, &ins_r,
                                               &ins_t, &ins_b);

        tw = th = 0;
        if (!chosen_desc->text.max_x)
          {
             /* text.min: X X
              * text.max: 0 1 */
             int temp_w, temp_h;

             if (chosen_desc->text.min_y)
               {
                  /* text.min: X 1
                   * text.max: 0 1
                   * Already calculated in text for height. */
                  tw = TO_INT(params->eval.w);
                  if (min_calc_w > tw)
                    tw = min_calc_w;

                  th = min_calc_h;
               }
             else
               {
                  /* text.min: X 0
                   * text.max: 0 1 */
                  temp_w = TO_INT(params->eval.w);
                  temp_h = TO_INT(params->eval.h);

                  if (min_calc_w > temp_w)
                    temp_w = min_calc_w;
                  if (maxw && (*maxw > -1) && (*maxw < temp_w))
                    temp_w = *maxw;
                  if (min_calc_h > temp_h)
                    temp_h = min_calc_h;

                  /* If base width for calculation is 0,
                   * don't get meaningless height for multiline */
                  if (temp_w > 0)
                    {
                       efl_gfx_size_set(ep->object, EINA_SIZE2D(temp_w,  temp_h));
                       efl_canvas_text_size_formatted_get(ep->object, &tw, &th);

                       tw += ins_l + ins_r;
                       th += ins_t + ins_b;
                    }
                  else
                    {
                       efl_canvas_text_size_native_get(ep->object, NULL, &th);

                       th += ins_t + ins_b;
                    }
               }
          }
        else
          {
             /* text.max: 1 X */
             if (dmin_x)
               {
                  /* text.min: 1 X
                   * text.max: 1 X
                   * Singleline. */
                  efl_canvas_text_size_native_get(ep->object, &tw, &th);

                  tw += ins_l + ins_r;
                  th += ins_t + ins_b;
               }
             else
               {
                  /* text.min: 0 X
                   * text.max: 1 X */
                  if (chosen_desc->text.max_y)
                    {
                       /* text.min: 0 X
                        * text.max: 1 1 */
                       int temp_w, temp_h;

                       temp_w = TO_INT(params->eval.w);
                       temp_h = TO_INT(params->eval.h);

                       if (min_calc_w > temp_w)
                         temp_w = min_calc_w;
                       if (min_calc_h > temp_h)
                         temp_h = min_calc_h;

                       if (dmin_y)
                         {
                            /* text.min: 0 1
                             * text.max: 1 1
                             * There is no need to calculate it again. */
                            tw = min_calc_w;
                            th = min_calc_h;
                         }
                       else
                         {
                            /* text.min: 0 0
                             * text.max: 1 1 */

                            efl_gfx_size_set(ep->object, EINA_SIZE2D(temp_w,  temp_h));
                            efl_canvas_text_size_formatted_get(ep->object, &tw, &th);

                            tw += ins_l + ins_r;
                            th += ins_t + ins_b;

                            /* If base width for calculation is 0,
                             * don't get meaningless height for multiline */
                            if (temp_w <= 0)
                              {
                                 efl_canvas_text_size_native_get(ep->object, NULL, &th);

                                 th += ins_t + ins_b;
                              }
                         }
                    }
                  else
                    {
                       /* text.min: 0 X
                        * text.max: 1 0 */
                       int temp_w, temp_h;

                       temp_w = TO_INT(params->eval.w);
                       if (min_calc_w > temp_w)
                         temp_w = min_calc_w;

                       temp_h = efl_gfx_size_get(ep->object).h;
                       efl_gfx_size_set(ep->object, EINA_SIZE2D(temp_w,  temp_h));
                       efl_canvas_text_size_formatted_get(ep->object, &tw, &th);

                       tw += ins_l + ins_r;
                       th += ins_t + ins_b;

                       /* If base width for calculation is 0,
                        * don't get meaningless height for multiline */
                       if (temp_w <= 0)
                         {
                            efl_canvas_text_size_native_get(ep->object, NULL, &th);

                            th += ins_t + ins_b;
                         }
                    }
               }
          }

        if (maxw && chosen_desc->text.max_x)
          {
             if (tw > *maxw) *maxw = tw;
             if (minw && (*maxw < *minw)) *maxw = *minw;
          }
        if (maxh && chosen_desc->text.max_y)
          {
             if (th > *maxh) *maxh = th;
             if (minh && (*maxh < *minh)) *maxh = *minh;
          }
     }
}

static void
_edje_textblock_colors_set(Edje *ed EINA_UNUSED,
                           Edje_Real_Part *ep,
                           Edje_Calc_Params *params)
{

   Evas_Text_Style_Type style;
   Edje_Text_Effect effect;
   Efl_Text_Style_Effect_Type st;
   Efl_Text_Style_Shadow_Direction dir;

   style = EVAS_TEXT_STYLE_PLAIN;

   //evas_object_color_set(ep->object,
   //      (params->color.r * params->color.a) / 255,
   //      (params->color.g * params->color.a) / 255,
   //      (params->color.b * params->color.a) / 255,
   //      params->color.a);
   effect = ep->part->effect;
   switch (effect & EDJE_TEXT_EFFECT_MASK_BASIC)
     {
      case EDJE_TEXT_EFFECT_NONE:
      case EDJE_TEXT_EFFECT_PLAIN:
         st = EFL_TEXT_STYLE_EFFECT_TYPE_NONE;
         style = EVAS_TEXT_STYLE_PLAIN;
         break;

      case EDJE_TEXT_EFFECT_OUTLINE:
         style = EVAS_TEXT_STYLE_OUTLINE;
         st = EFL_TEXT_STYLE_EFFECT_TYPE_OUTLINE;
         efl_text_glow2_color_set(ep->object,
               params->type.text->color2.r,
               params->type.text->color2.g,
               params->type.text->color2.b,
               params->type.text->color2.a);
         break;

      case EDJE_TEXT_EFFECT_SOFT_OUTLINE:
         style = EVAS_TEXT_STYLE_SOFT_OUTLINE;
         st = EFL_TEXT_STYLE_EFFECT_TYPE_SOFT_OUTLINE;
         efl_text_glow2_color_set(ep->object,
               params->type.text->color2.r,
               params->type.text->color2.g,
               params->type.text->color2.b,
               params->type.text->color2.a);
         break;

      case EDJE_TEXT_EFFECT_SHADOW:
         style = EVAS_TEXT_STYLE_SHADOW;
         st = EFL_TEXT_STYLE_EFFECT_TYPE_SHADOW;
         efl_text_shadow_color_set(ep->object,
               params->type.text->color3.r,
               params->type.text->color3.g,
               params->type.text->color3.b,
               params->type.text->color3.a);
         break;

      case EDJE_TEXT_EFFECT_SOFT_SHADOW:
         style = EVAS_TEXT_STYLE_SOFT_SHADOW;
         st = EFL_TEXT_STYLE_EFFECT_TYPE_SOFT_SHADOW;
         efl_text_shadow_color_set(ep->object,
               params->type.text->color3.r,
               params->type.text->color3.g,
               params->type.text->color3.b,
               params->type.text->color3.a);
         break;

      case EDJE_TEXT_EFFECT_OUTLINE_SHADOW:
         style = EVAS_TEXT_STYLE_OUTLINE_SHADOW;
         st = EFL_TEXT_STYLE_EFFECT_TYPE_OUTLINE_SHADOW;
         efl_text_glow2_color_set(ep->object,
               params->type.text->color2.r,
               params->type.text->color2.g,
               params->type.text->color2.b,
               params->type.text->color2.a);
         efl_text_shadow_color_set(ep->object,
               params->type.text->color3.r,
               params->type.text->color3.g,
               params->type.text->color3.b,
               params->type.text->color3.a);
         break;

      case EDJE_TEXT_EFFECT_OUTLINE_SOFT_SHADOW:
         style = EVAS_TEXT_STYLE_OUTLINE_SOFT_SHADOW;
         st = EFL_TEXT_STYLE_EFFECT_TYPE_OUTLINE_SOFT_SHADOW;
         efl_text_glow2_color_set(ep->object,
               params->type.text->color2.r,
               params->type.text->color2.g,
               params->type.text->color2.b,
               params->type.text->color2.a);
         efl_text_shadow_color_set(ep->object,
               params->type.text->color3.r,
               params->type.text->color3.g,
               params->type.text->color3.b,
               params->type.text->color3.a);
         break;

      case EDJE_TEXT_EFFECT_FAR_SHADOW:
         style = EVAS_TEXT_STYLE_FAR_SHADOW;
         st = EFL_TEXT_STYLE_EFFECT_TYPE_FAR_SHADOW;
         evas_object_text_shadow_color_set(ep->object,
               params->type.text->color3.r,
               params->type.text->color3.g,
               params->type.text->color3.b,
               params->type.text->color3.a);
         break;

      case EDJE_TEXT_EFFECT_FAR_SOFT_SHADOW:
         style = EVAS_TEXT_STYLE_FAR_SOFT_SHADOW;
         st = EFL_TEXT_STYLE_EFFECT_TYPE_FAR_SOFT_SHADOW;
         evas_object_text_shadow_color_set(ep->object,
               params->type.text->color3.r,
               params->type.text->color3.g,
               params->type.text->color3.b,
               params->type.text->color3.a);
         break;

      case EDJE_TEXT_EFFECT_GLOW:
         style = EVAS_TEXT_STYLE_GLOW;
         st = EFL_TEXT_STYLE_EFFECT_TYPE_GLOW;
         efl_text_glow_color_set(ep->object,
               params->type.text->color2.r,
               params->type.text->color2.g,
               params->type.text->color2.b,
               params->type.text->color2.a);
         efl_text_glow2_color_set(ep->object,
               params->type.text->color3.r,
               params->type.text->color3.g,
               params->type.text->color3.b,
               params->type.text->color3.a);
         break;

      default:
         style = EVAS_TEXT_STYLE_PLAIN;
         st = EFL_TEXT_STYLE_EFFECT_TYPE_NONE;
         break;
     }

   switch (effect & EDJE_TEXT_EFFECT_MASK_SHADOW_DIRECTION)
     {
      case EDJE_TEXT_EFFECT_SHADOW_DIRECTION_BOTTOM_RIGHT:
        dir = EFL_TEXT_STYLE_SHADOW_DIRECTION_RIGHT;
        break;

      case EDJE_TEXT_EFFECT_SHADOW_DIRECTION_BOTTOM:
        dir = EFL_TEXT_STYLE_SHADOW_DIRECTION_BOTTOM;
        break;

      case EDJE_TEXT_EFFECT_SHADOW_DIRECTION_BOTTOM_LEFT:
        dir = EFL_TEXT_STYLE_SHADOW_DIRECTION_BOTTOM_LEFT;
         break;

      case EDJE_TEXT_EFFECT_SHADOW_DIRECTION_LEFT:
        dir = EFL_TEXT_STYLE_SHADOW_DIRECTION_LEFT;
         EVAS_TEXT_STYLE_SHADOW_DIRECTION_SET
            (style, EVAS_TEXT_STYLE_SHADOW_DIRECTION_LEFT);
         break;

      case EDJE_TEXT_EFFECT_SHADOW_DIRECTION_TOP_LEFT:
        dir = EFL_TEXT_STYLE_SHADOW_DIRECTION_TOP_LEFT;
         EVAS_TEXT_STYLE_SHADOW_DIRECTION_SET
            (style, EVAS_TEXT_STYLE_SHADOW_DIRECTION_TOP_LEFT);
         break;

      case EDJE_TEXT_EFFECT_SHADOW_DIRECTION_TOP:
        dir = EFL_TEXT_STYLE_SHADOW_DIRECTION_TOP;
         EVAS_TEXT_STYLE_SHADOW_DIRECTION_SET
            (style, EVAS_TEXT_STYLE_SHADOW_DIRECTION_TOP);
         break;

      case EDJE_TEXT_EFFECT_SHADOW_DIRECTION_TOP_RIGHT:
        dir = EFL_TEXT_STYLE_SHADOW_DIRECTION_TOP_RIGHT;
         EVAS_TEXT_STYLE_SHADOW_DIRECTION_SET
            (style, EVAS_TEXT_STYLE_SHADOW_DIRECTION_TOP_RIGHT);
         break;

      case EDJE_TEXT_EFFECT_SHADOW_DIRECTION_RIGHT:
        dir = EFL_TEXT_STYLE_SHADOW_DIRECTION_RIGHT;
         EVAS_TEXT_STYLE_SHADOW_DIRECTION_SET
            (style, EVAS_TEXT_STYLE_SHADOW_DIRECTION_RIGHT);
         break;

      default:
         dir = EFL_TEXT_STYLE_SHADOW_DIRECTION_TOP;
         break;
     }
   efl_text_effect_type_set(ep->object, st);
   efl_text_shadow_direction_set(ep->object, dir);
}

void
_edje_part_recalc_single_textblock(FLOAT_T sc,
                                   Edje *ed,
                                   Edje_Real_Part *ep,
                                   Edje_Part_Description_Text *chosen_desc,
                                   Edje_Calc_Params *params,
                                   int *minw, int *minh,
                                   int *maxw, int *maxh)
{
   if ((ep->type != EDJE_RP_TYPE_TEXT) ||
       (!ep->typedata.text))
     return;

   /* Handle alignment */

   if (!TEXT_LEGACY(ed))

     {
        FLOAT_T align_x, align_y;
        Efl_Text_Format_Vertical_Alignment_Type valign;
        Efl_Text_Format_Horizontal_Alignment_Type halign;
        Eina_Position2D pos;
        if (params->type.text->align.x < FROM_INT(0))
          {
             if (evas_object_text_direction_get(ep->object) ==
                   EVAS_BIDI_DIRECTION_RTL)
               {
                  align_x = FROM_INT(1);
               }
             else
               {
                  align_x = FROM_INT(0);
               }
          }
        else
          {
             align_x = params->type.text->align.x;
          }
        align_y = params->type.text->align.y;

        // Convert to Efl.Text
        valign = EFL_TEXT_VERTICAL_ALIGNMENT_TOP;
        if (EINA_DBL_EQ(align_y, 0.5))
          {
             valign = EFL_TEXT_VERTICAL_ALIGNMENT_CENTER;
          }
        else if (EINA_DBL_EQ(align_y, 1.0))
          {
             valign = EFL_TEXT_VERTICAL_ALIGNMENT_BOTTOM;
          }
        halign = EFL_TEXT_HORIZONTAL_ALIGNMENT_LEFT;
        if (EINA_DBL_EQ(align_x, 0.5))
          {
             halign = EFL_TEXT_HORIZONTAL_ALIGNMENT_CENTER;
          }
        else if (EINA_DBL_EQ(align_x, 1.0))
          {
             halign = EFL_TEXT_HORIZONTAL_ALIGNMENT_RIGHT;
          }

        TPRN("align: %.02f x %.02f, eva: %d x %d obj: %d x %d, pos: %d x %d, text: %d x %d\n",
              params->type.text->align.x,
              params->type.text->align.y,
              sw, sh, sz.w, sz.h,
              ed->x + TO_INT(params->eval.x),
              ed->y + TO_INT(params->eval.y),
              ttw, tth);

        pos.x = ed->x + TO_INT(params->eval.x) + ep->typedata.text->offset.x;
        pos.y = ed->y + TO_INT(params->eval.y) + ep->typedata.text->offset.y;
        efl_text_valign_set(ep->object, valign);
        efl_text_halign_set(ep->object, halign);
        TPRN("new pos: %d x %d\n", pos.x, pos.y);
        efl_gfx_position_set(ep->object, pos);
     }

   if (chosen_desc)
     {
        Evas_Coord tw, th;
        const char *text = "";
        const char *style = "";
        Edje_Style *stl = NULL;
        const char *tmp;
        Eina_List *l;

        if (chosen_desc->text.id_source >= 0)
          {
             Edje_Part_Description_Text *et;

             ep->typedata.text->source = ed->table_parts[chosen_desc->text.id_source % ed->table_parts_size];

             et = _edje_real_part_text_source_description_get(ep, NULL);
             tmp = edje_string_get(&et->text.style);
             if (tmp) style = tmp;
          }
        else
          {
             ep->typedata.text->source = NULL;

             tmp = edje_string_get(&chosen_desc->text.style);
             if (tmp) style = tmp;
          }

        if (chosen_desc->text.id_text_source >= 0)
          {
             Edje_Part_Description_Text *et;
             Edje_Real_Part *rp;

             ep->typedata.text->text_source = ed->table_parts[chosen_desc->text.id_text_source % ed->table_parts_size];

             et = _edje_real_part_text_text_source_description_get(ep, &rp);
             text = edje_string_get(&et->text.text);

             if (rp->typedata.text->text) text = rp->typedata.text->text;
          }
        else
          {
             ep->typedata.text->text_source = NULL;
             text = edje_string_get(&chosen_desc->text.text);
             if (ep->typedata.text->text) text = ep->typedata.text->text;
          }

        EINA_LIST_FOREACH(ed->file->styles, l, stl)
          {
             if ((stl->name) && (!strcmp(stl->name, style))) break;
             stl = NULL;
          }

        if (ep->part->scale)
          evas_object_scale_set(ep->object, TO_DOUBLE(sc));

        if ((chosen_desc->text.fit_x) || (chosen_desc->text.fit_y))
          {
             double base_s = 1.0;
             double orig_s;
             double s = base_s;

             if (ep->part->scale) base_s = TO_DOUBLE(sc);
             efl_canvas_object_scale_set(ep->object, base_s);
             efl_canvas_text_size_native_get(ep->object, &tw, &th);

             orig_s = base_s;
             /* Now make it bigger so calculations will be more accurate
              * and less influenced by hinting... */
             {
                orig_s = _edje_part_recalc_single_textblock_scale_range_adjust(chosen_desc, base_s,
                                                                               orig_s * TO_INT(params->eval.w) / tw);
                efl_canvas_object_scale_set(ep->object, orig_s);
                efl_canvas_text_size_native_get(ep->object, &tw, &th);
             }
             if (chosen_desc->text.fit_x)
               {
                  if (tw > 0)
                    {
                       s = _edje_part_recalc_single_textblock_scale_range_adjust(chosen_desc, base_s,
                                                                                 orig_s * TO_INT(params->eval.w) / tw);
                       efl_canvas_object_scale_set(ep->object, s);
                       efl_canvas_text_size_native_get(ep->object, NULL, NULL);
                    }
               }
             if (chosen_desc->text.fit_y)
               {
                  if (th > 0)
                    {
                       double tmp_s = _edje_part_recalc_single_textblock_scale_range_adjust(chosen_desc, base_s,
                                                                                            orig_s * TO_INT(params->eval.h) / th);
                       /* If we already have X fit, restrict Y to be no bigger
                        * than what we got with X. */
                       if (!((chosen_desc->text.fit_x) && (tmp_s > s)))
                         {
                            s = tmp_s;
                         }

                       efl_canvas_object_scale_set(ep->object, s);
                       efl_canvas_text_size_native_get(ep->object, NULL, NULL);
                    }
               }

             /* Final tuning, try going down 90% at a time, hoping it'll
              * actually end up being correct. */
             {
                int i = 5;   /* Tries before we give up. */
                Evas_Coord fw, fh;
                efl_canvas_text_size_native_get(ep->object, &fw, &fh);

                /* If we are still too big, try reducing the size to
                 * 95% each try. */
                while ((i > 0) &&
                       ((chosen_desc->text.fit_x && (fw > TO_INT(params->eval.w))) ||
                        (chosen_desc->text.fit_y && (fh > TO_INT(params->eval.h)))))
                  {
                     double tmp_s = _edje_part_recalc_single_textblock_scale_range_adjust(chosen_desc, base_s, s * 0.95);

                     /* Break if we are not making any progress. */
                     if (EQ(tmp_s, s))
                       break;
                     s = tmp_s;

                     efl_canvas_object_scale_set(ep->object, s);
                     efl_canvas_text_size_native_get(ep->object, &fw, &fh);
                     i--;
                  }
             }
          }

        const char *font, *font_source;
        int size;

        _edje_part_recalc_textblock_font_get(ed, ep, chosen_desc,
              text, &font_source, &font, &size);

        if (TEXT_LEGACY(ed) && stl)
          {
             if (evas_object_textblock_style_get(ep->object) != stl->style)
               evas_object_textblock_style_set(ep->object, stl->style);
          }
        else
          {
             FLOAT_T ellip = params->type.text->ellipsis;
             efl_text_font_set(ep->object, font, size);
             efl_text_normal_color_set(ep->object, 255, 255, 255, 255);
             efl_gfx_color_set(ep->object, 255, 255, 255, 255);
             efl_text_ellipsis_set(ep->object, (ellip == -1.0) ? -1.0 : 1.0 - ellip);
             _edje_textblock_colors_set(ed, ep, params);
          }

        // FIXME: need to account for editing
        if (ep->part->entry_mode > EDJE_ENTRY_EDIT_MODE_NONE)
          {
             // do nothing - should be done elsewhere
          }
        else
          {
             evas_object_textblock_text_markup_set(ep->object, text);
          }

        if ((ed->file->efl_version.major >= 1) && (ed->file->efl_version.minor >= 19))
          {
             _edje_part_recalc_single_textblock_min_max_calc(ep,
                   chosen_desc,
                   params,
                   minw, minh,
                   maxw, maxh);
          }
        else
          {
             _edje_part_recalc_single_textblock_min_max_calc_legacy(ep,
                   chosen_desc,
                   params,
                   minw, minh,
                   maxw, maxh);
          }
     }
}

#if 0
static void
_have_text(const char *text EINA_UNUSED)
{
   int a = 1;
   a += 1;
   TPRN("[[%s]]\n", text);
}

static void
_get_text(Edje_Real_Part *ep)
{
   const char *text = efl_text_get(ep->object);
   _have_text(text);
}
#endif

void
_edje_textblock_recalc_apply(Edje *ed, Edje_Real_Part *ep,
                             Edje_Calc_Params *params,
                             Edje_Part_Description_Text *chosen_desc)
{
   /* FIXME: this is just an hack. */
   FLOAT_T sc;

#if 0
   _get_text(ep);
#endif

   sc = DIV(ed->scale, ed->file->base_scale);
   if (EQ(sc, ZERO)) sc = DIV(_edje_scale, ed->file->base_scale);
   if (chosen_desc->text.fit_x || chosen_desc->text.fit_y)
     {
        _edje_part_recalc_single_textblock(sc, ed, ep, chosen_desc, params,
                                           NULL, NULL, NULL, NULL);
     }
}


