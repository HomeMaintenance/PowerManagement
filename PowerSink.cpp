#include "PowerSink.h"

PowerSink::PowerSink(std::string _name): name(_name) {}

float PowerSink::get_allowed_power() const { return allowed_power; }

void PowerSink::set_allowed_power(float power) { allowed_power = power; }

// bool PowerSink::allow_power(float power) {
//     static_assert(false, "You should override this");
//     set_allowed_power(power);
//     return true;
// }

PowerRange PowerSink::get_requesting_power() const {
    return requesting_power;
}

void PowerSink::set_requesting_power(const PowerRange& range){
    requesting_power = range;
}

void PowerSink::set_requesting_power(PowerRange& range){
    requesting_power = range;
}

void PowerSink::set_requesting_power(float min, float max){
    requesting_power = PowerRange(min, max);
}
