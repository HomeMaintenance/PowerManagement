#pragma once
#include <PowerSink.h>

class TestSink: public PowerSink{
    virtual float using_power() const { return allowed_power(); }
};
