using System;
using System.Runtime.CompilerServices;

class TestMain
{
    static int WIDTH = 320;
    static int HEIGHT = 240;

    evas.Image image;


    static string ImagePath([CallerFilePath] string folder="")
    {
        return System.IO.Path.GetDirectoryName(folder);
    }

    public TestMain(evas.Image image)
    {
        this.image = image;
    }

    static void Main(string[] args)
    {
        efl.All.Init();

        efl.Loop loop = new efl.LoopConcrete();

        EcoreEvas ecore_evas = new EcoreEvas();
        eina.Size2D size = new eina.Size2D();
        
        efl.canvas.Object canvas = ecore_evas.canvas;
        canvas.visible_set(true);

        efl.Object parent = canvas.parent_get();
        System.Diagnostics.Debug.Assert(parent.raw_handle != IntPtr.Zero);
        
        efl.canvas.Rectangle bg = new efl.canvas.RectangleConcrete(canvas);
        bg.color_set(255, 255, 255, 255);
        size.W = WIDTH;
        size.H = HEIGHT;
        bg.size_set(size);
        bg.visible_set(true);

        string valid_path = args[0];
        evas.Image image = new evas.ImageConcrete(canvas);
        image.file_set(valid_path, null);

        /* FIXME evas-image uses error handling code from
         * evas_object_image_load_error_get, which seems to be not available
         * efl.image.load.State state = image.load_error_get(); */

        // FIXME missing move
        eina.Rect rect = new eina.Rect();

        rect.X = 0;
        rect.Y = 0;
        rect.W = WIDTH / 2;
        rect.H = HEIGHT / 2;
        image.fill_set(rect);

        size.W = WIDTH / 2;
        size.H = HEIGHT / 2;
        image.size_set(size);
        image.visible_set(true);

        rect = image.fill_get();
        rect.Y -= 50;
        rect.W += 100;
        image.fill_set(rect);

        TestMain listener = new TestMain(image);

        // TODO handle key events in order to alter the image like the C
        // example. Meanwhile, just set some w fill
        /* EventListener callback = new EventListener(); */

        /* bg.key_focus_set(true); */
        /* bg.event_callback_priority_add(evas.Callback_Type.Key_down, */ 
        /*         efl.Callback_Priority.Default, */
        /*         callback, null); */

        loop.begin();

        efl.All.Shutdown();
    }

    public void on_key_down(object sender, EventArgs e)
    {
        Console.WriteLine("on_key_down called");
    }
}


