#include "TestBatteryInverter.h"

TestBatteryInverter::TestBatteryInverter(): BatteryInverter() {}

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
