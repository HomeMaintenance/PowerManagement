#pragma once
#include <BatteryInverter.h>

class TestBatteryInverter: public BatteryInverter{
public:
    TestBatteryInverter();
    float soc() const override;
    float present_discharge() const override;
    float present_charge() const override;

    float charged_energy() const override;
    float total_capacity() const override;
    float missing_charge() const override;
};
