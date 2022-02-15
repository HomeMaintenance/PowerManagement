#pragma once

/**
 * @brief Abstract class for an Inverter that can be used in an #BatteryManager instance.
 *
 */
class BatteryInverter{
public:
    /**
     * @brief Construct a new Battery Inverter object
     *
     */
    BatteryInverter();

    /**
     * @brief Get the available power [W]
     *
     * @return float
     */
    float get_available_power();

    /**
     * @brief Get soc of the battery [%]
     *
     * @return float
     */
    virtual float soc() = 0;
    /**
     * @brief Get the discharge power [W]
     *
     * @return float
     */
    virtual float present_discharge() = 0;
    /**
     * @brief Get the charge power [W]
     *
     * @return float
     */
    virtual float present_charge() = 0;

    /**
     * @brief Get charged energy [Wh]
     *
     * @return float
     */
    virtual float charged_energy() = 0;

    /**
     * @brief Get maximal capacity of the battery [Wh]
     *
     * @return float
     */
    virtual float max_capacity() = 0;
    /**
     * @brief Get missing charge. Max - charged_energy [Wh]
     *
     * @return float
     */
    virtual float missing_charge() = 0;

protected:
    /**
     * @brief Set the available power object
     *
     * @param value Pos: power is available. Neg: power is needed for charging. [W]
     */
    void set_available_power(float value);

private:
    float available_power{0};
};
