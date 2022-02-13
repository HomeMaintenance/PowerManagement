#pragma once
#include <string>

class PowerRange{
public:
    PowerRange(float min, float max);
    void set_min(float value);
    float get_min() const;
    void set_max(float value);
    float get_max() const;
    std::string to_string() const;
    void floor();
    void ceil();
    void mean();
private:
    float min;
    float max;
};
