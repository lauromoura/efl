#define EFL_BETA_API_SUPPORT
#include <Eo.h>
#include "function_as_argument.eo.h"

typedef struct
{

} Function_As_Argument_Data;

EOLIAN static void
_function_as_argument_set_cb(Eo *obj, Function_As_Argument_Data *pd, SimpleFunc cb, void *cb_data, Eina_Free_Cb cb_free_cb)
{

}

EOLIAN static void
_function_as_argument_set_nonull_cb(Eo *obj, Function_As_Argument_Data *pd, VoidFunc cb, void *cb_data, Eina_Free_Cb cb_free_cb)
{

}

EOLIAN static char *
_function_as_argument_call_cb(Eo *obj, Function_As_Argument_Data *pd, int a, double b)
{

}

#include "function_as_argument.eo.c"
