#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "PowerSource.h"
#include "PowerSink.h"
#include "BatteryManager.h"

/**
 * @brief
 *
 */
class PowerManager{
public:
    PowerManager(
        std::vector<std::weak_ptr<PowerSource>> sources = {},
        std::vector<std::weak_ptr<PowerSink>> sinks = {}
    );

    void add_source(std::weak_ptr<PowerSource> source);
    void add_sink(std::weak_ptr<PowerSink> sink);

    std::unordered_map<std::string, float> get_power_distribution() const;

    float power_buffer{0};
    void set_power_grid(float (*function)());

    float distribute();

protected:
private:
    float available_power();

    float (*power_grid)();
    std::unordered_map<std::string, float> power_distribution;
    std::vector<std::weak_ptr<PowerSource>> sources;
    std::vector<std::weak_ptr<PowerSink>> sinks;
};
