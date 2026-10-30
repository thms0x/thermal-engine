
#pragma once

#include <filesystem>
#include <memory>

#include "ManagedController.hpp"

class DotNetHost {
public:
    DotNetHost(
        const std::filesystem::path& runtimeConfig,
        const std::filesystem::path& assemblyPath
    );

    ~DotNetHost() = default;

    DotNetHost(const DotNetHost&) = delete;
    DotNetHost& operator=(const DotNetHost&) = delete;

    [[nodiscard]]
    std::unique_ptr<ManagedController> createController(double targetTemperature) const;

private:
    using CreateFn = void* (*)(double);
    using CalculateFn = double (*)(void*, double);
    using DestroyFn = void (*)(void*);

    CreateFn create_ = nullptr;
    CalculateFn calculate_ = nullptr;
    DestroyFn destroy_ = nullptr;
};
