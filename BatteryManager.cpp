#include "BatteryManager.h"

BatteryManager::BatteryManager(std::string _name, std::vector<std::weak_ptr<BatteryInverter>> _inverters): name(_name), inverters(_inverters) {}

void BatteryManager::add_inverter(std::weak_ptr<BatteryInverter> inverter){
    inverters.push_back(inverter);
}

float BatteryManager::inverters_read_property(float (BatteryInverter::*func)() const) const {
     float result = 0;
    for(const auto& i: inverters){
        if(auto inverter = i.lock()){
            BatteryInverter* p_inverter = inverter.get();
            result += (p_inverter->*func)();
        }
    }
    return result;
}

float BatteryManager::available_power() const {
    return BatteryManager::inverters_read_property(&BatteryInverter::get_available_power);
}

float BatteryManager::soc() const {
    float result = 0;
    float reached_inverters = 0.f;
    for(const auto& i: inverters){
        if(auto inverter = i.lock()){
            ++reached_inverters;
            result += inverter->soc();
        }
    }
    return result/reached_inverters;
}

float BatteryManager::present_discharge() const {
    return BatteryManager::inverters_read_property(&BatteryInverter::present_discharge);
}

float BatteryManager::present_charge() const {
    return BatteryManager::inverters_read_property(&BatteryInverter::present_charge);
}

float BatteryManager::charged_energy() const {
    return BatteryManager::inverters_read_property(&BatteryInverter::charged_energy);
}

float BatteryManager::max_capacity() const {
    return BatteryManager::inverters_read_property(&BatteryInverter::max_capacity);
}

float BatteryManager::missing_charge() const {
    return BatteryManager::inverters_read_property(&BatteryInverter::missing_charge);
}
