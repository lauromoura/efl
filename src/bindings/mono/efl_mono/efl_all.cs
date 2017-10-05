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

    public const string CustomExports = "libeflcustomexportsmono-0";
#else
    public const string Efl = "efl";
    public const string Ecore = "ecore";
    public const string Eina = "eina";
    public const string Eo = "eo";
    public const string Evas = "evas";
    public const string Evil = "dl"; // Used for dlsym, dlopen and dlerror
    public const string EcoreEvas = "ecore_evas";

    public const string CustomExports = "eflcustomexportsmono";
#endif
}

static class UnsafeNativeMethods {
    [DllImport(efl.Libs.Ecore)] public static extern void ecore_init();
    [DllImport(efl.Libs.Ecore)] public static extern void ecore_shutdown();
    [DllImport(efl.Libs.Evas)] public static extern void evas_init();
    [DllImport(efl.Libs.Evas)] public static extern void evas_shutdown();
}

public static class All {
    public static void Init() {
        eina.Config.Init();
        efl.eo.Config.Init();
        ecore_init();
        evas_init();
    }

    /// <summary>Shutdowns all EFL subsystems.</summary>
    public static void Shutdown() {
        // Try to cleanup everything before actually shutting down.
        System.GC.Collect();
        System.GC.WaitForPendingFinalizers();

        evas_shutdown();
        ecore_shutdown();
        efl.eo.Config.Shutdown();
        eina.Config.Shutdown();
    }

    public static int Main() {
        return 0;
    }
}

}
