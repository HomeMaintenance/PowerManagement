#pragma once
#include <json/json.h>
#include <httplib.h>
#include <unordered_map>
#include <memory>
#include "PowerManager.h"

class PowerBalance{
public:
    Json::Value toJson() const;
    void register_http_server_functions(httplib::Server* svr);

    void setPowerManager(std::weak_ptr<PowerManager> powerManager);

    std::unordered_map<std::string, float> getPowerGeneration() const;
    void setPowerGeneration(const std::unordered_map<std::string, float>& map);

    std::unordered_map<std::string, float> getPowerDistribution() const;
    void setPowerDistribution(const std::unordered_map<std::string, float>& map);

    void setPowerReference(const std::string& reference);
    const std::string& getPowerReference() const;

    float getGridPower() const;
    void setGridPower(const float& value);

protected:
private:
    std::unordered_map<std::string, float> power_generation;
    std::unordered_map<std::string, float> power_distribution;
    float grid_power{0};
    std::string power_reference;
    std::weak_ptr<PowerManager> powerManager;

    bool update_valid{false};
    void update();
};
