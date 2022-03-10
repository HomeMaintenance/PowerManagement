#include <iostream>
#include <csignal>
#include "BatteryManager.h"
#include "TestBatteryInverter.h"
#include "TestGrid.h"
#include "PowerManager.h"

bool run_servers = true;

void signal_handler(int signal){
    std::cout << signal << std::endl;
    if(signal == SIGINT){
        run_servers = false;
    }
}

void server_thread_func(std::weak_ptr<httplib::Server> server, const char* address, int port){
    server.lock()->listen(address, port);
}

int main(int argc, char* argv[])
{
    std::signal(SIGINT, signal_handler);
    std::cout << "Hello, world!" << std::endl;

    std::shared_ptr<TestBatteryInverter> myInverter0 = std::make_shared<TestBatteryInverter>();
    std::shared_ptr<TestBatteryInverter> myInverter1 = std::make_shared<TestBatteryInverter>();
    std::shared_ptr<BatteryManager> myManager = std::make_shared<BatteryManager>("myManager", std::vector<std::weak_ptr<BatteryInverter>>({myInverter0, myInverter1}));
    float available_power = myManager->available_power();
    std::cout << "available_power: " << available_power << std::endl;
    float soc = myManager->soc();
    std::cout << "soc: " << soc << std::endl;
    float present_discharge = myManager->present_discharge();
    std::cout << "present_discharge: " << present_discharge << std::endl;
    float present_charge = myManager->present_charge();
    std::cout << "present_charge: " << present_charge << std::endl;
    float charged_energy = myManager->charged_energy();
    std::cout << "charged_energy: " << charged_energy << std::endl;
    float max_capacity = myManager->max_capacity();
    std::cout << "max_capacity: " << max_capacity << std::endl;
    float missing_charge = myManager->missing_charge();
    std::cout << "missing_charge: " << missing_charge << std::endl;


    std::shared_ptr<TestGrid> test_grid = std::make_shared<TestGrid>();

    PowerManager powerManager;
    powerManager.set_power_grid(test_grid);
    powerManager.set_battery_manager(myManager);
    powerManager.start_loop();

    std::shared_ptr<httplib::Server> svr = std::make_shared<httplib::Server>();

    // Creates /PowerManager/status url
    powerManager.register_http_server_functions(svr.get());

    // run server in thread
    std::thread server_thread(server_thread_func, svr, "0.0.0.0", 8800);

    while(run_servers){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    svr->stop();
    powerManager.stop_loop();
    server_thread.join();

    return 0;
}
