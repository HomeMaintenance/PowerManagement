#include "BatteryManager.h"

BatteryManager::BatteryManager(std::string _name, std::vector<std::weak_ptr<BatteryInverter>> _inverters): name(_name), _inverters(_inverters) {}

void BatteryManager::add_inverter(std::weak_ptr<BatteryInverter> inverter){
    _inverters.push_back(inverter);
}

float BatteryManager::inverters_read_property(float (BatteryInverter::*func)()){
     float result = 0;
    for(const auto& i: _inverters){
        if(auto inverter = i.lock()){
            BatteryInverter* p_inverter = inverter.get();
            result += (p_inverter->*func)();
        }
    }
    return result;
}

float BatteryManager::available_power(){
    return BatteryManager::inverters_read_property(&BatteryInverter::get_available_power);
}

float BatteryManager::soc(){
    float result = 0;
    float reached_inverters = 0.f;
    for(const auto& i: _inverters){
        if(auto inverter = i.lock()){
            ++reached_inverters;
            result += inverter->soc();
        }
    }
    return result/reached_inverters;
}

float BatteryManager::present_discharge(){
    return BatteryManager::inverters_read_property(&BatteryInverter::present_discharge);
}

float BatteryManager::present_charge(){
    return BatteryManager::inverters_read_property(&BatteryInverter::present_charge);
}

float BatteryManager::charged_energy(){
    return BatteryManager::inverters_read_property(&BatteryInverter::charged_energy);
}

float BatteryManager::max_capacity(){
    return BatteryManager::inverters_read_property(&BatteryInverter::max_capacity);
}

float BatteryManager::missing_charge(){
    return BatteryManager::inverters_read_property(&BatteryInverter::missing_charge);
}

std::map<std::string, bool> BatteryManager::online_inverters() const {
    std::map<std::string, bool> result;
    for(const auto& i: _inverters){
        auto inverter = i.lock();
        if(!inverter)
            continue;
        result[inverter->name()] = inverter->online();
    }
    return result;
}

std::vector<std::weak_ptr<BatteryInverter>> BatteryManager::inverters() const {
    return _inverters;
}

Json::Value BatteryManager::toJson(){
    Json::Value result;
    result["soc"] = soc();
    result["power"] = available_power();
    result["discharge"] = present_discharge();
    result["charge"] = present_charge();
    Json::Value jsonInverters;
    for(const auto& i: _inverters){
        auto inverter = i.lock();
        if(!inverter)
            continue;
        jsonInverters[inverter->name()] = inverter->toJson();
    }
    result["inverters"] = jsonInverters;
    return result;
}
