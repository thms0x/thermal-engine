
#include "ManagedController.hpp"

#include <stdexcept>

ManagedController::ManagedController(void* handle, double (*calculate)(void*, double),void (*destroy)(void*))
    : handle_(handle), calculate_(calculate), destroy_(destroy)
{
    if (!handle_ || !calculate_ || !destroy_) {
        throw std::invalid_argument(
            "Invalid managed controller or function pointer"
        );
    }
}

ManagedController::~ManagedController()
{
    destroy_(handle_);
}

double ManagedController::calculatePower(double temperature) const
{
    return calculate_(handle_, temperature);
}
