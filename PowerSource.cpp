#include "PowerSource.h"

PowerSource::PowerSource(std::string _name): name(_name) {}

float PowerSource::get_available_power() const {
    return available_power;
}

void PowerSource::set_available_power(float value){
    if(value != available_power)
        available_power = value;
}

Json::Value PowerSource::toJson() const{
    Json::Value jsonData;
    jsonData["available_power"] = get_available_power();
    return jsonData;
}
