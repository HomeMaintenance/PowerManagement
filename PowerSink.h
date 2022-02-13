#pragma once
#include <string>
#include "PowerRange.h"

class PowerSink{
public:
    PowerSink(std::string name);

    const std::string name;

    virtual float using_power() const;
    float allowed_power() const;

    virtual bool allow_power(float power);

    PowerRange get_requesting_power() const;

protected:
    virtual void set_requesting_power(PowerRange& range);
    virtual void set_requesting_power(float min, float max);

private:

    PowerRange requesting_power{PowerRange{0,0}};
    float _allowed_power{0.f};
};
