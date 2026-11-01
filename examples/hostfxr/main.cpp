#include <iostream>

#include "ThermalSystem.hpp"
#include "DotNetHost.hpp"

int main()
{
    try {
        const std::filesystem::path folder =
            "managed/ThermalController/bin/Release/net10.0";

        DotNetHost host{
            folder / "ThermalController.runtimeconfig.json",
            folder / "ThermalController.dll"
        };

        auto controller = host.createController(50.0);

        ThermalSystem system{20.0, 1000.0, 20.0, 5.0};

        for (int i = 0; i < 100; ++i) {
            double power = controller->calculatePower(
                system.getTemperature()
            );

            system.step(power, 1.0);
        }

        std::cout << "Final temperature: "
                  << system.getTemperature() << " C\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}