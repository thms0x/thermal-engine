
#include <exception>
#include <filesystem>
#include <iostream>

#include "NativeAotHost.hpp"
#include "ManagedController.hpp"
#include "ThermalSystem.hpp"

int main(int argc, char* argv[])
{
    try {
        if (argc != 2) {
            std::cerr
                << "Usage: thermal_aot_app <library-path>\n";

            return 1;
        }

        const std::filesystem::path libraryPath{argv[1]};

        NativeAotHost host{libraryPath};


        constexpr double targetTemperature = 50.0;

        auto controller = host.createController(targetTemperature);

        constexpr double initialTemperature = 20.0;
        constexpr double heatCapacity = 1000.0;
        constexpr double ambientTemperature = 20.0;
        constexpr double heatLossCoefficient = 5.0;

        ThermalSystem system{
            initialTemperature,
            heatCapacity,
            ambientTemperature,
            heatLossCoefficient
        };

        constexpr int steps = 100;
        constexpr double dt = 1.0;

        std::cout << "Starting Native AOT simulation...\n";
        std::cout << "Initial temperature: "
                  << system.getTemperature() << " C\n";

        for (int i = 0; i < steps; ++i) {
            // C++ calls C# through Native AOT exports
            const double power = controller->calculatePower(
                system.getTemperature()
            );

            // Native C++ performs the thermal calculation
            system.step(power, dt);
        }


        std::cout << "Final temperature: "
                  << system.getTemperature() << " C\n";

        std::cout << "Target temperature: "
                  << targetTemperature << " C\n";

        std::cout << "Simulation steps: "
                  << steps << '\n';
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
