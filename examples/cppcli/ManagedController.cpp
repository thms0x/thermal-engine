
#include "ManagedController.hpp"

#include <msclr/gcroot.h>

#using <ThermalController.dll>

// --------------------------------------------------
// C++/CLI implementation
// Windows only - requires MSVC with /clr
// --------------------------------------------------

class ManagedController::Impl {
public:
    explicit Impl(double targetTemperature)
        : controller_(
            gcnew ThermalController::ThermalController(
                targetTemperature
            )
        )
    {
    }

    double calculatePower(double temperature) const
    {
        // Directly call the C# instance method
        return controller_->CalculatePower(temperature);
    }

private:
    // GC-aware reference to the C# object
    msclr::gcroot<
        ThermalController::ThermalController^
    > controller_;
};

// --------------------------------------------------
// Native-facing wrapper
// --------------------------------------------------

ManagedController::ManagedController(double targetTemperature)
    : impl_(new Impl(targetTemperature))
{
}

ManagedController::~ManagedController()
{
    delete impl_;
}

double ManagedController::calculatePower(
    double temperature) const
{
    return impl_->calculatePower(temperature);
}
