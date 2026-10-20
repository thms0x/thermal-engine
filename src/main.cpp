#include <iostream>
#include "ThermalSystem.hpp"

int main(){
 ThermalSystem thermalSystem(25.0);
    double currentTemperature = thermalSystem.getTemperature();
    std::cout << "Current Temperature: " << currentTemperature << "°C" << std::endl;
}