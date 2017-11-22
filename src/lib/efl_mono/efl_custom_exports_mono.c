#include "Eo.h"
#include "Eina.h"

#include <stdlib.h>
#include <string.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
#  define EAPI __declspec(dllexport)
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

EAPI void *efl_mono_native_alloc(unsigned int size)
{
   return malloc(size);
}

EAPI void efl_mono_native_free(void *ptr)
{
   free(ptr);
}

EAPI void efl_mono_native_free_ref(void **ptr)
{
   if (!ptr) return;
   free(*ptr);
}

EAPI void *efl_mono_native_alloc_copy(const void *val, unsigned int size)
{
    if (!val) return NULL;
    void *r = malloc(size);
    memcpy(r, val, size);
    return r;
}

EAPI const char *efl_mono_native_strdup(const char *str)
{
    if (!str) return NULL;
    return strdup(str);
}

EAPI int efl_mono_native_ptr_compare(const void *ptr1, const void *ptr2)
{
    uintptr_t addr1 = (uintptr_t)ptr1;
    uintptr_t addr2 = (uintptr_t)ptr2;
    return (addr1 > addr2) - (addr1 < addr2);
}

EAPI Eina_Compare_Cb efl_mono_native_ptr_compare_addr_get()
{
    return efl_mono_native_ptr_compare;
}

EAPI Eina_Compare_Cb efl_mono_native_str_compare_addr_get()
{
    return (Eina_Compare_Cb)strcmp;
}

EAPI Eina_Free_Cb efl_mono_native_free_addr_get()
{
    return (Eina_Free_Cb)free;
}

EAPI Eina_Free_Cb efl_mono_native_efl_unref_addr_get()
{
    return (Eina_Free_Cb)efl_unref;
}

// Iterator Wrapper //

typedef struct _Eina_Iterator_Wrapper_Mono
{
   Eina_Iterator iterator; // Must be the first
   Eina_Iterator *internal;
} Eina_Iterator_Wrapper_Mono;

static void *eina_iterator_wrapper_get_container_mono(Eina_Iterator_Wrapper_Mono *it)
{
   return eina_iterator_container_get(it->internal);
}

static void eina_iterator_wrapper_free_mono(Eina_Iterator_Wrapper_Mono *it)
{
   eina_iterator_free(it->internal);
   free(it);
}


static Eina_Iterator *eina_iterator_wrapper_new_mono(Eina_Iterator *internal, Eina_Iterator_Next_Callback next_cb)
{
   if (!internal) return NULL;

   Eina_Iterator_Wrapper_Mono *it = calloc(1, sizeof(Eina_Iterator_Wrapper_Mono));
   if (!it)
     {
        eina_iterator_free(internal);
        return NULL;
     }

   it->internal = internal;

   it->iterator.next = next_cb;

   it->iterator.version = EINA_ITERATOR_VERSION;
   it->iterator.get_container = FUNC_ITERATOR_GET_CONTAINER(eina_iterator_wrapper_get_container_mono);
   it->iterator.free = FUNC_ITERATOR_FREE(eina_iterator_wrapper_free_mono);

   EINA_MAGIC_SET(&it->iterator, EINA_MAGIC_ITERATOR);

   return &it->iterator;
}

// Array //

EAPI void eina_array_clean_custom_export_mono(Eina_Array *array) EINA_ARG_NONNULL(1)
{
   eina_array_clean(array);
}

EAPI Eina_Bool eina_array_push_custom_export_mono(Eina_Array *array, const void *data) EINA_ARG_NONNULL(1, 2)
{
   return eina_array_push(array, data);
}

EAPI void *eina_array_pop_custom_export_mono(Eina_Array *array) EINA_ARG_NONNULL(1)
{
   return eina_array_pop(array);
}

EAPI void *eina_array_data_get_custom_export_mono(const Eina_Array *array, unsigned int idx) EINA_ARG_NONNULL(1)
{
   return eina_array_data_get(array, idx);
}

EAPI void eina_array_data_set_custom_export_mono(const Eina_Array *array, unsigned int idx, const void *data) EINA_ARG_NONNULL(1)
{
   eina_array_data_set(array, idx, data);
}

EAPI unsigned int eina_array_count_custom_export_mono(const Eina_Array *array) EINA_ARG_NONNULL(1)
{
   return eina_array_count(array);
}

EAPI Eina_Bool eina_array_foreach_custom_export_mono(Eina_Array *array, Eina_Each_Cb cb, void *fdata)
{
   return eina_array_foreach(array, cb, fdata);
}

// List //

EAPI Eina_List *eina_list_last_custom_export_mono(const Eina_List *list)
{
   return eina_list_last(list);
}

EAPI Eina_List *eina_list_next_custom_export_mono(const Eina_List *list)
{
   return eina_list_next(list);
}

EAPI Eina_List *eina_list_prev_custom_export_mono(const Eina_List *list)
{
   return eina_list_prev(list);
}

EAPI void *eina_list_data_get_custom_export_mono(const Eina_List *list)
{
   return eina_list_data_get(list);
}

