#pragma once

class ThermalSystem {
    public:
        explicit ThermalSystem(double initialTemperature);

        [[nodiscard]] double getTemperature() const;

    private: 
        double temperature;
};