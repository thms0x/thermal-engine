
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace ThermalController;

public static class ThermalControllerApi
{
    // --------------------------------------------------
    // Create a managed controller instance
    // --------------------------------------------------

    [UnmanagedCallersOnly(
        EntryPoint = "thermal_create",
        CallConvs = new[] { typeof(CallConvCdecl) }
    )]
    public static nint Create(double targetTemperature)
    {
        try
        {
            var controller = new ThermalController(targetTemperature);
            var handle = GCHandle.Alloc(controller);

            return GCHandle.ToIntPtr(handle);
        }
        catch (Exception)
        {
            return 0;
        }
    }

    // --------------------------------------------------
    // Call the managed calculation
    // --------------------------------------------------

    [UnmanagedCallersOnly(
        EntryPoint = "thermal_calculate",
        CallConvs = new[] { typeof(CallConvCdecl) }
    )]
    public static double Calculate(nint handle, double temperature)
    {
        try
        {
            var gcHandle = GCHandle.FromIntPtr(handle);

            var controller =
                (ThermalController)gcHandle.Target!;

            return controller.CalculatePower(temperature);
        }
        catch (Exception)
        {
            // Temporary PoC convention: NaN signals failure.
            return double.NaN;
        }
    }

    // --------------------------------------------------
    // Release the managed controller handle
    // --------------------------------------------------

    [UnmanagedCallersOnly(
        EntryPoint = "thermal_destroy",
        CallConvs = new[] { typeof(CallConvCdecl) }
    )]
    public static void Destroy(nint handle)
    {
        if (handle == 0)
            return;

        try
        {
            GCHandle.FromIntPtr(handle).Free();
        }
        catch (Exception)
        {
            // TODO: implement a error channel, for example return int
        }
    }
}
