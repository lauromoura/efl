
void _owning_test1(Eo *obj, Owning_Data *pd, Eina_List *test1, Eina_Iterator *test2, Eina_Hash *test3, Eina_Accessor *test4);

static void
_owning_test1_ownership_fallback(Eina_List *test1, Eina_Iterator *test2, Eina_Hash *test3, Eina_Accessor *test4)
{
   eina_list_free(test1);
   eina_iterator_free(test2);
   eina_hash_free_cb_set(test3,NULL);
   eina_hash_free(test3);
   eina_accessor_free(test4);
}

EOAPI EFL_VOID_FUNC_BODYV_FALLBACK(owning_test1, _owning_test1_ownership_fallback(test1, test2, test3, test4);, EFL_FUNC_CALL(test1, test2, test3, test4), Eina_List *test1, Eina_Iterator *test2, Eina_Hash *test3, Eina_Accessor *test4);

void _owning_test2(Eo *obj, Owning_Data *pd, Eina_List *test1, Eina_Iterator *test2, Eina_Hash *test3, Eina_Accessor *test4);

static void
_owning_test2_ownership_fallback(Eina_List *test1, Eina_Iterator *test2, Eina_Hash *test3, Eina_Accessor *test4)
{
   B* test1_iter;
   EINA_LIST_FREE(test1,test1_iter)
     {
        free_b(test1_iter);
     }
   B* test2_iter;
   EINA_ITERATOR_FOREACH(test2,test2_iter)
     {
        free_b(test2_iter);
     }
   eina_hash_free_cb_set(test3,NULL);
   eina_hash_free(test3);
   B* test4_iter;
   unsigned int test4_i = 0;
   EINA_ACCESSOR_FOREACH(test4,test4_i,test4_iter)
     {
        free_b(test4_iter);
     }
}

EOAPI EFL_VOID_FUNC_BODYV_FALLBACK(owning_test2, _owning_test2_ownership_fallback(test1, test2, test3, test4);, EFL_FUNC_CALL(test1, test2, test3, test4), Eina_List *test1, Eina_Iterator *test2, Eina_Hash *test3, Eina_Accessor *test4);

static Eina_Bool
_owning_class_initializer(Efl_Class *klass)
{
   const Efl_Object_Ops *opsp = NULL, *copsp = NULL;

#ifndef OWNING_EXTRA_OPS
#define OWNING_EXTRA_OPS
#endif

   EFL_OPS_DEFINE(ops,
      EFL_OBJECT_OP_FUNC(owning_test1, _owning_test1),
      EFL_OBJECT_OP_FUNC(owning_test2, _owning_test2),
      OWNING_EXTRA_OPS
   );
   opsp = &ops;

#ifdef OWNING_EXTRA_CLASS_OPS
   EFL_OPS_DEFINE(cops, OWNING_EXTRA_CLASS_OPS);
   copsp = &cops;
#endif

   return efl_class_functions_set(klass, opsp, copsp);
}

static const Efl_Class_Description _owning_class_desc = {
   EO_VERSION,
   "Owning",
   EFL_CLASS_TYPE_REGULAR,
   sizeof(Owning_Data),
   _owning_class_initializer,
   NULL,
   NULL
};

EFL_DEFINE_CLASS(owning_class_get, &_owning_class_desc, NULL, NULL);
