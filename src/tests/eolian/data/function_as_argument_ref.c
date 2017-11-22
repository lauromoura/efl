
void _function_as_argument_set_cb(Eo *obj, Function_As_Argument_Data *pd, SimpleFunc cb, void *cb_data, Eina_Free_Cb cb_free_cb);

EOAPI EFL_VOID_FUNC_BODYV(function_as_argument_set_cb, EFL_FUNC_CALL(cb, cb_data, cb_free_cb), SimpleFunc cb, void *cb_data, Eina_Free_Cb cb_free_cb);

void _function_as_argument_set_nonull_cb(Eo *obj, Function_As_Argument_Data *pd, VoidFunc cb, void *cb_data, Eina_Free_Cb cb_free_cb);

EOAPI EFL_VOID_FUNC_BODYV(function_as_argument_set_nonull_cb, EFL_FUNC_CALL(cb, cb_data, cb_free_cb), VoidFunc cb, void *cb_data, Eina_Free_Cb cb_free_cb);

char *_function_as_argument_call_cb(Eo *obj, Function_As_Argument_Data *pd, int a, double b);

EOAPI EFL_FUNC_BODYV(function_as_argument_call_cb, char *, NULL, EFL_FUNC_CALL(a, b), int a, double b);

static Eina_Bool
_function_as_argument_class_initializer(Efl_Class *klass)
{
   const Efl_Object_Ops *opsp = NULL, *copsp = NULL;

#ifndef FUNCTION_AS_ARGUMENT_EXTRA_OPS
#define FUNCTION_AS_ARGUMENT_EXTRA_OPS
#endif

   EFL_OPS_DEFINE(ops,
      EFL_OBJECT_OP_FUNC(function_as_argument_set_cb, _function_as_argument_set_cb),
      EFL_OBJECT_OP_FUNC(function_as_argument_set_nonull_cb, _function_as_argument_set_nonull_cb),
      EFL_OBJECT_OP_FUNC(function_as_argument_call_cb, _function_as_argument_call_cb),
      FUNCTION_AS_ARGUMENT_EXTRA_OPS
   );
   opsp = &ops;

#ifdef FUNCTION_AS_ARGUMENT_EXTRA_CLASS_OPS
   EFL_OPS_DEFINE(cops, FUNCTION_AS_ARGUMENT_EXTRA_CLASS_OPS);
   copsp = &cops;
#endif

   return efl_class_functions_set(klass, opsp, copsp);
}

static const Efl_Class_Description _function_as_argument_class_desc = {
   EO_VERSION,
   "Function_As_Argument",
   EFL_CLASS_TYPE_REGULAR,
   sizeof(Function_As_Argument_Data),
   _function_as_argument_class_initializer,
   NULL,
   NULL
};

EFL_DEFINE_CLASS(function_as_argument_class_get, &_function_as_argument_class_desc, NULL, NULL);
