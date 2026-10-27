namespace ThermalController;

public class ThermalController
{
    private readonly double targetTemperature;

    public ThermalController(double targetTemperature)
    {
        this.targetTemperature = targetTemperature;
    }
    public double CalculatePower(double temperature){
        return temperature < targetTemperature ? 100.0 : 0.0;
    }
}
