#pragma once

class ThermalSystem {
    public:
        explicit ThermalSystem(double initialTemperature, double heatCapacity, double ambientTemperature, double heatLossCoefficient);

        [[nodiscard]] double getTemperature() const;
        double step(double power, double dt);

    private: 
        double temperature_;
        double heatCapacity_;
        double ambientTemperature_;
        double heatLossCoefficient_;
};