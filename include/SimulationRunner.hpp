#pragma once

#include <cstddef>
#include <vector>


class ThermalSystem;

class SimulationRunner {
    public:
        explicit SimulationRunner(ThermalSystem& system);
        
        void run(std::size_t steps, double power, double dt);
    
        [[nodiscard]] const std::vector<double>& getHistory() const;

    private:
        ThermalSystem& system_;
        std::vector<double> history_;


        
};
