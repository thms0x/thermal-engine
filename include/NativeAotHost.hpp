
#pragma once

#include <filesystem>
#include <memory>

class ManagedController;

class NativeAotHost {
public:
    explicit NativeAotHost(
        const std::filesystem::path& libraryPath
    );

    ~NativeAotHost();

    NativeAotHost(const NativeAotHost&) = delete;
    NativeAotHost& operator=(const NativeAotHost&) = delete;

    NativeAotHost(NativeAotHost&&) = delete;
    NativeAotHost& operator=(NativeAotHost&&) = delete;

    [[nodiscard]]
    std::unique_ptr<ManagedController> createController(
        double targetTemperature
    ) const;

private:
    using CreateFn = void* (*)(double);
    using CalculateFn = double (*)(void*, double);
    using DestroyFn = void (*)(void*);

    void* library_ = nullptr;

    CreateFn create_ = nullptr;
    CalculateFn calculate_ = nullptr;
    DestroyFn destroy_ = nullptr;
};
