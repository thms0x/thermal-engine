#include <iostream>
#include "ThermalSystem.hpp"

int main(){
 ThermalSystem thermalSystem(80.0, 1000.0, 20.0, 5.0);
    double currentTemperature = thermalSystem.getTemperature();
    std::cout << "Current Temperature: " << currentTemperature << "°C" << std::endl;
    thermalSystem.step(300, 10);
    currentTemperature = thermalSystem.getTemperature();
    std::cout << "Current Temperature: " << currentTemperature << "°C" << std::endl;

    try{
    ThermalSystem system{20.0, 0.0, 20.0, 5.0};
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
}