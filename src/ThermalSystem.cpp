#include "ThermalSystem.hpp"

ThermalSystem::ThermalSystem(double initialTemperature)
    : temperature(initialTemperature) {}

double ThermalSystem::getTemperature() const {
    return temperature;
}