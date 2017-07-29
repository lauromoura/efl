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
            obj.set_value_ptr(v);
            Test.AssertEquals(eina.ValueOwnership.Managed, v.Ownership);

            eina.Value v_received = obj.get_value_ptr_own();
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

            obj.set_value_ptr_own(v);
            Test.AssertEquals(eina.ValueOwnership.Unmanaged, v.Ownership);

            eina.Value v_received = obj.get_value_ptr();
            Test.AssertEquals(eina.ValueOwnership.Unmanaged, v_received.Ownership);

            Test.AssertEquals(v, v_received);

            obj.clear_value();
        }
    }

    public static void TestEolianEinaValueOut()
    {
    }

    public static void TestEolianEinaValueOutOwn()
    {
    }

}
#pragma warning restore 1591
}
