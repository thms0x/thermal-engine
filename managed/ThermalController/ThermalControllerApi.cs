using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace ThermalController;

public static class ThermalControllerApi
{
    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    public static nint Create(double targetTemperature)
    {
        var controller = new ThermalController(targetTemperature);
        var handle = GCHandle.Alloc(controller);
        return GCHandle.ToIntPtr(handle);
    }

    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    public static double Calculate(nint handle, double temperature)
    {
        var gcHandle = GCHandle.FromIntPtr(handle);
        var controller = (ThermalController)gcHandle.Target!;
        return controller.CalculatePower(temperature);
    }

    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    public static void Destroy(nint handle)
    {
        GCHandle.FromIntPtr(handle).Free();
    }
}