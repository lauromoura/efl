#ifndef ELM_TEST_FOCUS_COMMON_H
#define ELM_TEST_FOCUS_COMMON_H

#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define EFL_ACCESS_PROTECTED
#define ELM_INTERNAL_API_ARGESFSDFEFC
#include <Elementary.h>
#include "elm_suite.h"
#include "elm_widget.h"


#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef DLL_EXPORT
#  define EAPI __declspec(dllexport)
# else
#  define EAPI
# endif
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
#endif

#include "focus_test.eo.h"

#define TEST_OBJ_NEW(name, x, y, w, h) \
  Efl_Ui_Focus_Object* name; \
  name = elm_focus_test_object_new("" #name "",x, y, w, h); \


Efl_Ui_Focus_Object* elm_focus_test_object_new(const char *name, int x, int y, int w, int h);

void elm_focus_test_setup_cross(Efl_Ui_Focus_Object **middle,
                                Efl_Ui_Focus_Object **south,
                                Efl_Ui_Focus_Object **north,
                                Efl_Ui_Focus_Object **east,
                                Efl_Ui_Focus_Object **west);

Efl_Ui_Focus_Manager *elm_focus_test_manager_new(Efl_Ui_Focus_Object **middle);
#endif