
#include "NativeAotHost.hpp"
#include "ManagedController.hpp"

#include <dlfcn.h>

#include <stdexcept>
#include <string>
#include <utility>


NativeAotHost::NativeAotHost(
    const std::filesystem::path& libraryPath
) {
    const auto path = std::filesystem::absolute(libraryPath);

    library_ = dlopen(
        path.c_str(),
        RTLD_NOW | RTLD_LOCAL
    );

    if (!library_) {
        const char* error = dlerror();

        throw std::runtime_error(
            std::string("Cannot load Native AOT library: ") +
            (error ? error : "unknown error")
        );
    }

    auto resolve = [this](const char* name) -> void* {
        dlerror(); 

        void* address = dlsym(library_, name);

        const char* error = dlerror();

        if (error || !address) {
            throw std::runtime_error(
                std::string("Cannot resolve AOT function: ") + name
            );
        }

        return address;
    };

    create_ = reinterpret_cast<CreateFn>(
        resolve("thermal_create")
    );

    calculate_ = reinterpret_cast<CalculateFn>(
        resolve("thermal_calculate")
    );

    destroy_ = reinterpret_cast<DestroyFn>(
        resolve("thermal_destroy")
    );
}


NativeAotHost::~NativeAotHost() = default;


std::unique_ptr<ManagedController>
NativeAotHost::createController(
    double targetTemperature
) const {
    void* handle = create_(targetTemperature);

    if (!handle) {
        throw std::runtime_error(
            "Failed to create Native AOT controller"
        );
    }

    std::unique_ptr<void, DestroyFn> guard(handle, destroy_);

    auto controller = std::make_unique<ManagedController>(
        handle,
        calculate_,
        destroy_
    );

    guard.release();

    return controller;
}
