#include <stdexcept>
#include <cmath>

#include "ThermalSystem.hpp"

ThermalSystem::ThermalSystem(double initialTemperature, double heatCapacity, double ambientTemperature, double heatLossCoefficient)
    : temperature_(initialTemperature), heatCapacity_(heatCapacity), ambientTemperature_(ambientTemperature), heatLossCoefficient_(heatLossCoefficient) {
        if(heatCapacity <= 0){
            throw std::invalid_argument("Heat capacity must be greater zero.");
        }
        if(heatLossCoefficient < 0){
            throw std::invalid_argument("Heat loss coefficient must be non-negative.");
        }
        if(!std::isfinite(initialTemperature) || !std::isfinite(heatCapacity) || !std::isfinite(ambientTemperature) || !std::isfinite(heatLossCoefficient)){
            throw std::invalid_argument("Function arguments must be finite.");
        }
    }

double ThermalSystem::getTemperature() const {
    return temperature_;
}

double ThermalSystem::step(double power, double dt)
{
    if (dt <= 0 || !std::isfinite(dt)) {
        throw std::invalid_argument("Time step must be positive and finite.");
    }
    if (!std::isfinite(power)) {
        throw std::invalid_argument("Power must be finite.");
    }

    temperature_ += ((power - heatLossCoefficient_ * (temperature_ - ambientTemperature_)) / heatCapacity_) * dt;

    return temperature_;
}
