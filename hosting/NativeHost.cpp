
#include <iostream>
#include <filesystem>
#include <dlfcn.h>

#include <nethost.h>
#include <hostfxr.h>
#include <coreclr_delegates.h>

#include "ManagedController.hpp"

namespace fs = std::filesystem;

using CreateFn = void* (*)(double);
using CalculateFn = double (*)(void*, double);
using DestroyFn = void (*)(void*);

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

    // get c# methods
    //create()
    void* createAddress = nullptr;

    rc = load(
        assembly.c_str(),
        "ThermalController.ThermalControllerApi, ThermalController",
        "Create",
        UNMANAGEDCALLERSONLY_METHOD,
        nullptr,
        &createAddress
    );

    if (rc != 0 || !createAddress) {
        std::cerr << "Failed to resolve Create\n";
        return 1;
    }

    auto create = reinterpret_cast<CreateFn>(createAddress);

    //calculate()
    void* calculateAdress = nullptr;

    rc = load(
        assembly.c_str(),
        "ThermalController.ThermalControllerApi, ThermalController",
        "Calculate",
        UNMANAGEDCALLERSONLY_METHOD,
        nullptr,
        &calculateAdress
    );

    if (rc != 0 || !calculateAdress) {
        std::cerr << "Failed to resolve Calculate\n";
        return 1;
    }

    auto calculate = reinterpret_cast<CalculateFn>(calculateAdress);

    //Destroy()
    void* destroyAdress = nullptr;

    rc = load(
        assembly.c_str(),
        "ThermalController.ThermalControllerApi, ThermalController",
        "Destroy",
        UNMANAGEDCALLERSONLY_METHOD,
        nullptr,
        &destroyAdress
    );

    if (rc != 0 || !destroyAdress) {
        std::cerr << "Failed to resolve Destroy\n";
        return 1;
    }

    auto destroy = reinterpret_cast<DestroyFn>(destroyAdress);


    // call c# method
    ManagedController first{create(50.0), calculate, destroy};
    ManagedController second{create(80.0), calculate, destroy};

    std::cout << first.calculatePower(60.0) << '\n';
    std::cout << second.calculatePower(60.0) << '\n';

    // both destructors are automatically called here, so the gc on the c# side picks it up.
    
}
