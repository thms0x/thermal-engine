
#include <exception>
#include <iostream>

#include "ManagedController.hpp"
#include "ThermalSystem.hpp"

// --------------------------------------------------
// C++/CLI thermal simulation application
// --------------------------------------------------

// Windows-only reference implementation.
// Requires MSVC and a mixed-mode C++/CLI DLL.

int main()
{
    try {
        // --------------------------------------------------
        // Create managed C# controller via C++/CLI
        // --------------------------------------------------

        constexpr double targetTemperature = 50.0;

        ManagedController controller{targetTemperature};

        // --------------------------------------------------
        // Initialize native thermal system
        // --------------------------------------------------

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

        // --------------------------------------------------
        // Run thermal simulation
        // --------------------------------------------------

        constexpr int steps = 100;
        constexpr double dt = 1.0;

        std::cout << "Starting C++/CLI simulation...\n";
        std::cout << "Initial temperature: "
                  << system.getTemperature() << " C\n";

        for (int i = 0; i < steps; ++i) {
            // Calls the managed C# controller through C++/CLI
            const double power = controller.calculatePower(
                system.getTemperature()
            );

            // Native C++ performs the thermal calculation
            system.step(power, dt);
        }

        // --------------------------------------------------
        // Print simulation results
        // --------------------------------------------------

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
