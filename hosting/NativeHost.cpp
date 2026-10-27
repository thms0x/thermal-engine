
#include <iostream>
#include <filesystem>
#include <dlfcn.h>

#include <nethost.h>
#include <hostfxr.h>
#include <coreclr_delegates.h>

namespace fs = std::filesystem;

using PowerFunction = double (*)(double);

int main()
{
    // find hostfxr
    char_t path[4096];
    size_t size = sizeof(path);

    if (get_hostfxr_path(path, &size, nullptr) != 0)
        return 1;

    void* library = dlopen(path, RTLD_NOW | RTLD_LOCAL);

    if (!library) {
        std::cerr << dlerror() << '\n';
        return 1;
    }

    // get hostfxr functions
    auto init = reinterpret_cast<hostfxr_initialize_for_runtime_config_fn>(
        dlsym(library, "hostfxr_initialize_for_runtime_config"));

    auto getDelegate = reinterpret_cast<hostfxr_get_runtime_delegate_fn>(
        dlsym(library, "hostfxr_get_runtime_delegate"));

    auto close = reinterpret_cast<hostfxr_close_fn>(
        dlsym(library, "hostfxr_close"));

    if (!init || !getDelegate || !close)
        return 1;

    // init .NET
    fs::path folder = fs::absolute("managed/ThermalController/bin/Release/net10.0");
    fs::path config = folder / "ThermalController.runtimeconfig.json";
    fs::path assembly = folder / "ThermalController.dll";

    hostfxr_handle context = nullptr;

    int rc = init(config.c_str(), nullptr, &context);

    if (rc < 0 || !context) {
        std::cerr << "Runtime initialization failed: " << rc << '\n';
        if (context) close(context);
        return 1;
    }

    // get assembly loader
    void* loader = nullptr;

    rc = getDelegate(
        context,
        hdt_load_assembly_and_get_function_pointer,
        &loader
    );

    close(context);

    if (rc < 0 || !loader)
        return 1;

    auto load = reinterpret_cast<load_assembly_and_get_function_pointer_fn>(
        loader);

    // get c# method
    PowerFunction calculatePower = nullptr;

    rc = load(
        assembly.c_str(),
        "ThermalController.ThermalController, ThermalController",
        "CalculatePower",
        UNMANAGEDCALLERSONLY_METHOD,
        nullptr,
        reinterpret_cast<void**>(&calculatePower)
    );

    if (rc != 0 || !calculatePower) {
        std::cerr << "Could not load C# method: " << rc << '\n';
        return 1;
    }

    // call c# method
    std::cout << calculatePower(30.0) << '\n';
    std::cout << calculatePower(70.0) << '\n';

    return 0;
}
