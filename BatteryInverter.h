#pragma once

class BatteryInverter{
public:
    BatteryInverter();

    float get_available_power() const;

    virtual float soc() const = 0;
    virtual float present_discharge() const = 0;
    virtual float present_charge() const = 0;

    virtual float charged_energy() const = 0;
    virtual float total_capacity() const = 0;
    virtual float missing_charge() const = 0;

protected:
    void set_available_power(float value);

private:
    float available_power{0};
};
