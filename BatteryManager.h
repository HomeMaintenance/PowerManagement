#pragma once
#include <string>
#include <vector>
#include <memory>
#include "BatteryInverter.h"

/**
 * @brief BatteryManager
 * Used to manage multiple battery inverters
 *
 */
class BatteryManager{
public:
    /**
     * @brief Construct a new Battery Manager object
     *
     * @param name Name of the battery manager
     * @param inverters Vector of battery inverters
     */
    BatteryManager(std::string name, std::vector<std::weak_ptr<BatteryInverter>> inverters = {});
    std::string name;
    /**
     * @brief Add a battery inverter
     *
     * @param inverter
     */
    void add_inverter(std::weak_ptr<BatteryInverter> inverter);
    /**
     * @brief Read cumulative available power
     *
     * @return float [W]
     */
    float available_power() const; // W
    /**
     * @brief Read mean soc
     *
     * @return float [%]
     */
    float soc() const; // %
    /**
     * @brief Read cumulative present discharge
     *
     * @return float [W]
     */
    float present_discharge() const; // W
    /**
     * @brief Read cumulative present charge
     *
     * @return float [W]
     */
    float present_charge() const; // W
    /**
     * @brief Read cumulative present charged energy
     *
     * @return float [Wh]
     */
    float charged_energy() const; // Wh
    /**
     * @brief Read cumulative max capacity
     *
     * @return float [Wh]
     */
    float max_capacity() const; // Wh
    /**
     * @brief Read cumulative missing charge
     *
     * @return float [Wh]
     */
    float missing_charge() const; // Wh
protected:
private:
    /**
     * @brief Call function of inverters and return sum of results
     *
     * @param func Function to call of #BatteryInverter
     * @return float sum of results
     */
    float inverters_read_property(float (BatteryInverter::*func)() const) const;
    std::vector<std::weak_ptr<BatteryInverter>> inverters;
};
