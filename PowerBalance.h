#pragma once
#include <json/json.h>
#include <httplib.h>
#include <unordered_map>

class PowerBalance{
public:
    Json::Value toJson() const;
    void register_http_server_functions(httplib::Server* svr);

    std::unordered_map<std::string, float> getPowerGeneration() const;
    void setPowerGeneration(const std::unordered_map<std::string, float>& map);

    std::unordered_map<std::string, float> getPowerDistribution() const;
    void setPowerDistribution(const std::unordered_map<std::string, float>& map);

protected:
private:
    std::unordered_map<std::string, float> power_generation;
    std::unordered_map<std::string, float> power_distribution;
    float grid_power{0};
};
