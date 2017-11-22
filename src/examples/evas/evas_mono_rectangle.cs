using System;

class TestMain
{
    private static int[,] colors = new int[,] {
        {255, 0, 0},
        {0, 255, 0},
        {0, 0, 255}
    };

    static void Main(string[] args)
    {
        int color_index = 0;

        efl.All.Init();

        efl.Loop loop = new efl.LoopConcrete();
        EcoreEvas ecore_evas = new EcoreEvas();
        efl.canvas.Object canvas = ecore_evas.canvas;
        canvas.visible_set(true);

        efl.Object parent = canvas.parent_get();
        System.Diagnostics.Debug.Assert(parent.raw_handle != IntPtr.Zero);

        efl.canvas.Rectangle rect = new efl.canvas.RectangleConcrete(canvas);
        rect.color_set(colors[0, 0], colors[0, 1], colors[0, 2], 255);
        eina.Size2D size = new eina.Size2D();
        size.W = 640;
        size.H = 480;
        rect.size_set(size);
        rect.visible_set(true);

        canvas.KEY_DOWN += (object sender, efl.input.KEY_DOWN_Args e) => {
            color_index = (color_index + 1) % 3;
            Console.WriteLine("Key Down");
            Console.WriteLine("Got key obj at {0}", e.arg.raw_handle.ToString("X"));
            Console.WriteLine("Got key_get() == [{0}]", e.arg.key_get());
            rect.color_set(colors[color_index, 0],
                           colors[color_index, 1],
                           colors[color_index, 2], 255);
        };

        loop.begin();

        efl.All.Shutdown();
    }
}
