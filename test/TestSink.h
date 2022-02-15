#pragma once
#include <PowerSink.h>

class TestSink: public PowerSink{
    virtual float using_power(){ return allowed_power(); }
};
