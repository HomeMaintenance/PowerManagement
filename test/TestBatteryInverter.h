#pragma once
#include <BatteryInverter.h>

class TestBatteryInverter: public BatteryInverter{
public:
    TestBatteryInverter(const std::string& name);
    float soc() override;
    float present_discharge() override;
    float present_charge() override;

    float charged_energy() override;
    float max_capacity() override;
    float missing_charge() override;

    float max_charge_rate() const override;
    float max_discharge_rate() const override;

    bool online() const final;
    bool _online{true};
};
