#pragma once

class PowerGrid{
public:
    /**
     *  Function to calulate the power from the grid.
     *  Negative if more power is drawn than generated.
     */
    virtual float get_power() const = 0;
};
