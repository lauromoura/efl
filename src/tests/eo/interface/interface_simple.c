#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "Eo.h"
#include "interface_interface.h"
#include "interface_interface2.h"
#include "interface_simple.h"

typedef struct
{
   int a;
   int b;
} Private_Data;

#define MY_CLASS SIMPLE_CLASS

#define _GET_SET_FUNC(name) \
static int \
_##name##_get(Eo *obj EINA_UNUSED, void *class_data) \
{ \
   const Private_Data *pd = class_data; \
   printf("%s %s:%d %d\n", __func__, __FILE__, __LINE__, pd->name); \
   return pd->name; \
} \
static void \
_##name##_set(Eo *obj EINA_UNUSED, void *class_data, int name) \
{ \
   Private_Data *pd = class_data; \
   printf("%s %s:%d %d\n", __func__, __FILE__, __LINE__, pd->name); \
   pd->name = name; \
   printf("%s %s:%d %d\n", __func__, __FILE__, __LINE__, pd->name); \
} \
EO_FUNC_VOID_API_DEFINE(simple_##name##_set, EO_FUNC_CALL(name), int name); \
EO_FUNC_API_DEFINE(simple_##name##_get, int, 0, );

_GET_SET_FUNC(a)
_GET_SET_FUNC(b)

static int
_ab_sum_get(Eo *obj, void *class_data EINA_UNUSED)
{
   int a = 0, b = 0;
   eo_do(obj, a = simple_a_get(obj), b = simple_b_get(obj));
   printf("%s %s:%d %s %s\n", __func__, __FILE__, __LINE__, eo_class_name_get(MY_CLASS), __func__);
   return a + b;
}

static int
_ab_sum_get2(Eo *obj, void *class_data EINA_UNUSED)
{
   int a = 0, b = 0;
   eo_do(obj, a = simple_a_get(obj), b = simple_b_get(obj));
   printf("%s %s:%d %s %s\n", __func__, __FILE__, __LINE__, eo_class_name_get(MY_CLASS), __func__);
   return a + b + 1;
}

static Eo_Op_Description op_descs[] = {
     EO_OP_FUNC(simple_a_set, _a_set),
     EO_OP_FUNC(simple_a_get, _a_get),
     EO_OP_FUNC(simple_b_set, _b_set),
     EO_OP_FUNC(simple_b_get, _b_get),
     EO_OP_FUNC_OVERRIDE(interface_ab_sum_get, _ab_sum_get),
     EO_OP_FUNC_OVERRIDE(interface2_ab_sum_get2, _ab_sum_get2),
};

static const Eo_Class_Description class_desc = {
     EO_VERSION,
     "Simple",
     EO_CLASS_TYPE_REGULAR,
     EO_CLASS_DESCRIPTION_OPS(op_descs),
     NULL,
     sizeof(Private_Data),
     NULL,
     NULL
};

EO_DEFINE_CLASS(simple_class_get, &class_desc, EO_CLASS, INTERFACE2_CLASS, NULL);
