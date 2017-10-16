using System;
using System.Runtime.InteropServices;

using static efl.UnsafeNativeMethods;

namespace efl {

/// <summary>
/// Define the name of the libraries to be passed to DllImport statements.
/// </summary>
public class Libs {
#if WIN32
    // TODO: Hardcoded -VERSION version suffixes
    public const string Efl = "libefl-1";
    public const string Ecore = "libecore-1";
    public const string Eina = "libeina-1";
    public const string Eo = "libeo-1";
    public const string Evas = "libevas-1";
    public const string Evil = "libevil-1";
    public const string EcoreEvas = "libecore_evas-1";
    public const string Edje = "libedje-1";
    public const string Elementary = "libelementary-1";
    public const string Eldbus = "libeldbus-1";

    public const string CustomExports = "libeflcustomexportsmono-0";
#else
    public const string Efl = "efl";
    public const string Ecore = "ecore";
    public const string Eina = "eina";
    public const string Eo = "eo";
    public const string Evas = "evas";
    public const string Evil = "dl"; // Used for dlsym, dlopen and dlerror
    public const string EcoreEvas = "ecore_evas";
    public const string Edje = "edje";
    public const string Elementary = "elementary";
    public const string Eldbus = "eldbus";

    public const string CustomExports = "eflcustomexportsmono";
#endif
}

static class UnsafeNativeMethods {
    [DllImport(efl.Libs.Ecore)] public static extern void ecore_init();
    [DllImport(efl.Libs.Ecore)] public static extern void ecore_shutdown();
    [DllImport(efl.Libs.Evas)] public static extern void evas_init();
    [DllImport(efl.Libs.Evas)] public static extern void evas_shutdown();
    [DllImport(efl.Libs.Elementary)] public static extern int elm_init(int argc, IntPtr argv);
    [DllImport(efl.Libs.Elementary)] public static extern void elm_shutdown();
    [DllImport(efl.Libs.Elementary)] public static extern void elm_run();
    [DllImport(efl.Libs.Elementary)] public static extern void elm_exit();
}

public static class All {
    public static void Init() {
        eina.Config.Init();
        efl.eo.Config.Init();
        efl.ui.Config.Init();
        ecore_init();
        evas_init();
        eldbus.Config.Init();
    }

    /// <summary>Shutdowns all EFL subsystems.</summary>
    public static void Shutdown() {
        // Try to cleanup everything before actually shutting down.
        System.GC.Collect();
        System.GC.WaitForPendingFinalizers();

        eldbus.Config.Shutdown();
        evas_shutdown();
        ecore_shutdown();
        efl.ui.Config.Shutdown();
        efl.eo.Config.Shutdown();
        eina.Config.Shutdown();
    }
}

// Placeholder. Will move to elm_config.cs later
namespace ui {

public static class Config {
    public static void Init() {
        // TODO Support elm command line arguments
        elm_init(0, IntPtr.Zero);
    }
    public static void Shutdown() {
        elm_shutdown();
    }

    public static void Run() {
        elm_run();
    }

    public static void Exit() {
        elm_exit();
    }
}

}

}
