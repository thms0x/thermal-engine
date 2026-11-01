
#include "DotNetHost.hpp"

#include <nethost.h>
#include <hostfxr.h>
#include <coreclr_delegates.h>

#include <dlfcn.h>
#include <stdexcept>
#include <string>
#include <vector>

// --------------------------------------------------
// Initialize .NET and resolve managed functions
// --------------------------------------------------

DotNetHost::DotNetHost(
    const std::filesystem::path& runtimeConfig,
    const std::filesystem::path& assemblyPath
) {
    // Locate hostfxr
    std::vector<char_t> path(4096);
    size_t size = path.size();

    if (get_hostfxr_path(path.data(), &size, nullptr) != 0)
        throw std::runtime_error("Cannot locate hostfxr");

    // Load hostfxr
    void* library = dlopen(path.data(), RTLD_NOW | RTLD_LOCAL);

    if (!library)
        throw std::runtime_error(dlerror());

    // Resolve hosting functions
    auto init = reinterpret_cast<hostfxr_initialize_for_runtime_config_fn>(
        dlsym(library, "hostfxr_initialize_for_runtime_config"));

    auto getDelegate = reinterpret_cast<hostfxr_get_runtime_delegate_fn>(
        dlsym(library, "hostfxr_get_runtime_delegate"));

    auto close = reinterpret_cast<hostfxr_close_fn>(
        dlsym(library, "hostfxr_close"));

    if (!init || !getDelegate || !close)
        throw std::runtime_error("Cannot resolve hostfxr functions");

    // Initialize .NET
    auto config = std::filesystem::absolute(runtimeConfig);
    auto assembly = std::filesystem::absolute(assemblyPath);

    hostfxr_handle context = nullptr;
    int rc = init(config.c_str(), nullptr, &context);

    if (rc < 0 || !context) {
        if (context) close(context);
        throw std::runtime_error("Cannot initialize .NET: "
                                 + std::to_string(rc));
    }

    // Get the assembly loader
    void* loader = nullptr;

    rc = getDelegate(
        context,
        hdt_load_assembly_and_get_function_pointer,
        &loader
    );

    close(context);

    if (rc < 0 || !loader)
        throw std::runtime_error("Cannot get .NET runtime delegate");

    auto load = reinterpret_cast<
        load_assembly_and_get_function_pointer_fn>(loader);

    // Helper for resolving managed methods
    auto resolve = [&](const char* method) -> void* {
        void* address = nullptr;

        int result = load(
            assembly.c_str(),
            "ThermalController.ThermalControllerApi, ThermalController",
            method,
            UNMANAGEDCALLERSONLY_METHOD,
            nullptr,
            &address
        );

        if (result != 0 || !address)
            throw std::runtime_error(
                std::string("Cannot resolve managed method: ") + method
            );

        return address;
    };

    // Resolve Create, Calculate and Destroy
    create_ = reinterpret_cast<CreateFn>(resolve("Create"));
    calculate_ = reinterpret_cast<CalculateFn>(resolve("Calculate"));
    destroy_ = reinterpret_cast<DestroyFn>(resolve("Destroy"));
}

// --------------------------------------------------
// Create a managed C# controller
// --------------------------------------------------

std::unique_ptr<ManagedController>
DotNetHost::createController(double targetTemperature) const
{
    void* handle = create_(targetTemperature);

    if (!handle)
        throw std::runtime_error("Cannot create managed controller");

    // Protect the handle if constructing the wrapper throws.
    std::unique_ptr<void, DestroyFn> guard(handle, destroy_);

    auto controller = std::make_unique<ManagedController>(
        handle, calculate_, destroy_
    );

    guard.release(); // Ownership transferred to ManagedController
    return controller;
}
