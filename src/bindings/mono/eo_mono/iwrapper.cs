
using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;

namespace efl { namespace eo {

public class Globals {
    [DllImport("eo")] public static extern IntPtr
        _efl_add_internal_start([MarshalAs(UnmanagedType.LPStr)] String file, int line,
                                IntPtr klass, IntPtr parent, byte is_ref, byte is_fallback);
    [DllImport("eo")] public static extern IntPtr
        _efl_add_end(IntPtr eo, byte is_ref, byte is_fallback);
    [DllImport("eo")] public static extern IntPtr
        efl_ref(IntPtr eo);
    [DllImport("eo")] public static extern void
        efl_unref(IntPtr eo);
    [DllImport("eo")] public static extern IntPtr
        efl_class_new(IntPtr class_description, IntPtr base0);
    [DllImport("eo")] public static extern IntPtr
        efl_class_new(IntPtr class_description, IntPtr base0, IntPtr base1);
    [DllImport("eo")] public static extern IntPtr
        efl_class_new(IntPtr class_description, IntPtr base0, IntPtr base1, IntPtr base2);
    [DllImport("eo")] public static extern IntPtr
        efl_class_new(IntPtr class_description, IntPtr base0, IntPtr base1, IntPtr base2, IntPtr base3);
    [DllImport("eo")] public static extern IntPtr
        efl_class_new(IntPtr class_description, IntPtr base0, IntPtr base1, IntPtr base2, IntPtr base3, IntPtr base4);
    [DllImport("eo")] public static extern IntPtr
        efl_class_new(IntPtr class_description, IntPtr base0, IntPtr base1, IntPtr base2, IntPtr base3, IntPtr base4, IntPtr base5);
    [DllImport("eo")] public static extern IntPtr
        efl_class_new(IntPtr class_description, IntPtr base0, IntPtr base1, IntPtr base2, IntPtr base3, IntPtr base4, IntPtr base5, IntPtr base6);
    [DllImport("eo")] public static extern IntPtr
        efl_class_new(IntPtr class_description, IntPtr base0, IntPtr base1, IntPtr base2, IntPtr base3, IntPtr base4, IntPtr base5, IntPtr base6, IntPtr base7);
    [DllImport("eo")] public static extern byte efl_class_functions_set(IntPtr klass_id, IntPtr object_ops, IntPtr class_ops);
    [DllImport("eo")] public static extern IntPtr efl_data_scope_get(IntPtr obj, IntPtr klass);
    [DllImport("eo")] public static extern IntPtr efl_super(IntPtr obj, IntPtr klass);
    [DllImport("eo")] public static extern IntPtr efl_class_get(IntPtr obj);
    [DllImport("dl")] public static extern IntPtr dlsym
       (IntPtr handle, [MarshalAs(UnmanagedType.LPStr)] String name);
    [DllImport("dl")] public static extern IntPtr dlopen(String name, int flags);

   [DllImport("eo")] public static extern bool efl_event_callback_priority_add(
              System.IntPtr obj,
              // FIXME commented to allow passing null stuff during test
              /* ref efl.kw_event.Description desc, */
              efl.kw_event.Description desc,
              short priority,
              efl.Event_Cb cb,
              System.IntPtr data);
   [DllImport("eo")] public static extern bool efl_event_callback_del(
              System.IntPtr obj,
              efl.kw_event.Description desc,
              efl.Event_Cb cb,
              System.IntPtr data);


    public const int RTLD_NOW = 2;

    public delegate byte class_initializer(IntPtr klass);
    
