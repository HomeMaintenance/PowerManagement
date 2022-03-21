#pragma once
#include <string>

/**
 * @brief Used to add a source to the #PowerManager
 *
 */
class PowerSource{
public:
    PowerSource(std::string name);
    virtual ~PowerSource() = default;

    /**
     * @brief Get the available power
     *
     * @return float [W]
     */
    virtual float get_available_power() const;
    const std::string name;
protected:
    /**
     * @brief Set the available power
     *
     * @param value [W]
     */
    virtual void set_available_power(float value);
private:
    float available_power{0.f};
};
