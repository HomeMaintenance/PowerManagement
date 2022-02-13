#include <iostream>
#include "BatteryManager.h"
#include "TestBatteryInverter.h"
#include "PowerManager.h"

int main(int argc, char* argv[])
{
    std::cout << "Hello, world!" << std::endl;

    std::shared_ptr<TestBatteryInverter> myInverter0 = std::make_shared<TestBatteryInverter>();
    std::shared_ptr<TestBatteryInverter> myInverter1 = std::make_shared<TestBatteryInverter>();
    BatteryManager myManager("myManager", {myInverter0, myInverter1});
    float available_power = myManager.available_power();
    std::cout << "available_power: " << available_power << std::endl;
    float soc = myManager.soc();
    std::cout << "soc: " << soc << std::endl;
    float present_discharge = myManager.present_discharge();
    std::cout << "present_discharge: " << present_discharge << std::endl;
    float present_charge = myManager.present_charge();
    std::cout << "present_charge: " << present_charge << std::endl;
    float charged_energy = myManager.charged_energy();
    std::cout << "charged_energy: " << charged_energy << std::endl;
    float total_capacity = myManager.total_capacity();
    std::cout << "total_capacity: " << total_capacity << std::endl;
    float missing_charge = myManager.missing_charge();
    std::cout << "missing_charge: " << missing_charge << std::endl;



    PowerManager powerManager;


    return 0;
}
