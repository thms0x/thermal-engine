
#pragma once

class ManagedController {
public:
    ManagedController(
        void* handle,
        double (*calculate)(void*, double),
        void (*destroy)(void*)
    );

    ~ManagedController();

    ManagedController(const ManagedController&) = delete;
    ManagedController& operator=(const ManagedController&) = delete;

    double calculatePower(double temperature) const;

private:
    void* handle_;

    double (*calculate_)(void*, double);
    void (*destroy_)(void*);
};
