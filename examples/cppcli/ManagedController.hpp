
#pragma once

// --------------------------------------------------
// Native interface to a managed C# controller
// --------------------------------------------------

class ManagedController {
public:
    explicit ManagedController(double targetTemperature);
    ~ManagedController();

    // Prevent accidental copying
    ManagedController(const ManagedController&) = delete;
    ManagedController& operator=(const ManagedController&) = delete;

    // Call the managed C# calculation
    double calculatePower(double temperature) const;

private:
    // Hide all C++/CLI-specific types from native code
    class Impl;
    Impl* impl_;
};
