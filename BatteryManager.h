#pragma once
#include <string>
#include <vector>
#include <memory>
#include "BatteryInverter.h"

class BatteryManager{
public:
    BatteryManager(std::string name, std::vector<std::weak_ptr<BatteryInverter>> inverters = {});
    std::string name;
    void add_inverter(std::weak_ptr<BatteryInverter> inverter);
    float available_power() const; // W
    float soc() const; // %
    float present_discharge() const; // W
    float present_charge() const; // W
    float charged_energy() const; // Wh
    float total_capacity() const; // Wh
    float missing_charge() const; // Wh
protected:
private:
    float inverters_read_property(float (BatteryInverter::*func)() const) const;
    std::vector<std::weak_ptr<BatteryInverter>> inverters;
};
