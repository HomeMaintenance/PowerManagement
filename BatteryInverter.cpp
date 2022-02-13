#pragma once
#include "BatteryInverter.h"

BatteryInverter::BatteryInverter(){}

float BatteryInverter::get_available_power() const { return available_power; }

void BatteryInverter::set_available_power(float value){ available_power = value; }
