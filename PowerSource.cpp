#include "PowerSource.h"

PowerSource::PowerSource(std::string _name): name(_name) {}

float PowerSource::get_available_power(){
    return available_power;
}

void PowerSource::set_available_power(float value){
    if(value != available_power)
        available_power = value;
}
