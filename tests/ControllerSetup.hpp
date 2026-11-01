
#pragma once

#include <cstdlib>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>

#include "ManagedController.hpp"

#if defined(THERMAL_USE_HOSTFXR)
    #include "DotNetHost.hpp"
    using ControllerHost = DotNetHost;
#elif defined(THERMAL_USE_AOT)
    #include "NativeAotHost.hpp"
    using ControllerHost = NativeAotHost;
#else
    #error "Select an interoperability implementation"
#endif


inline std::filesystem::path requiredPath(
    const char* variable)
{
    const char* value = std::getenv(variable);

    if (!value || !*value) {
        throw std::runtime_error(
            std::string("Missing environment variable: ") +
            variable
        );
    }

    return std::filesystem::path{value};
}

inline ControllerHost createHost()
{
#if defined(THERMAL_USE_HOSTFXR)
    return ControllerHost{
        requiredPath("THERMAL_RUNTIME_CONFIG"),
        requiredPath("THERMAL_ASSEMBLY")
    };
#else
    return ControllerHost{
        requiredPath("THERMAL_AOT_LIBRARY")
    };
#endif
}