EAPI void *eina_list_data_set_custom_export_mono(Eina_List *list, const void *data)
{
   return eina_list_data_set(list, data);
}

EAPI unsigned int eina_list_count_custom_export_mono(const Eina_List *list)
{
   return eina_list_count(list);
}

EAPI void *eina_list_last_data_get_custom_export_mono(const Eina_List *list)
{
   return eina_list_last_data_get(list);
}

// Inlist //

typedef struct _Inlist_Node_Mono
{
   EINA_INLIST;
   char mem_start;
} Inlist_Node_Mono;

static Eina_Bool eina_inlist_iterator_wrapper_next_mono(Eina_Iterator_Wrapper_Mono *it, void **data)
{
   Inlist_Node_Mono *node = NULL;

   if (!eina_iterator_next(it->internal, (void**)&node))
     return EINA_FALSE;

   if (data)
     *data = &node->mem_start;

   return EINA_TRUE;
}

EAPI Eina_Iterator *eina_inlist_iterator_wrapper_new_custom_export_mono(const Eina_Inlist *in_list)
{
   return eina_iterator_wrapper_new_mono(eina_inlist_iterator_new(in_list), FUNC_ITERATOR_NEXT(eina_inlist_iterator_wrapper_next_mono));
}

EAPI Eina_Inlist *eina_inlist_first_custom_export_mono(const Eina_Inlist *list)
{
   return eina_inlist_first(list);
}

EAPI Eina_Inlist *eina_inlist_last_custom_export_mono(const Eina_Inlist *list)
{
   return eina_inlist_last(list);
}

EAPI Eina_Inlist *eina_inlist_next_custom_export_mono(const Eina_Inlist *list)
{
   if (list)
     return list->next;
   return NULL;
}

EAPI Eina_Inlist *eina_inlist_prev_custom_export_mono(const Eina_Inlist *list)
{
   if (list)
     return list->prev;
   return NULL;
}

// Hash //

static Eina_Bool eina_hash_iterator_ptr_key_wrapper_next_mono(Eina_Iterator_Wrapper_Mono *it, void **data)
{
   void **ptr = NULL;

   if (!eina_iterator_next(it->internal, (void**)&ptr))
     return EINA_FALSE;

   if (data)
     *data = *ptr;

   return EINA_TRUE;
}

EAPI Eina_Iterator *eina_hash_iterator_ptr_key_wrapper_new_custom_export_mono(const Eina_Hash *hash)
{
   return eina_iterator_wrapper_new_mono(eina_hash_iterator_key_new(hash), FUNC_ITERATOR_NEXT(eina_hash_iterator_ptr_key_wrapper_next_mono));
}

// Eina Value //
EAPI const Eina_Value_Type *type_int32() {
   return EINA_VALUE_TYPE_INT;
}
EAPI const Eina_Value_Type *type_uint32() {
   return EINA_VALUE_TYPE_UINT;
}
EAPI const Eina_Value_Type *type_string() {
   return EINA_VALUE_TYPE_STRING;
}
EAPI const Eina_Value_Type *type_array() {
   return EINA_VALUE_TYPE_ARRAY;
}

EAPI size_t eina_value_sizeof()
{
   return sizeof(Eina_Value);
}

EAPI Eina_Bool eina_value_set_wrapper(Eina_Value *value, va_list argp)
{
   return eina_value_set(value, argp);
}

EAPI Eina_Bool eina_value_setup_wrapper(Eina_Value *value,
                                   const Eina_Value_Type *type)
{
   return eina_value_setup(value, type);
}

EAPI void eina_value_flush_wrapper(Eina_Value *value)
{
   eina_value_flush(value);
}

EAPI const Eina_Value_Type *eina_value_type_get_wrapper(const Eina_Value *value)
{
   return eina_value_type_get(value);
}

EAPI Eina_Bool eina_value_get_wrapper(const Eina_Value *value, void *output)
{
   return eina_value_get(value, output);
}

EAPI int eina_value_compare_wrapper(const Eina_Value *this, const Eina_Value *other)
{
   return eina_value_compare(this, other);
}

EAPI Eina_Bool eina_value_array_setup_wrapper(Eina_Value *array, const Eina_Value_Type *subtype, unsigned int step)
{
   return eina_value_array_setup(array, subtype, step);
}

EAPI Eina_Bool eina_value_array_append_wrapper(Eina_Value *array, va_list argp)
{
   return eina_value_array_append(array, argp);
}

EAPI Eina_Bool eina_value_array_get_wrapper(const Eina_Value *array, int i, void *output)
{
   return eina_value_array_get(array, i, output);
}

EAPI Eina_Bool eina_value_array_set_wrapper(const Eina_Value *array, int i, void *value)
{
   return eina_value_array_set(array, i, value);
}

// Not actually a wrapper, but keeping the naming convention for functions on this file.
EAPI const Eina_Value_Type* eina_value_array_subtype_get_wrapper(const Eina_Value *array)
{
   Eina_Value_Array array_value;
   eina_value_get(array, &array_value);
   return array_value.subtype;
}

EAPI unsigned int eina_value_array_count_wrapper(const Eina_Value *array)
{
   return eina_value_array_count(array);
}
