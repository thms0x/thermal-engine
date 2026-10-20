#include "SimulationRunner.hpp"
#include "ThermalSystem.hpp"

SimulationRunner::SimulationRunner(ThermalSystem& system) : system_(system){
}

void SimulationRunner::run(std::size_t steps, double power, double dt){
    history_.clear();
    history_.reserve(steps);

    for(std::size_t i = 0; i < steps; i++){
        system_.step(power, dt);
        history_.push_back(system_.getTemperature());
    }
}

const std::vector<double>& SimulationRunner::getHistory() const{
    return history_;

}