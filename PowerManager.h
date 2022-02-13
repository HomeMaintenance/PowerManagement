#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "PowerSource.h"
#include "PowerSink.h"
#include "BatteryManager.h"

/**
 * @brief Manager of power between multiple #PowerSource and #PowerSink
 *
 */
class PowerManager{
public:
    /**
     * @brief Construct a new Power Manager
     *
     * @param sources Vector of #PowerSource
     * @param sinks Vector of #PowerSink
     */
    PowerManager(
        std::vector<std::weak_ptr<PowerSource>> sources = {},
        std::vector<std::weak_ptr<PowerSink>> sinks = {}
    );

    /**
     * @brief Add a source to the manager
     *
     * @param source
     */
    void add_source(std::weak_ptr<PowerSource> source);
    /**
     * @brief Add a sink to the manager
     *
     * @param sink
     */
    void add_sink(std::weak_ptr<PowerSink> sink);

    /**
     * @brief Get the power distribution of the manager
     *
     * @return std::unordered_map<std::string, float>
     */
    std::unordered_map<std::string, float> get_power_distribution() const;

    float power_buffer{0};

    /**
     * @brief Set a function to allow the manager to get the power status to the grid
     *
     * @param function
     */
    void set_power_grid(float (*function)());

    /**
     * @brief Function to distribute the power between sources and sinks
     *
     * @return float Remaining power which couldn't be distributed
     */
    float distribute();

protected:
private:
    /**
     * @brief Get the available power from sources
     *
     * @return float
     */
    float available_power();

    float (*power_grid)();
    std::unordered_map<std::string, float> power_distribution;
    std::vector<std::weak_ptr<PowerSource>> sources;
    std::vector<std::weak_ptr<PowerSink>> sinks;
};
