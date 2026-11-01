
#include <benchmark/benchmark.h>

#include "ControllerSetup.hpp"

// Measure native-to-C# method-call overhead
static void BM_CalculatePower(benchmark::State& state)
{
    auto host = createHost();
    auto controller = host.createController(50.0);

    // Warm up 
    const double warmup = controller->calculatePower(30.0);
    benchmark::DoNotOptimize(warmup);

    // meassure calculatePower()
    for (auto _ : state) {
        (void)_;

        double temperature = 30.0;
        benchmark::DoNotOptimize(temperature);

        double power = controller->calculatePower(temperature);
        benchmark::DoNotOptimize(power);
    }
}

BENCHMARK(BM_CalculatePower)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_MAIN();
