#include "edje_private.h"

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

   /* min_calc_* values need to save calculated minumum size
    * for maximum size calculation */
   if (minw) min_calc_w = *minw;
   if (minh) min_calc_h = *minh;

   if ((chosen_desc->text.min_x) || (chosen_desc->text.min_y))
     {
        evas_object_textblock_style_insets_get(ep->object, &ins_l,
                                               &ins_r, &ins_t, &ins_b);

        tw = th = 0;
        if (!chosen_desc->text.min_x)
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
             if (chosen_desc->text.min_y && (!chosen_desc->text.max_x) &&
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
        if (chosen_desc->text.min_x && minw) *minw = min_calc_w;
        if (chosen_desc->text.min_y && minh) *minh = min_calc_h;
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
             if (chosen_desc->text.min_x)
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

                       if (chosen_desc->text.min_y)
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

        if (stl)
          {
             if (evas_object_textblock_style_get(ep->object) != stl->style)
               evas_object_textblock_style_set(ep->object, stl->style);
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

        evas_object_textblock_valign_set(ep->object, TO_DOUBLE(chosen_desc->text.align.y));
     }
}

void
_edje_textblock_recalc_apply(Edje *ed, Edje_Real_Part *ep,
                             Edje_Calc_Params *params,
                             Edje_Part_Description_Text *chosen_desc)
{
   /* FIXME: this is just an hack. */
   FLOAT_T sc;
   sc = DIV(ed->scale, ed->file->base_scale);
   if (EQ(sc, ZERO)) sc = DIV(_edje_scale, ed->file->base_scale);
   if (chosen_desc->text.fit_x || chosen_desc->text.fit_y)
     {
        _edje_part_recalc_single_textblock(sc, ed, ep, chosen_desc, params,
                                           NULL, NULL, NULL, NULL);
     }
}


