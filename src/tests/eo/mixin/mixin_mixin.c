#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "Eo.h"
#include "mixin_mixin.h"
#include "mixin_simple.h"

#define MY_CLASS MIXIN_CLASS

static int
_ab_sum_get(Eo *obj, void *class_data EINA_UNUSED)
{
   int a = 0, b = 0;
   eo_do(obj, a = simple_a_get(obj), b = simple_b_get(obj));
   printf("%s %s\n", eo_class_name_get(MY_CLASS), __func__);
   return a + b;
}

static Eo *
_constructor(Eo *obj, void *class_data EINA_UNUSED)
{
   return eo_super_eo_constructor( MY_CLASS, obj);
}

static void
_destructor(Eo *obj, void *class_data EINA_UNUSED)
{
   eo_super_eo_destructor(MY_CLASS, obj);
}

EAPI EO_FUNC_API_DEFINE(mixin_ab_sum_get, int, 0,);

static Eo_Op_Description op_descs[] = {
     EO_OP_FUNC_OVERRIDE(eo_constructor, _constructor),
     EO_OP_FUNC_OVERRIDE(eo_destructor, _destructor),
     EO_OP_FUNC(mixin_ab_sum_get, _ab_sum_get),
};

static const Eo_Class_Description class_desc = {
     EO_VERSION,
     "Mixin",
     EO_CLASS_TYPE_MIXIN,
     EO_CLASS_DESCRIPTION_OPS(op_descs),
     NULL,
     0,
     NULL,
     NULL
};

EO_DEFINE_CLASS(mixin_class_get, &class_desc, NULL, EO_CLASS, NULL)

