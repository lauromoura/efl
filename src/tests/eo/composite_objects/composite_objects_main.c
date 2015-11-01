#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#define EO_BASE_BETA

#include "Eo.h"
#include "composite_objects_simple.h"
#include "composite_objects_comp.h"

#include "../eunit_tests.h"

static int cb_called = EINA_FALSE;

static Eina_Bool
_a_changed_cb(void *data, Eo *obj, const Eo_Event_Description *desc, void *event_info)
{
   (void) desc;
   (void) obj;
   int new_a = *((int *) event_info);
   printf("%s event_info:'%d' data:'%s'\n", __func__, new_a, (const char *) data);

   cb_called = EINA_TRUE;

   return EO_CALLBACK_CONTINUE;
}

int
main(int argc, char *argv[])
{
   Eina_Bool tmp;
   (void) argc;
   (void) argv;
   eo_init();

   Eo *obj;
   eo_add(obj, COMP_CLASS, NULL);
   eo_do(obj, eo_event_callback_add(obj, EV_A_CHANGED, _a_changed_cb, NULL));

   fail_if(!eo_isa(obj, COMP_CLASS));
   fail_if(!eo_isa(obj, SIMPLE_CLASS));

   int a = 0;
   cb_called = EINA_FALSE;
   eo_do(obj, simple_a_set(obj, 1));
   fail_if(!cb_called);

   /* Test functions from all across the chain. */
   cb_called = EINA_FALSE;
   eo_do(obj, simple_a_set1(obj, 1));
   fail_if(!cb_called);
   cb_called = EINA_FALSE;
   eo_do(obj, simple_a_set15(obj, 1));
   fail_if(!cb_called);
   cb_called = EINA_FALSE;
   eo_do(obj, simple_a_set32(obj, 1));
   fail_if(!cb_called);

   eo_do(obj, a = simple_a_get(obj));
   fail_if(a != 1);

   /* disable the callback forwarder, and fail if it's still called. */
   Eo *simple = NULL;
   eo_do(obj, simple = eo_key_data_get(obj, "simple-obj"));
   eo_ref(simple);
   eo_do(simple, eo_event_callback_forwarder_del(obj, EV_A_CHANGED, obj));

   cb_called = EINA_FALSE;
   eo_do(obj, simple_a_set(obj, 2));
   fail_if(cb_called);

   fail_if(!eo_do_ret(simple, tmp,eo_composite_part_is(simple)));
   fail_if(!eo_do_ret(obj, tmp,eo_composite_detach(obj, simple)));
   fail_if(eo_do_ret(obj, tmp,eo_composite_detach(obj, simple)));
   fail_if(eo_do_ret(simple, tmp,eo_composite_part_is(simple)));
   fail_if(!eo_do_ret(obj, tmp,eo_composite_attach(obj, simple)));
   fail_if(!eo_do_ret(simple, tmp,eo_composite_part_is(simple)));
   fail_if(eo_do_ret(obj, tmp,eo_composite_attach(obj, simple)));

   fail_if(eo_do_ret(simple, tmp,eo_composite_attach(simple, obj)));

   eo_unref(simple);
   eo_unref(obj);

   eo_shutdown();
   return 0;
}

