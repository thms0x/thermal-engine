
#include <benchmark/benchmark.h>

#include <cstddef>

#include "ControllerSetup.hpp"
#include "ThermalSystem.hpp"

// --------------------------------------------------
// Benchmark configuration
// --------------------------------------------------

constexpr double targetTemperature = 50.0;
constexpr double inputTemperature = 30.0;

constexpr double initialTemperature = 20.0;
constexpr double heatCapacity = 1000.0;
constexpr double ambientTemperature = 20.0;
constexpr double heatLossCoefficient = 5.0;

constexpr double timeStep = 1.0;
constexpr std::size_t simulationSteps = 100;

// --------------------------------------------------
// Benchmark 1: Managed method invocation
// --------------------------------------------------

// Measures the steady-state cost of a C++ -> C# call.
// Runtime initialization and controller creation
// are excluded from the timed section.

static void BM_CalculatePower(benchmark::State& state)
{
    auto host = createHost();
    auto controller = host.createController(targetTemperature);

    // Warm up before measurements.
    benchmark::DoNotOptimize(
        controller->calculatePower(inputTemperature)
    );

    for (auto _ : state) {
        (void)_;

        double temperature = inputTemperature;
        benchmark::DoNotOptimize(temperature);

        const double power =
            controller->calculatePower(temperature);

        benchmark::DoNotOptimize(power);
    }
}

BENCHMARK(BM_CalculatePower)
    ->Unit(benchmark::kNanosecond);

// --------------------------------------------------
// Benchmark 2: Controller creation and destruction
// --------------------------------------------------

// Measures the combined cost of:
// - Creating the C# ThermalController
// - Allocating its GCHandle
// - Creating the native RAII wrapper
// - Destroying the wrapper and freeing the handle
//
// Runtime/library initialization is excluded.

static void BM_CreateDestroy(benchmark::State& state)
{
    auto host = createHost();

    for (auto _ : state) {
        (void)_;

        auto controller =
            host.createController(targetTemperature);

        benchmark::DoNotOptimize(controller.get());

        // The controller is destroyed at the end
        // of this iteration.
    }
}

BENCHMARK(BM_CreateDestroy)
    ->Unit(benchmark::kNanosecond);

// --------------------------------------------------
// Benchmark 3: Full thermal simulation
// --------------------------------------------------

// Measures a complete simulation consisting of
// 100 steps using a managed C# controller.
//
// Each benchmark iteration creates a fresh native
// ThermalSystem with identical initial conditions.
//
// Runtime initialization and managed controller
// creation are excluded.

static void BM_FullSimulation(benchmark::State& state)
{
    auto host = createHost();
    auto controller = host.createController(targetTemperature);

    // Warm up the managed call.
    benchmark::DoNotOptimize(
        controller->calculatePower(initialTemperature)
    );

    for (auto _ : state) {
        (void)_;

        // Each iteration starts with the same state.
        ThermalSystem system{
            initialTemperature,
            heatCapacity,
            ambientTemperature,
            heatLossCoefficient
        };

        for (std::size_t i = 0; i < simulationSteps; ++i) {
            const double power =
                controller->calculatePower(
                    system.getTemperature()
                );

            system.step(power, timeStep);
        }

        benchmark::DoNotOptimize(
            system.getTemperature()
        );
    }
}

BENCHMARK(BM_FullSimulation)
    ->Unit(benchmark::kMicrosecond);


BENCHMARK_MAIN();
