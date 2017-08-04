#define CODE_ANALYSIS

using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Security.Permissions;
using System.Security;
using System.Diagnostics.CodeAnalysis;
using System.Runtime.Serialization;

using static eina.EinaNative.UnsafeNativeMethods;
using static eina.TraitFunctions;


namespace eina {

namespace EinaNative {

// Structs to be passed from/to C when dealing with containers and
// optional values.
[StructLayout(LayoutKind.Sequential)]
struct Value_Array
{
    public IntPtr subtype;
    public uint step;
    public IntPtr subarray;
}

[StructLayout(LayoutKind.Sequential)]
struct Value_List
{
    public IntPtr subtype;
    public IntPtr sublist;
}

[SuppressUnmanagedCodeSecurityAttribute]
static internal class UnsafeNativeMethods {

    [DllImport("eina")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_convert(IntPtr handle, IntPtr convert);

    // Wrapped and helper methods
    [DllImport("eflcustomexportsmono")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern int eina_value_sizeof();

    [DllImport("eflcustomexportsmono", CharSet=CharSet.Auto)]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_set_wrapper(IntPtr handle, string value);

    [DllImport("eflcustomexportsmono")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_set_wrapper(IntPtr handle, int value);

    [DllImport("eflcustomexportsmono")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_set_wrapper(IntPtr handle, uint value);

    [DllImport("eflcustomexportsmono")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_set_wrapper(IntPtr handle, IntPtr value);

    [DllImport("eflcustomexportsmono")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_setup_wrapper(IntPtr handle, IntPtr type);

    [DllImport("eflcustomexportsmono")]
    internal static extern void eina_value_flush_wrapper(IntPtr handle);

    [DllImport("eflcustomexportsmono")]
    internal static extern IntPtr eina_value_type_get_wrapper(IntPtr handle);

    [DllImport("eflcustomexportsmono")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_get_wrapper(IntPtr handle, out IntPtr output);

    [DllImport("eflcustomexportsmono")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_get_wrapper(IntPtr handle, out int output);

    [DllImport("eflcustomexportsmono")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_get_wrapper(IntPtr handle, out uint output);

    [DllImport("eflcustomexportsmono")]
    internal static extern int eina_value_compare_wrapper(IntPtr handle, IntPtr other);

    [DllImport("eina", CharSet=CharSet.Auto)]
    internal static extern String eina_value_to_string(IntPtr handle); // We take ownership of the returned string.

    [DllImport("eflcustomexportsmono")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_array_setup_wrapper(IntPtr handle, IntPtr subtype, uint step);

    [DllImport("eflcustomexportsmono")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_list_setup_wrapper(IntPtr handle, IntPtr subtype);

    [DllImport("eflcustomexportsmono")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_array_append_wrapper(IntPtr handle, IntPtr data);

    [DllImport("eflcustomexportsmono")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_list_append_wrapper(IntPtr handle, IntPtr data);

    [DllImport("eflcustomexportsmono")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_array_get_wrapper(IntPtr handle, int index, out IntPtr output);

    [DllImport("eflcustomexportsmono")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_list_get_wrapper(IntPtr handle, int index, out IntPtr output);

    [DllImport("eflcustomexportsmono")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_array_set_wrapper(IntPtr handle, int index, IntPtr value);

    [DllImport("eflcustomexportsmono")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_list_set_wrapper(IntPtr handle, int index, IntPtr value);

    [DllImport("eflcustomexportsmono")]
    internal static extern IntPtr eina_value_array_subtype_get_wrapper(IntPtr handle);

    [DllImport("eflcustomexportsmono")]
    internal static extern IntPtr eina_value_list_subtype_get_wrapper(IntPtr handle);

    [DllImport("eflcustomexportsmono")]
    internal static extern uint eina_value_array_count_wrapper(IntPtr handle);

    [DllImport("eflcustomexportsmono")]
    internal static extern uint eina_value_list_count_wrapper(IntPtr handle);

    [DllImport("eflcustomexportsmono")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_optional_empty_is_wrapper(IntPtr handle, [MarshalAsAttribute(UnmanagedType.U1)] out bool empty);

    [DllImport("eina")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_optional_pset(IntPtr handle, IntPtr subtype, ref int value);

    [DllImport("eina")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_optional_pset(IntPtr handle, IntPtr subtype, ref uint value);

    [DllImport("eina", CharSet=CharSet.Auto)]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_optional_pset(IntPtr handle, IntPtr subtype, ref string value);

    [DllImport("eina")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_optional_pset(IntPtr handle, IntPtr subtype, ref IntPtr value);

    [DllImport("eina")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_optional_reset(IntPtr handle);

    [DllImport("eina")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_optional_pget(IntPtr handle, out eina.EinaNative.Value_Array output);

    [DllImport("eina")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_optional_pget(IntPtr handle, out eina.EinaNative.Value_List output);

    [DllImport("eina")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_optional_pget(IntPtr handle, out IntPtr output);

    [DllImport("eina")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_optional_pget(IntPtr handle, out int output);

    [DllImport("eina")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_optional_pget(IntPtr handle, out uint output);

    [DllImport("eflcustomexportsmono")]
    internal static extern IntPtr eina_value_optional_type_get_wrapper(IntPtr handle);

    [DllImport("eflcustomexportsmono")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_pset_wrapper(IntPtr handle, ref eina.EinaNative.Value_Array ptr);

    [DllImport("eflcustomexportsmono")]
    [return: MarshalAsAttribute(UnmanagedType.U1)]
    internal static extern bool eina_value_pset_wrapper(IntPtr handle, ref eina.EinaNative.Value_List ptr);

    // Supported types
    [DllImport("eflcustomexportsmono")]
    internal static extern IntPtr type_int32();
    [DllImport("eflcustomexportsmono")]
    internal static extern IntPtr type_uint32();
    [DllImport("eflcustomexportsmono")]
    internal static extern IntPtr type_string();
    [DllImport("eflcustomexportsmono")]
    internal static extern IntPtr type_array();
    [DllImport("eflcustomexportsmono")]
    internal static extern IntPtr type_list();
    [DllImport("eflcustomexportsmono")]
    internal static extern IntPtr type_optional();
}
}

/// <summary>Struct for passing Values by value to Unmanaged functions.</summary>
[StructLayout(LayoutKind.Sequential)]
public struct Value_Native
{
    IntPtr type;
    IntPtr value; // Atually an Eina_Value_Union, but it is padded to 8 bytes.
}


/// <summary>Exception for trying to access flushed values.</summary>
[Serializable]
public class ValueFlushedException : Exception
{
    /// <summary> Default constructor.</summary>
    public ValueFlushedException() : base () { }
    /// <summary> Most commonly used contructor.</summary>
    public ValueFlushedException(string msg) : base(msg) { }
    /// <summary> Wraps an inner exception.</summary>
    public ValueFlushedException(string msg, Exception inner) : base(msg, inner) { }
    /// <summary> Serializable constructor.</summary>
    protected ValueFlushedException(SerializationInfo info, StreamingContext context) : base(info, context) { }
}

/// <summary>Exception for failures when setting an container item.</summary>
[Serializable]
public class SetItemFailedException : Exception
{
    /// <summary> Default constructor.</summary>
    public SetItemFailedException() : base () { }
    /// <summary> Most commonly used contructor.</summary>
    public SetItemFailedException(string msg) : base(msg) { }
    /// <summary> Wraps an inner exception.</summary>
    public SetItemFailedException(string msg, Exception inner) : base(msg, inner) { }
    /// <summary> Serializable constructor.</summary>
    protected SetItemFailedException(SerializationInfo info, StreamingContext context) : base(info, context) { }
}

/// <summary>Exception for methods that must have been called on a container.</summary>
[Serializable]
public class InvalidValueTypeException: Exception
{
    /// <summary> Default constructor.</summary>
    public InvalidValueTypeException() : base () { }
    /// <summary> Most commonly used contructor.</summary>
    public InvalidValueTypeException(string msg) : base(msg) { }
    /// <summary> Wraps an inner exception.</summary>
    public InvalidValueTypeException(string msg, Exception inner) : base(msg, inner) { }
    /// <summary> Serializable constructor.</summary>
    protected InvalidValueTypeException(SerializationInfo info, StreamingContext context) : base(info, context) { }
}

/// <summary>Internal enum to handle value ownership between managed and unmanaged code.</summary>
public enum ValueOwnership {
    /// <summary> The value is owned by the managed code. It'll free the handle on disposal.</summary>
    Managed,
    /// <summary> The value is owned by the unmanaged code. It won't be freed on disposal.</summary>
    Unmanaged
}

/// <summary>Managed-side Enum to represent Eina_Value_Type constants</summary>
public enum ValueType {
    /// <summary>Signed 32 bit integer. Same as 'int'</summary>
    Int32,
    /// <summary>Unsigned 32 bit integer. Same as 'uint'</summary>
    UInt32,
    /// <summary>Strings</summary>
    String,
    /// <summary>Array of Value items.</summary>
    Array,
    /// <summary>Linked list of Value items.</summary>
    List,
    /// <summary>Map of string keys to Value items.</summary>
    Hash,
    /// <summary>Optional (aka empty) values.</summary>
    Optional,
}

static class ValueTypeMethods {
    public static bool IsNumeric(this ValueType val)
    {
        switch (val) {
            case ValueType.Int32:
            case ValueType.UInt32:
                return true;
            default:
                return false;
        }
    }

    public static bool IsString(this ValueType val)
    {
        switch(val) {
            case ValueType.String:
                return true;
            default:
                return false;
        }
    }

    public static bool IsContainer(this ValueType val)
    {
        switch(val) {
            case ValueType.Array:
            case ValueType.List:
            case ValueType.Hash:
                return true;
            default:
                return false;
        }
    }

    public static bool IsOptional(this ValueType val)
    {
        return val == ValueType.Optional;
    }
}
static class ValueTypeBridge
{
    private static Dictionary<ValueType, IntPtr> ManagedToNative = new Dictionary<ValueType, IntPtr>();
    private static Dictionary<IntPtr, ValueType> NativeToManaged = new Dictionary<IntPtr, ValueType>();
    private static bool TypesLoaded; // CLR defaults to false;

    public static ValueType GetManaged(IntPtr native)
    {
        if (!TypesLoaded)
            LoadTypes();

        return NativeToManaged[native];
    }

    public static IntPtr GetNative(ValueType valueType)
    {
        if (!TypesLoaded)
            LoadTypes();

        return ManagedToNative[valueType];
    }

    private static void LoadTypes()
    {
        ManagedToNative.Add(ValueType.Int32, type_int32());
        NativeToManaged.Add(type_int32(), ValueType.Int32);

        ManagedToNative.Add(ValueType.UInt32, type_uint32());
        NativeToManaged.Add(type_uint32(), ValueType.UInt32);

        ManagedToNative.Add(ValueType.String, type_string());
        NativeToManaged.Add(type_string(), ValueType.String);

        ManagedToNative.Add(ValueType.Array, type_array());
        NativeToManaged.Add(type_array(), ValueType.Array);

        ManagedToNative.Add(ValueType.List, type_list());
        NativeToManaged.Add(type_list(), ValueType.List);

        ManagedToNative.Add(ValueType.Optional, type_optional());
        NativeToManaged.Add(type_optional(), ValueType.Optional);

        TypesLoaded = true;
    }
}

/// <summary>Wrapper around Eina_Value generic storage.
///
/// <para>Eina_Value is EFL's main workhorse to deal with storing generic data in
/// an uniform way.</para>
///
/// <para>It comes with predefined types for numbers, strings, array, list, hash,
/// blob and structs. It is able to convert between data types, including
/// to string.</para>
/// </summary>
public class Value : IDisposable, IComparable<Value>, IEquatable<Value>
{

    // Unmanaged type - Managed type mapping
    // EINA_VALUE_TYPE_UCHAR: unsigned char -- byte
    // EINA_VALUE_TYPE_USHORT: unsigned short -- ushort
    // Ok EINA_VALUE_TYPE_UINT: unsigned int -- uint
    // EINA_VALUE_TYPE_ULONG: unsigned long -- ulong
    // EINA_VALUE_TYPE_UINT64: uint64_t -- ulong
    // EINA_VALUE_TYPE_CHAR: char -- sbyte
    // EINA_VALUE_TYPE_SHORT: short -- short
    // Ok EINA_VALUE_TYPE_INT: int -- int
    // EINA_VALUE_TYPE_LONG: long -- long
    // EINA_VALUE_TYPE_INT64: int64_t -- long
    // EINA_VALUE_TYPE_FLOAT: float -- float
    // EINA_VALUE_TYPE_DOUBLE: double -- double
    // EINA_VALUE_TYPE_STRINGSHARE: const char * -- string
    // Ok EINA_VALUE_TYPE_STRING: const char * -- string
    // Ok EINA_VALUE_TYPE_ARRAY: Eina_Value_Array -- eina.Array?
    // Ok EINA_VALUE_TYPE_LIST: Eina_Value_List -- eina.List?
    // EINA_VALUE_TYPE_HASH: Eina_Value_Hash -- eina.Hash?
    // EINA_VALUE_TYPE_TIMEVAL: struct timeval -- FIXME
    // EINA_VALUE_TYPE_BLOB: Eina_Value_Blob -- FIXME
    // EINA_VALUE_TYPE_STRUCT: Eina_Value_Struct -- FIXME


    public IntPtr Handle { get; protected set;}
    public ValueOwnership Ownership { get; protected set;}
    private bool Disposed;
    public bool Flushed { get; protected set;}
    public bool Optional {
        get {
            return GetValueType() == eina.ValueType.Optional;
        }
        /* protected set {
            // Should we expose this?
            // Something like {
            //    flush(handle)/free(handle)
            //    handle = eina_value_optional_empty_new()
            // }
         } */
    }
    public bool Empty {
        get {
            OptionalSanityChecks();
            bool empty;
            if (!eina_value_optional_empty_is_wrapper(this.Handle, out empty))
                throw new System.InvalidOperationException("Couldn't get the empty information");
            else
                return empty;
        }
    }

    // Constructor to be used by the "FromContainerDesc" methods.
    private Value() {
        this.Handle = Marshal.AllocHGlobal(eina_value_sizeof());
        this.Ownership = ValueOwnership.Managed;
    }

    internal Value(IntPtr handle, ValueOwnership ownership=ValueOwnership.Managed) {
        this.Handle = handle;
        this.Ownership = ownership;
    }

    /// <summary>Creates a new value storage for values of type 'type'.</summary>
    public Value(ValueType type)
    {
        if (type.IsContainer())
            throw new ArgumentException("To use container types you must provide a subtype");
        this.Handle = Marshal.AllocHGlobal(eina_value_sizeof());
        this.Ownership = ValueOwnership.Managed;
        Setup(type);
    }

    /// <summary>Constructor for container values, like Array, Hash.</summary>
    public Value(ValueType containerType, ValueType subtype, uint step=0)
    {
        if (!containerType.IsContainer())
            throw new ArgumentException("First type must be a container type.");

        this.Handle = Marshal.AllocHGlobal(eina_value_sizeof());
        this.Ownership = ValueOwnership.Managed;

        Setup(containerType, subtype, step);
    }

    /// <summary>Constructor to build value from Values_Natives passed by value from C
    public Value(Value_Native value)
    {
        this.Handle = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(Value_Native)));
        try {
            Marshal.StructureToPtr(value, this.Handle, false);
        } catch {
            Marshal.FreeHGlobal(this.Handle);
            throw;
        }
        this.Ownership = ValueOwnership.Managed;
    }

    /// <summary>Creates an Value instance from a given array description.</summary>
    private static Value FromArrayDesc(eina.EinaNative.Value_Array arrayDesc)
    {
        Value value = new Value();
        value.Setup(ValueType.Array, ValueType.String); // Placeholder values to be overwritten by the following pset call.

        eina_value_pset_wrapper(value.Handle, ref arrayDesc);
        return value;
    }

    /// <summary>Creates an Value instance from a given array description.</summary>
    private static Value FromListDesc(eina.EinaNative.Value_List listDesc)
    {
        Value value = new Value();
        value.Setup(ValueType.List, ValueType.String); // Placeholder values to be overwritten by the following pset call.

        eina_value_pset_wrapper(value.Handle, ref listDesc);
        return value;
    }

    /// <summary>Releases the ownership of the underlying value to C.</summary>
    public void ReleaseOwnership()
    {
        this.Ownership = ValueOwnership.Unmanaged;
    }

    /// <summary>Takes the ownership of the underlying value to the Managed runtime.</summary>
    public void TakeOwnership()
    {
        this.Ownership = ValueOwnership.Managed;
    }

    /// <summary>Public method to explicitly free the wrapped eina value.</summary>
    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    /// <summary>Actually free the wrapped eina value. Can be called from Dispose() or through the GC.</summary>
    protected virtual void Dispose(bool disposing)
    {
        if (this.Ownership == ValueOwnership.Unmanaged) {
            Disposed = true;
            return;
        }

        if (!Disposed && (Handle != IntPtr.Zero)) {
            if (!Flushed)
                eina_value_flush_wrapper(this.Handle);

            Marshal.FreeHGlobal(this.Handle);
        }
        Disposed = true;
    }

    /// <summary>Finalizer to be called from the Garbage Collector.</summary>
    ~Value()
    {
        Dispose(false);
    }

    /// <summary>Returns the native handle wrapped by this object.</summary>
    public IntPtr NativeHandle()
    {
        if (Disposed)
            throw new ObjectDisposedException(base.GetType().Name);
        return this.Handle;
    }

    /// <summary>Converts this storage to type 'type'</summary>
    public bool Setup(ValueType type)
    {
        if (Disposed)
            throw new ObjectDisposedException(base.GetType().Name);

        if (type.IsContainer())
            throw new ArgumentException("To setup a container you must provide a subtype.");

        bool ret = eina_value_setup_wrapper(this.Handle, ValueTypeBridge.GetNative(type));
        if (ret)
            Flushed = false;
        return ret;
    }

    public bool Setup(ValueType containerType, ValueType subtype, uint step=0) {
        IntPtr native_subtype = ValueTypeBridge.GetNative(subtype);
        bool ret = false;
        switch (containerType) {
            case ValueType.Array:
                ret = eina_value_array_setup_wrapper(this.Handle, native_subtype, step);
                break;
            case ValueType.List:
                ret = eina_value_list_setup_wrapper(this.Handle, native_subtype);
                break;
        }

        if (ret)
            Flushed = false;

        return ret;
    }

    private void SanityChecks()
    {
        if (Disposed)
            throw new ObjectDisposedException(GetType().Name);
        if (Flushed)
            throw new ValueFlushedException("Trying to use value that has been flushed. Setup it again.");
    }

    private void ContainerSanityChecks(int targetIndex=-1)
    {
        SanityChecks();
        uint size = 0;
        ValueType type = GetValueType();

        if (!type.IsContainer())
                throw new InvalidValueTypeException("Value type must be a container");

        if (targetIndex == -1)  // Some methods (e.g. append) don't care about size
            return;

        switch (type) {
            case ValueType.Array:
                size = eina_value_array_count_wrapper(this.Handle);
                break;
            case ValueType.List:
                size = eina_value_list_count_wrapper(this.Handle);
                break;
        }

        if (targetIndex >= size)
                throw new System.ArgumentOutOfRangeException(
                        $"Index {targetIndex} is larger than max array index {size-1}");
    }

    private void OptionalSanityChecks()
    {
        SanityChecks();
        ValueType type = GetValueType();

        if (!type.IsOptional())
            throw new InvalidValueTypeException("Value is not an Optional one");
    }

    /// <summary>Releases the memory stored by this value. It can be reused by calling setup again.
    /// </summary>
    public void Flush()
    {
        if (Disposed)
            throw new ObjectDisposedException(GetType().Name);
        eina_value_flush_wrapper(this.Handle);
        Flushed = true;
    }

    /// <summary>Get a Value_Native struct with the *value* pointed by this eina.Value.</summary>
    public Value_Native GetNative()
    {
        Value_Native value = (Value_Native)Marshal.PtrToStructure(this.Handle, typeof(Value_Native));
        return value;
    }

    /// <summary>Stores the given uint value.</summary>
    public bool Set(uint value)
    {
        SanityChecks();

        if (this.Optional)
            return eina_value_optional_pset(this.Handle,
                                            ValueTypeBridge.GetNative(ValueType.UInt32),
                                            ref value);

        if (!GetValueType().IsNumeric())
            throw (new ArgumentException(
                        "Trying to set numeric value on a non-numeric eina.Value"));
        return eina_value_set_wrapper(this.Handle, value);
    }

    /// <summary>Stores the given int value.</summary>
    public bool Set(int value)
    {
        SanityChecks();

        if (this.Optional)
            return eina_value_optional_pset(this.Handle,
                                            ValueTypeBridge.GetNative(ValueType.Int32),
                                            ref value);

        if (!GetValueType().IsNumeric())
            throw (new ArgumentException(
                        "Trying to set numeric value on a non-numeric eina.Value"));
        return eina_value_set_wrapper(this.Handle, value);
    }

    /// <summary>Stores the given string value.</summary>
    public bool Set(string value)
    {
        SanityChecks();

        if (this.Optional)
            return eina_value_optional_pset(this.Handle,
                                            ValueTypeBridge.GetNative(ValueType.String),
                                            ref value);

        if (!GetValueType().IsString())
            throw (new ArgumentException(
                        "Trying to set non-string value on a string eina.Value"));
        // No need to worry about ownership as eina_value_set will copy the passed string.
        return eina_value_set_wrapper(this.Handle, value);
    }

    /// <summary>Stores the given value into this value. The target value must be an optional.</summary>
    public bool Set(Value value)
    {
        OptionalSanityChecks();
        ValueType subtype = value.GetValueType();

        IntPtr ptr_val = IntPtr.Zero;
        IntPtr native_type = ValueTypeBridge.GetNative(subtype);

        switch (subtype) {
            case ValueType.Array:
            case ValueType.List:
                // PSet on Container types require an Eina_Value_<Container>, which is the structure
                // that contains subtype, etc.
                if (!eina_value_get_wrapper(value.Handle, out ptr_val))
                    return false;
                break;
            default:
                throw new InvalidValueTypeException("Only containers can be passed as raw eina.Values");
        }
        return eina_value_optional_pset(this.Handle, native_type, ref ptr_val);
    }

    /// <summary>Gets the currently stored value as an int.</summary>
    public bool Get(out int value)
    {
        SanityChecks();
        if (this.Optional)
            return eina_value_optional_pget(this.Handle, out value);
        else
            return eina_value_get_wrapper(this.Handle, out value);
    }

    /// <summary>Gets the currently stored value as an uint.</summary>
    public bool Get(out uint value)
    {
        SanityChecks();
        if (this.Optional)
            return eina_value_optional_pget(this.Handle, out value);
        else
            return eina_value_get_wrapper(this.Handle, out value);
    }

    /// <summary>Gets the currently stored value as a string.</summary>
    public bool Get(out string value)
    {
        SanityChecks();
        IntPtr output = IntPtr.Zero;
        if (this.Optional) {
            if (!eina_value_optional_pget(this.Handle, out output)) {
                value = String.Empty;
                return false;
            }
        } else if (!eina_value_get_wrapper(this.Handle, out output)) {
            value = String.Empty;
            return false;
        }
        value = Marshal.PtrToStringAuto(output);
        return true;
    }

    /// <summary>Gets the currently stored value as an complex (e.g. container) eina.Value.</summary>
    public bool Get(out Value value)
    {
        SanityChecks();
        value = null;

        if (!this.Optional)
            throw new InvalidValueTypeException("Values can only be retreived");

        IntPtr nativeType = eina_value_optional_type_get_wrapper(this.Handle);
        ValueType managedType = ValueTypeBridge.GetManaged(nativeType);

        switch (managedType) {
            case ValueType.Array:
                eina.EinaNative.Value_Array array_desc;

                if (!eina_value_optional_pget(this.Handle, out array_desc))
                    return false;
                value = Value.FromArrayDesc(array_desc);
                break;
            case ValueType.List:
                eina.EinaNative.Value_List list_desc;

                if (!eina_value_optional_pget(this.Handle, out list_desc))
                    return false;
                value = Value.FromListDesc(list_desc);
                break;
        }

        return true;
    }

    /// <summary>Gets the 'Type' this value is currently configured to store.</summary>
    public ValueType GetValueType()
    {
        if (Disposed)
            throw new ObjectDisposedException(base.GetType().Name);
        IntPtr native_type = eina_value_type_get_wrapper(this.Handle);
        return ValueTypeBridge.GetManaged(native_type);
    }

    /// <summary>Converts the value on this storage to the type of 'target' and stores
    /// the result in 'target'.</summary>
    public bool ConvertTo(Value target)
    {
        if (target == null)
            return false;

        SanityChecks();

        return eina_value_convert(this.Handle, target.Handle);
    }

    /// <summary>Compare two eina values.</summary>
    public int CompareTo(Value other)
    {
        if (other == null)
            return 1;
        SanityChecks();
        other.SanityChecks();
        return eina_value_compare_wrapper(this.Handle, other.Handle);
    }

    public int Compare(Value other)
    {
        return this.CompareTo(other);
    }

    public override bool Equals(object obj)
    {
        if (obj == null)
            return false;

        Value v = obj as Value;
        if (v == null)
            return false;

        return this.Equals(v);
    }

    public bool Equals(Value other)
    {
        try {
            return this.CompareTo(other) == 0;
        } catch (ObjectDisposedException) {
            return false;
        } catch (ValueFlushedException) {
            return false;
        }
    }

    public override int GetHashCode()
    {
        return this.Handle.ToInt32();
    }

    public static bool operator==(Value x, Value y)
    {
        if (object.ReferenceEquals(x, null))
            return object.ReferenceEquals(y, null);

        return x.Equals(y);
    }

    public static bool operator!=(Value x, Value y)
    {
        if (object.ReferenceEquals(x, null))
            return !object.ReferenceEquals(y, null);
        return !x.Equals(y);
    }

    public static bool operator>(Value x, Value y)
    {
        if (object.ReferenceEquals(x, null) || object.ReferenceEquals(y, null))
            return false;
        return x.CompareTo(y) > 0;
    }

    public static bool operator<(Value x, Value y)
    {
        if (object.ReferenceEquals(x, null) || object.ReferenceEquals(y, null))
            return false;
        return x.CompareTo(y) < 0;
    }


    /// <summary>Converts value to string.</summary>
    public override String ToString()
    {
        SanityChecks();
        return eina_value_to_string(this.Handle);
    }

    /// <summary>Empties an optional eina.Value, freeing what was previously contained.</summary>
    public bool Reset()
    {
        OptionalSanityChecks();
        return eina_value_optional_reset(this.Handle);
    }

    // Container methods methods
    public int Count() {
        ContainerSanityChecks();
        switch(GetValueType()) {
            case ValueType.Array:
                return (int)eina_value_array_count_wrapper(this.Handle);
            case ValueType.List:
                return (int)eina_value_list_count_wrapper(this.Handle);
        }
        return -1;
    }
    public bool Append(object o) {
        ContainerSanityChecks();
        using (DisposableIntPtr marshalled_value = MarshalValue(o)) {
            switch (GetValueType()) {
                case ValueType.Array:
                    return eina_value_array_append_wrapper(this.Handle, marshalled_value.Handle);
                case ValueType.List:
                    return eina_value_list_append_wrapper(this.Handle, marshalled_value.Handle);
            }
        }
        return false;
    }

    public object this[int i]
    {
        get {
            ContainerSanityChecks(i);

            IntPtr output = IntPtr.Zero;
            switch (GetValueType()) {
                case ValueType.Array:
                    if (!eina_value_array_get_wrapper(this.Handle, i, out output))
                        return null;
                    break;
                case ValueType.List:
                    if (!eina_value_list_get_wrapper(this.Handle, i, out output))
                        return null;
                    break;
            }
            return UnMarshalPtr(output);
        }
        set {
            ContainerSanityChecks(i);

            switch (GetValueType()) {
                case ValueType.Array:
                    ArraySet(i, value);
                    break;
                case ValueType.List:
                    ListSet(i, value);
                    break;
            }
        }
    }

    private void ArraySet(int i, object value) {
        using (DisposableIntPtr marshalled_value = MarshalValue(value))
        {
            if (!eina_value_array_set_wrapper(this.Handle, i,
                                              marshalled_value.Handle)) {
                throw new SetItemFailedException($"Failed to set item at index {i}");
            }
        }
    }

    private void ListSet(int i, object value) {
        using (DisposableIntPtr marshalled_value = MarshalValue(value))
        {
            if (!eina_value_list_set_wrapper(this.Handle, i,
                                             marshalled_value.Handle)) {
                throw new SetItemFailedException($"Failed to set item at index {i}");
            }
        }
    }

    public ValueType GetValueSubType()
    {
        ContainerSanityChecks();

        IntPtr native_subtype = IntPtr.Zero;

        switch (GetValueType()) {
            case ValueType.Array:
                native_subtype = eina_value_array_subtype_get_wrapper(this.Handle);
                break;
            case ValueType.List:
                native_subtype = eina_value_list_subtype_get_wrapper(this.Handle);
                break;
        }
        return ValueTypeBridge.GetManaged(native_subtype);
    }

    private DisposableIntPtr MarshalValue(object value)
    {
        IntPtr ret = IntPtr.Zero;
        bool shouldFree = false;
        switch(GetValueSubType()) {
            case ValueType.Int32:
                {
                    int x = Convert.ToInt32(value);
                    ret = new IntPtr(x);
                }
                break;
            case ValueType.UInt32:
                {
                    uint x = Convert.ToUInt32(value);
                    ret = new IntPtr((int)x);
                }
                break;
            case ValueType.String:
                {
                    string x = value as string;
                    if (x == null)
                        ret = IntPtr.Zero;
                    else {
                        ret = Marshal.StringToHGlobalAnsi(x);
                        shouldFree = true;
                    }
                }
                break;
        }

        return new DisposableIntPtr(ret, shouldFree);
    }

    private object UnMarshalPtr(IntPtr data)
    {
        switch(GetValueSubType()) {
            case ValueType.Int32:
                return Convert.ToInt32(data.ToInt32());
            case ValueType.UInt32:
                return Convert.ToUInt32(data.ToInt32());
            case ValueType.String:
                return Marshal.PtrToStringAuto(data);
            default:
                return null;
        }
    }

}

/// <summary> Custom marshaler to convert value pointers to managed values and back,
/// without changing ownership.</summary>
public class ValueMarshaler : ICustomMarshaler {

    /// <summary>Creates a managed value from a C pointer, whitout taking ownership of it.</summary>
    public object MarshalNativeToManaged(IntPtr pNativeData) {
        return new Value(pNativeData, ValueOwnership.Unmanaged);
    }

    /// <summary>Retrieves the C pointer from a given managed value,
    /// keeping the managed ownership.</summary>
    public IntPtr MarshalManagedToNative(object managedObj) {
        try {
            Value v = (Value)managedObj;
            return v.Handle;
        } catch (InvalidCastException) {
            return IntPtr.Zero;
        }
    }

    public void CleanUpNativeData(IntPtr pNativeData) {
    }

    public void CleanUpManagedData(object managedObj) {
    }

    public int GetNativeDataSize() {
        return -1;
    }

    public static ICustomMarshaler GetInstance(string cookie) {
        if (marshaler == null) {
            marshaler = new ValueMarshaler();
        }
        return marshaler;
    }
    static private ValueMarshaler marshaler;
}

/// <summary> Custom marshaler to convert value pointers to managed values and back,
/// also transferring the ownership to the other side.</summary>
public class ValueMarshalerOwn : ICustomMarshaler {
    /// <summary>Creates a managed value from a C pointer, taking the ownership.</summary>
    public object MarshalNativeToManaged(IntPtr pNativeData) {
        return new Value(pNativeData, ValueOwnership.Managed);
    }

    /// <summary>Retrieves the C pointer from a given managed value,
    /// transferring the ownership to the unmanaged side, which should release it
    /// when not needed. </summary>
    public IntPtr MarshalManagedToNative(object managedObj) {
        try {
            Value v = (Value)managedObj;
            v.ReleaseOwnership();
            return v.Handle;
        } catch (InvalidCastException) {
            return IntPtr.Zero;
        }
    }

    public void CleanUpNativeData(IntPtr pNativeData) {
    }

    public void CleanUpManagedData(object managedObj) {
    }

    public int GetNativeDataSize() {
        return -1;
    }

    public static ICustomMarshaler GetInstance(string cookie) {
        if (marshaler == null) {
            marshaler = new ValueMarshalerOwn();
        }
        return marshaler;
    }
    static private ValueMarshalerOwn marshaler;
}
}
