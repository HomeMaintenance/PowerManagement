#include "TestBatteryInverter.h"

TestBatteryInverter::TestBatteryInverter(): BatteryInverter() {}

float TestBatteryInverter::soc() const {
    return 0.47;
}

float TestBatteryInverter::present_discharge() const {
    return 0;
}

float TestBatteryInverter::present_charge() const {
    return 560;
}

float TestBatteryInverter::charged_energy() const {
    return max_capacity() * soc();
}

float TestBatteryInverter::max_capacity() const {
    return 9500;
}

float TestBatteryInverter::missing_charge() const {
    return max_capacity() - charged_energy();
}
