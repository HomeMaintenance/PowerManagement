#include "BatteryInverter.h"

BatteryInverter::BatteryInverter(const std::string& name){
    _name = name;
}

float BatteryInverter::get_available_power() { return available_power; }

void BatteryInverter::set_available_power(float value){ available_power = value; }

std::string BatteryInverter::name() const{
    return _name;
}

void BatteryInverter::setName(const std::string& name){
    if(name != _name)
        _name = name;
}

Json::Value BatteryInverter::toJson(){
    Json::Value jsonData;
    jsonData["available_power"] = get_available_power();
    jsonData["present_discharge"] = present_discharge();
    jsonData["present_charge"] = present_charge();
    jsonData["online"] = online();
    return jsonData;
}
