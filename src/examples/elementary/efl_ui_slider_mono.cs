using System;

public class Example
{
    public static efl.ui.Button CreateButton(efl.Object parent,
                                             string text,
                                             int w, int h,
                                             EventHandler callback) {
        efl.ui.Button button = new efl.ui.ButtonConcrete(parent);
        button.text_set(text);
        button.size_set(w, h);

        button.CLICKED += callback;

        return button;
    }

    public static void Main() {
        efl.All.Init(efl.Components.Ui);

        efl.ui.Win win = new efl.ui.WinConcrete(null);
        win.text_set("Hello, C#!!");
        win.autohide_set(true);

        efl.ui.Box_Flow box = new efl.ui.Box_FlowConcrete(win);

        efl.ui.Button button = CreateButton(box, "Click to exit", 120, 30,
                (object sender, EventArgs e) => {
                    efl.ui.Config.Exit();
                });

        box.pack(button);

        efl.ui.Progressbar bar = new efl.ui.ProgressbarConcrete(box);
        bar.size_set(120, 30);

        efl.ui.Slider slider = new efl.ui.SliderConcrete(box);
        slider.size_set(120, 30);

        slider.CHANGED += (object sender, EventArgs e) => {
            bar.range_value_set(slider.range_value_get());
        };

        box.pack(bar);
        box.pack(slider);

        button.visible_set(true);
        box.visible_set(true);

        win.size_set(120, 90);
        win.visible_set(true);

        efl.ui.Config.Run();

        efl.All.Shutdown();
    }

}
