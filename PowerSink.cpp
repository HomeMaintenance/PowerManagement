#include "PowerSink.h"

PowerSink::PowerSink(std::string _name): name(_name) {}

float PowerSink::using_power() const { return allowed_power(); }

float PowerSink::allowed_power() const { return _allowed_power; }

bool PowerSink::allow_power(float power) {
    _allowed_power = power;
    return true;
}

PowerRange PowerSink::get_requesting_power() const {
    return requesting_power;
}

void PowerSink::set_requesting_power(PowerRange& range){
    requesting_power = range;
}

void PowerSink::set_requesting_power(float min, float max){
    requesting_power = PowerRange(min, max);
}
