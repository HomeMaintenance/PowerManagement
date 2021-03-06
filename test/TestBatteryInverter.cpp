#include "TestBatteryInverter.h"

TestBatteryInverter::TestBatteryInverter(const std::string& name): BatteryInverter(name) {}

float TestBatteryInverter::soc() {
    return 0.47;
}

float TestBatteryInverter::present_discharge() {
    return 0;
}

float TestBatteryInverter::present_charge() {
    return 560;
}

float TestBatteryInverter::charged_energy() {
    return max_capacity() * soc();
}

float TestBatteryInverter::max_capacity() {
    return 9500;
}

float TestBatteryInverter::missing_charge() {
    return max_capacity() - charged_energy();
}

float TestBatteryInverter::max_charge_rate() const{
    return 3000;
}

float TestBatteryInverter::max_discharge_rate() const{
    return 4500;
}

bool TestBatteryInverter::online() const
{
    return _online;
};
