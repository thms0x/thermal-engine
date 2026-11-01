
#include <iostream>
#include <exception>
#include <filesystem>

#include "NativeAotHost.hpp"
#include "ManagedController.hpp"

int main(int argc, char* argv[])
{
    try {
        if (argc != 2) {
            std::cerr
                << "Usage: native_aot_test <library-path>\n";
            return 1;
        }

        NativeAotHost host{std::filesystem::path{argv[1]}};

        auto first = host.createController(50.0);
        auto second = host.createController(80.0);

        constexpr double temperature = 60.0;

        std::cout << "Temperature: "
                  << temperature << " C\n\n";

        std::cout << "Controller 1 (target 50 C): "
                  << first->calculatePower(temperature)
                  << " W\n";

        std::cout << "Controller 2 (target 80 C): "
                  << second->calculatePower(temperature)
                  << " W\n";

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
