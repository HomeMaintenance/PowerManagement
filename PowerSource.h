#pragma once
#include <string>

class PowerSource{
public:
    PowerSource(std::string name);
    ~PowerSource() = default;
    float get_available_power() const;
    const std::string name;
protected:
    virtual void set_available_power(float value);
private:
    float available_power{0.f};
};
