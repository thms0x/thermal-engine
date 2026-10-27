using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace ThermalController;

public class ThermalController
{
    // signal .NET that this function can be called by an unmanaged func pointer
    // must be static and only blittable params
    [UnmanagedCallersOnly(CallConvs = new[] {typeof(CallConvCdecl)})]
    public static double CalculatePower(double temperature){
        return temperature < 50.0 ? 100.0 : 0.0;
    }
}
