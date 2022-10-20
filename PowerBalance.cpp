#include "PowerBalance.h"

std::string convert_to_string(const Json::Value& json);
float sumUp(const std::unordered_map<std::string, float>& map);

Json::Value PowerBalance::toJson() const {
    Json::Value result{};

    // Generation
    Json::Value generation{};
    float generation_total{0};
    for(const auto& i : power_generation){
        generation[i.first] = i.second;
        generation_total += i.second;
    }
    result["generation"] = generation;
    result["generation_total"] = generation_total;

    // Distribution
    Json::Value distribution{};
    float distribution_total{0};
    for(const auto& i: power_distribution){
        distribution[i.first] = i.second;
        distribution_total += i.second;
    }
    result["distribution"] = distribution;
    result["distribution_total"] = distribution_total;
    result["grid"] = grid_power;
    result["power_reference"] = power_reference;

    return result;
}

void PowerBalance::register_http_server_functions(httplib::Server* svr){
    svr->Get("/PowerBalance/json", [this](const httplib::Request& request, httplib::Response& response){
        update();
        if(update_valid){
            response.status = 200;
            response.set_content(convert_to_string(toJson()),"application/json");
        }
        else{
            response.status = 500;
        }
    });
}

void PowerBalance::setPowerManager(std::weak_ptr<PowerManager> pm){
    powerManager = pm;
}


std::unordered_map<std::string, float> PowerBalance::getPowerGeneration() const{
    return power_generation;
}

void PowerBalance::setPowerGeneration(const std::unordered_map<std::string, float>& map){
    power_generation = map;
}


std::unordered_map<std::string, float> PowerBalance::getPowerDistribution() const{
    return power_distribution;
}

void PowerBalance::setPowerDistribution(const std::unordered_map<std::string, float>& map){
    power_distribution = map;
}

void PowerBalance::setPowerReference(const std::string& reference) {
    power_reference = reference;
}

const std::string& PowerBalance::getPowerReference() const {
    return power_reference;
}

float PowerBalance::getGridPower() const{
    return grid_power;
}

void PowerBalance::setGridPower(const float& value){
    grid_power = value;
}

void PowerBalance::update(){
    if(auto pm = powerManager.lock()){
        setPowerDistribution(pm->get_power_distribution());
        setPowerGeneration(pm->get_power_generation());
        setGridPower(pm->get_grid_power());
        setPowerReference(pm->get_power_reference_str());
        update_valid = true;
    }
    else{
        update_valid = false;
    }
}
