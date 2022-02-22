#pragma once

class PowerGrid{
public:
    /**
     *  Function to calulate the power from the grid.
     *  Positive if more power is drawn than generated.
     *  If more power is generated set to negative value.
     */
    virtual float get_power() const = 0;
};
