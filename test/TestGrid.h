#pragma once
#include <PowerGrid.h>

class TestGrid: public PowerGrid{
public:
    virtual float get_power() const override{
        return 300;
    }
};
