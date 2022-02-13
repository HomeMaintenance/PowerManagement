#pragma once
#include <string>

/**
 * @brief PowerRange
 *
 */
class PowerRange{
public:
    /**
     * @brief Construct a new Power Range object
     *
     * @param min min power
     * @param max max power
     */
    PowerRange(float min, float max);
    void set_min(float value);
    float get_min() const;
    void set_max(float value);
    float get_max() const;
    std::string to_string() const;
    /**
     * @brief Set max power to min power
     *
     */
    void floor();
    /**
     * @brief Set min power to max power
     *
     */
    void ceil();
    /**
     * @brief Set min and max power to the average power of both
     *
     */
    void mean();
private:
    float min;
    float max;
};
