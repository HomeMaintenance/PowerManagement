#include "PowerBalance.h"

std::string convert_to_string(const Json::Value& json);
float sumUp(const std::unordered_map<std::string, float>& map);

Json::Value PowerBalance::toJson() const {
    Json::Value result{};
    Json::Value distribution{};
    Json::Value generation{};
    for(const auto& i : power_generation){
        generation[i.first] = i.second;
    }
    result["generation"] = generation;
    for(const auto& i: power_distribution){
        distribution[i.first] = i.second;
    }
    result["distribution"] = distribution;
    result["grid"] = grid_power;
    return result;
}

void PowerBalance::register_http_server_functions(httplib::Server* svr){
    svr->Get("/PowerBalance/json", [this](const httplib::Request& request, httplib::Response& response){
        response.status = 200;
        response.set_content(convert_to_string(toJson()),"application/json");
    });
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