    public static IntPtr register_class(class_initializer initializer, IntPtr base_klass)
    {
        ClassDescription description;
        description.version = 2; // EO_VERSION
        description.name = "BoxInherit";
        description.class_type = 0; // REGULAR
        description.data_size = (UIntPtr)8;
        description.class_initializer = IntPtr.Zero;
        description.class_constructor = IntPtr.Zero;
        description.class_destructor = IntPtr.Zero;

        if(initializer != null)
            description.class_initializer = Marshal.GetFunctionPointerForDelegate(initializer);

        IntPtr description_ptr = Marshal.AllocHGlobal(Marshal.SizeOf(description));
        Marshal.StructureToPtr(description, description_ptr, false);
      
        eina.Log.Debug("Going to register!");
        IntPtr klass = efl.eo.Globals.efl_class_new(description_ptr, base_klass, IntPtr.Zero);
        if(klass == IntPtr.Zero)
            eina.Log.Error("klass was not registered");
        eina.Log.Debug("Registered?");
        return klass;
    }
    public static IntPtr instantiate(IntPtr klass, efl.Object parent)
    {
        eina.Log.Debug("Instantiating");
        System.IntPtr parent_ptr = System.IntPtr.Zero;
        if(parent != null)
            parent_ptr = parent.raw_handle;

        System.IntPtr eo = efl.eo.Globals._efl_add_internal_start("file", 0, klass, parent_ptr, 0, 0);
        eina.Log.Debug("efl_add_internal_start returned");
        eo = efl.eo.Globals._efl_add_end(eo, 0, 0);
        eina.Log.Debug("efl_add_end returned");
        return eo;
    }
    public static void data_set(efl.eo.IWrapper obj)
    {
      IntPtr pd = efl.eo.Globals.efl_data_scope_get(obj.raw_handle, obj.raw_klass);
      {
          GCHandle gch = GCHandle.Alloc(obj);
          EolianPD epd;
          epd.pointer = GCHandle.ToIntPtr(gch);
          Marshal.StructureToPtr(epd, pd, false);
      }
    }
    public static efl.eo.IWrapper data_get(IntPtr pd)
    {
        EolianPD epd = (EolianPD)Marshal.PtrToStructure(pd, typeof(EolianPD));
        if(epd.pointer != IntPtr.Zero)
        {
            GCHandle gch = GCHandle.FromIntPtr(epd.pointer);
            return (efl.eo.IWrapper)gch.Target;
        }
        else
            return null;
    }

    public static IntPtr cached_string_to_intptr(Dictionary<String, IntPtr> dict, String str)
    {
        IntPtr ptr = IntPtr.Zero;
        if (!dict.TryGetValue(str, out ptr))
        {
            ptr = Marshal.StringToHGlobalAuto(str);
            dict[str] = ptr;
        }

        return ptr;
    }

    public static void free_dict_values(Dictionary<String, IntPtr> dict)
    {
        foreach(IntPtr ptr in dict.Values)
        {
            Marshal.FreeHGlobal(ptr);
        }
    }
}        
        
public interface IWrapper
{
    IntPtr raw_handle
    {
        get;
    }
    IntPtr raw_klass
    {
        get;
    }
}

public class MarshalTest<T> : ICustomMarshaler
{
    public static ICustomMarshaler GetInstance(string cookie)
    {
        eina.Log.Debug("MarshalTest.GetInstace cookie " + cookie);
        return new MarshalTest<T>();
    }
    public void CleanUpManagedData(object ManagedObj)
    {
        eina.Log.Warning("MarshalTest.CleanUpManagedData not implemented");
        //throw new NotImplementedException();
    }

    public void CleanUpNativeData(IntPtr pNativeData)
    {
        eina.Log.Warning("MarshalTest.CleanUpNativeData not implemented");
        //throw new NotImplementedException();
    }

    public int GetNativeDataSize()
    {
        eina.Log.Debug("MarshalTest.GetNativeDataSize");
        return 0;
        //return 8;
    }

    public IntPtr MarshalManagedToNative(object ManagedObj)
    {
        eina.Log.Debug("MarshalTest.MarshallManagedToNative");
        return ((IWrapper)ManagedObj).raw_handle;
    }

    public object MarshalNativeToManaged(IntPtr pNativeData)
    {
        eina.Log.Debug("MarshalTest.MarshalNativeToManaged");
        return Activator.CreateInstance(typeof(T), new System.Object[] {pNativeData});
//        return null;
    }
}

} }
