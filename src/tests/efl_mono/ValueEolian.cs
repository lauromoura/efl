#define CODE_ANALYSIS

// Disable warning about missing docs when generating docs
#pragma warning disable 1591

using System;

namespace TestSuite {


public static class TestEinaValueEolian {
    public static void TestEolianEinaValueInReturn()
    {
        test.Testing obj = new test.TestingConcrete();

        using (eina.Value v = new eina.Value(eina.ValueType.Int32)) {
            v.Set(42);
            obj.SetValuePtr(v);
            Test.AssertEquals(eina.ValueOwnership.Managed, v.Ownership);

            eina.Value v_received = obj.GetValuePtrOwn();
            Test.AssertEquals(eina.ValueOwnership.Managed, v_received.Ownership);
            Test.AssertEquals(v, v_received);
            v_received.Dispose();
        }
    }

    public static void TestEolianEinaValueInOwn()
    {
        test.Testing obj = new test.TestingConcrete();

        using (eina.Value v = new eina.Value(eina.ValueType.Int32)) {
            v.Set(2001);
            Test.AssertEquals(eina.ValueOwnership.Managed, v.Ownership);

            obj.SetValuePtrOwn(v);
            Test.AssertEquals(eina.ValueOwnership.Unmanaged, v.Ownership);

            eina.Value v_received = obj.GetValuePtr();
            Test.AssertEquals(eina.ValueOwnership.Unmanaged, v_received.Ownership);

            Test.AssertEquals(v, v_received);

            obj.ClearValue();
        }
    }

    public static void TestEolianEinaValueOut()
    {
        test.Testing obj = new test.TestingConcrete();

        using (eina.Value v = new eina.Value(eina.ValueType.String)) {
            eina.Value v_out = null;

            v.Set("hello!");
            obj.SetValuePtr(v);
            obj.OutValuePtr(out v_out);

            Test.AssertEquals(v, v_out);
            Test.AssertEquals(eina.ValueOwnership.Unmanaged, v_out.Ownership);
        }
    }

    public static void TestEolianEinaValueOutOwn()
    {
        test.Testing obj = new test.TestingConcrete();

        using (eina.Value v = new eina.Value(eina.ValueType.String)) {
            eina.Value v_out = null;

            v.Set("hello!");
            obj.SetValuePtr(v);
            obj.OutValuePtrOwn(out v_out);

            Test.AssertEquals(v, v_out);
            Test.AssertEquals(eina.ValueOwnership.Managed, v_out.Ownership);
        }
    }

    public static void TestEolianEinaValueInReturnByValue()
    {
        test.Testing obj = new test.TestingConcrete();

        using (eina.Value v = new eina.Value(eina.ValueType.Int32)) {
            v.Set(42);
            obj.SetValue(v);
            Test.AssertEquals(eina.ValueOwnership.Managed, v.Ownership);

            // Using get_value_ptr while get_value() is not supported.
            eina.Value v_received = obj.GetValuePtrOwn();
            Test.AssertEquals(eina.ValueOwnership.Managed, v_received.Ownership);
            Test.AssertEquals(v, v_received);
            v_received.Dispose();
        }
    }

    public static void TestEolianEinaValueOutByValue()
    {
        test.Testing obj = new test.TestingConcrete();

        using (eina.Value v = new eina.Value(eina.ValueType.String)) {
            eina.Value v_out = null;

            v.Set("hello!");
            obj.SetValue(v);
            obj.OutValue(out v_out);

            Test.AssertEquals(v, v_out);
            Test.AssertEquals(eina.ValueOwnership.Managed, v_out.Ownership);
        }
    }
}
#pragma warning restore 1591
}

