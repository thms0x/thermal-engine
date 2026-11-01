
#include <benchmark/benchmark.h>

#include <cstddef>

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
// Native C++ controller
// --------------------------------------------------

class NativeController {
public:
    explicit NativeController(double targetTemperature)
        : targetTemperature_(targetTemperature)
    {
    }

    double calculatePower(double temperature) const
    {
        return temperature < targetTemperature_ ? 100.0 : 0.0;
    }

private:
    double targetTemperature_;
};

// --------------------------------------------------
// Benchmark 1: Native C++ method invocation
// --------------------------------------------------

static void BM_CalculatePower(benchmark::State& state)
{
    NativeController controller{targetTemperature};

    for (auto _ : state) {
        (void)_;

        double temperature = inputTemperature;
        benchmark::DoNotOptimize(temperature);

        const double power =
            controller.calculatePower(temperature);

        benchmark::DoNotOptimize(power);
    }
}

BENCHMARK(BM_CalculatePower)
    ->Unit(benchmark::kNanosecond);

// --------------------------------------------------
// Benchmark 2: Native thermal simulation
// --------------------------------------------------

static void BM_FullSimulation(benchmark::State& state)
{
    NativeController controller{targetTemperature};

    for (auto _ : state) {
        (void)_;

        ThermalSystem system{
            initialTemperature,
            heatCapacity,
            ambientTemperature,
            heatLossCoefficient
        };

        for (std::size_t i = 0; i < simulationSteps; ++i) {
            const double power =
                controller.calculatePower(
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

// --------------------------------------------------
// Benchmark entry point
// --------------------------------------------------

BENCHMARK_MAIN();
