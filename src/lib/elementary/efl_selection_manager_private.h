#ifndef EFL_SELECTION_MANAGER_PRIVATE_H
#define EFL_SELECTION_MANAGER_PRIVATE_H

#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#include <Elementary.h>
#include "elm_priv.h"

typedef struct _Efl_Selection_Manager_Data Efl_Selection_Manager_Data;

struct _Efl_Selection_Manager_Data
{
    Eo *obj;
    Ecore_Event_Handler *notify_handler;

    void *data_func_data;
    Efl_Selection_Data_Ready data_func;
    Eina_Free_Cb data_func_free_cb;

    Efl_Selection_Type type;
    Efl_Selection_Format format;
    void *buf;
    int len;

    Eo *sel_owner;
};

#endif
