#pragma once
#include <vector>
#include <array>
#include <memory>
#include <unordered_map>
#include <thread>
#include <json/json.h>
#include <httplib.h>
#include "PowerSource.h"
#include "PowerSink.h"
#include "PowerGrid.h"
#include "BatteryManager.h"


struct DistributeData{
    float remaining;
    float available;
    float power;
    float grid;
    float buffer;
    float battery_power;
    float battery_soc;
    float battery_discharge;
    std::string power_reference;
    std::unordered_map<std::string, float> distribution;
    Json::Value toJson() const{
        Json::Value jsonData;
        jsonData["remaining"] = remaining;
        jsonData["available"] = available;
        jsonData["power"] = power;
        jsonData["grid"] = grid;
        jsonData["buffer"] = buffer;
        jsonData["battery_power"] = battery_power;
        jsonData["battery_soc"] = battery_soc;
        jsonData["battery_discharge"] = battery_discharge;
        jsonData["power_reference"] = power_reference;
        Json::Value jsonDistribution;
        for(const auto& d: distribution){
            jsonDistribution[d.first] = d.second;
        }
        jsonData["distribution"] = jsonDistribution;
        return jsonData;
    }
};

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

    struct DistributionResult {
    public:
        void set_power(float pwr);
        float get_power() const;
        void set_remaining(float remaining);
        float get_remaining() const;
        void set_used(float used);
        float get_used() const;
        void set_distribution(std::unordered_map<std::string, float>& distribution);
        std::unordered_map<std::string, float> get_distribution() const;
        bool validate() const;
    private:
        uint8_t _set{0};
        float _power;
        float _remaining;
        float _used;
        std::unordered_map<std::string, float> _distribution;
    };

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

    void set_battery_manager(std::weak_ptr<BatteryManager> battery_manager);

    /**
     * @brief Get the power distribution of the manager
     *
     * @return std::unordered_map<std::string, float>
     */
    std::unordered_map<std::string, float> get_power_distribution() const;
    std::unordered_map<std::string, float> get_power_generation() const;
    float get_grid_power() const;

    void set_power_buffer(float buffer);
    float get_power_buffer() const;

    /**
     * @brief Set a function to allow the manager to get the power status to the grid
     *
     * @param function
     */
    void set_power_grid(std::weak_ptr<PowerGrid> grid);

    /**
     * @brief Function to distribute the power between sources and sinks
     *
     * @return float Remaining power which couldn't be distributed
     */
    DistributionResult distribute();

    void start_loop();

    void stop_loop();

    void register_http_server_functions(httplib::Server* svr);

    DistributeData dist_buffer;

    void use_power_from_sources();
    void use_power_from_grid();
    std::string get_power_reference_str() const;

    void enable_log();
    void disable_log();

protected:
private:
    /**
     * @brief Get the available power from sources
     *
     * @return float
     */
    std::unordered_map<std::string, float> available_power();

    // Function to calulate the power from the grid.
    // Positive if more power is drawn than generated.
    // If more power is generated set to negative value.
    std::weak_ptr<PowerGrid> power_grid;
    std::unordered_map<std::string, float> power_distribution;
    std::unordered_map<std::string, float> power_generation;
    float grid_power_value{0};
    std::vector<std::weak_ptr<PowerSource>> sources;
    std::vector<std::weak_ptr<PowerSink>> sinks;
    std::weak_ptr<BatteryManager> battery_manager;
    float power_buffer{0};

    float distribute_remaining{0.0f};
    std::unique_ptr<std::thread> distribute_thread;
    void distribute_loop();
    float distribute_period = 5; // seconds
    bool distribute_run;

    bool _enable_log{false};
    void log(std::string message) const;

    enum PowerFrom{
        Sources = 0,
        Grid = 1
    };
    const std::array<std::string, 2> _powerFrom_LUT = {"Sources", "Grid"};
    PowerFrom _power_from{PowerFrom::Sources};
};
