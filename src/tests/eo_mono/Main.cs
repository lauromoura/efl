using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Reflection;
using System.Linq;

public class MyBox : evas.BoxInherit
{
    public MyBox(efl.Object parent) : base(parent) {}

    [DllImport("evas")] static extern void evas_obj_box_layout_vertical(IntPtr obj, IntPtr data, IntPtr privdata);
    [DllImport("evas")] static extern void evas_obj_box_layout_horizontal(IntPtr obj, IntPtr data, IntPtr privdata);
    [DllImport("evas")] static extern void evas_object_box_layout_horizontal(IntPtr obj, IntPtr data, IntPtr privdata);
    [DllImport("evas")] static extern IntPtr evas_object_evas_get(IntPtr obj);
    [DllImport("evas")] static extern void evas_event_freeze(IntPtr obj);
    [DllImport("evas")] static extern void evas_event_thaw(IntPtr obj);

    override public void group_calculate()
    {
        IntPtr evas = evas_object_evas_get(raw_handle);
        evas_event_freeze(evas);
        Console.WriteLine("called group_calculate");
        layouting_set(true);
        evas_obj_box_layout_vertical(raw_handle, IntPtr.Zero, IntPtr.Zero);
        layouting_set(false);
        children_changed_set(false);
        evas_event_thaw(evas);
    }
}

namespace testsuite {

class Core
{
    public static void return_same_object()
    {
        test.Testing testing = new test.TestingConcrete();
        test.Testing o1 = testing.return_object();
        System.Diagnostics.Debug.Assert(o1.raw_handle != IntPtr.Zero);
        System.Diagnostics.Debug.Assert(o1.raw_handle == testing.raw_handle);
        test.Testing o2 = o1.return_object();
        System.Diagnostics.Debug.Assert(o2.raw_handle != IntPtr.Zero);
        System.Diagnostics.Debug.Assert(o2.raw_handle == o1.raw_handle);
    }
}

class Evas
{
    private static string ImagePath([CallerFilePath] string folder="")
    {
        return System.IO.Path.GetDirectoryName(folder);
    }

    public static void simple_layout()
    {
//         efl.Loop loop = new efl.LoopConcrete();

        EcoreEvas ecore_evas = new EcoreEvas();

        System.Console.WriteLine("Hello World");

        efl.canvas.Object canvas = ecore_evas.canvas;
        canvas.visible_set(true);

        efl.Object parent = canvas.parent_get();
        System.Diagnostics.Debug.Assert(parent.raw_handle != IntPtr.Zero);

        efl.canvas.Rectangle rect = new efl.canvas.RectangleConcrete(canvas);
        rect.color_set(255, 255, 255, 255);
        rect.size_set(640, 480);
        rect.visible_set(true);

        evas.Box box = new MyBox(canvas);
        rect.size_set(320, 240);
        box.visible_set(true);

        efl.canvas.Image image1 = new efl.canvas.ImageConcrete(canvas);
        image1.file_set(ImagePath() + "/../../examples/elementary/sphere_hunter/score.jpg", "");
        image1.hint_min_set(160, 240);
        image1.visible_set(true);

        efl.canvas.Image image2 = new efl.canvas.ImageConcrete(canvas);
        image2.file_set(ImagePath() + "/../../examples/evas/shooter/assets/images/bricks.jpg", "");
        image2.hint_min_set(160, 120);
        image2.visible_set(true);

        box.append(image1);
        box.append(image2);

//         loop.begin();
    }
}

}

class TestMain
{
    [DllImport("eo")] static extern void efl_object_init();
    [DllImport("ecore")] static extern void ecore_init();
    [DllImport("evas")] static extern void evas_init();

    static Type[] GetTestSuites()
    {
        return Assembly.GetExecutingAssembly().GetTypes().Where(t => String.Equals(t.Namespace, "testsuite", StringComparison.Ordinal)).ToArray();
    }

    static void Main(string[] args)
    {
        efl_object_init();
        ecore_init();
        evas_init();

        var suites = GetTestSuites();
        foreach(var suite in suites)
        {
            var testcases = suite.GetMethods(BindingFlags.Static | BindingFlags.Public);
            foreach(var testcase in testcases)
            {
                testcase.Invoke(null, null);
            }
        }
    }
}
