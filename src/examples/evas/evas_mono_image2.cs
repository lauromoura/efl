using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

class TestMain
{
    static int WIDTH = 320;
    static int HEIGHT = 240;


    static void Main(string[] args)
    {
        efl.All.Init();

        efl.Loop loop = new efl.LoopConcrete();

        EcoreEvas ecore_evas = new EcoreEvas();

        eina.Size2D size = new eina.Size2D();
        eina.Position2D pos = new eina.Position2D();
        
        efl.canvas.Object canvas = ecore_evas.canvas;
        canvas.visible_set(true);

        efl.canvas.Rectangle bg = new efl.canvas.RectangleConcrete(canvas);
        bg.color_set(255, 255, 255, 255);
        pos.X = 0;
        pos.Y = 0;
        bg.position_set(pos);
        size.W = WIDTH;
        size.H = HEIGHT;
        bg.size_set(size);
        bg.visible_set(true);

        string path = args[0];
        evas.Image logo = new evas.ImageConcrete(canvas);
        logo.fill_auto_set(true);

        // TODO add preloaded support (depends on events)

        logo.file_set(path, null);
        size.W = WIDTH / 2;
        size.H = HEIGHT / 2;
        logo.size_set(size);

        // TODO add a bunch of key/mouse handlers

        logo.visible_set(true);

        int[] pixels = new int[(WIDTH/4) * (HEIGHT / 4)];
        System.Random generator = new System.Random();
        for (int i = 0; i < pixels.Length; i++) {
            pixels[i] = generator.Next();
        }

        evas.Image noise_img = new evas.ImageConcrete(canvas);
        size.W = WIDTH / 4;
        size.H = HEIGHT / 4;
        noise_img.size_set(size);
        // FIXME Add a way to set the pixels.
        // noise_img.data_set(pixels);
        noise_img.fill_auto_set(true);
        pos.X = WIDTH * 5 / 8;
        pos.Y = HEIGHT / 8;
        noise_img.position_set(pos);
        noise_img.visible_set(true);
        Console.WriteLine("Creating noise image with sizez %d, %d", WIDTH/4, HEIGHT/4);

        efl.canvas.Proxy proxy_img = new efl.canvas.ProxyConcrete(canvas);
        proxy_img.source_set(noise_img);
        pos.X = WIDTH / 2;
        pos.Y = HEIGHT / 2;
        proxy_img.position_set(pos);
        size.W = WIDTH / 2;
        size.H = HEIGHT / 2;
        proxy_img.size_set(size);
        proxy_img.visible_set(true);

        loop.begin();
    }
}


