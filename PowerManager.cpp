#include "PowerManager.h"

Json::Value convert_to_json(const std::string str){
    Json::Value jsonData;
    Json::Reader jsonReader;
    jsonReader.parse(str, jsonData);
    return jsonData;
}

std::string convert_to_string(const Json::Value& json){
    return Json::FastWriter().write(json);
}

PowerManager::PowerManager(
        std::vector<std::weak_ptr<PowerSource>> _sources,
        std::vector<std::weak_ptr<PowerSink>> _sinks
    ):
    sources(_sources), sinks(_sinks)
{

}

void PowerManager::add_source(std::weak_ptr<PowerSource> source){
    sources.push_back(source);
}

void PowerManager::add_sink(std::weak_ptr<PowerSink> sink){
    sinks.push_back(sink);
}

void PowerManager::set_battery_manager(std::weak_ptr<BatteryManager> _battery_manager){
    battery_manager = _battery_manager;
}

std::unordered_map<std::string, float> PowerManager::get_power_distribution() const {
    return power_distribution;
}

void PowerManager::set_power_grid(std::weak_ptr<PowerGrid> grid){
    power_grid = grid;
}

float PowerManager::distribute(){
    float _power_grid = 0.f;
    float _available_power = 0.f;
    if(auto pwr_grd = power_grid.lock()){
        _power_grid = pwr_grd->get_power();
        dist_buffer.grid = _power_grid;
        dist_buffer.available = 0;
        for(const auto& i: power_distribution){
            // Needed because otherwise the already turned on devices would be ignored
            _available_power += i.second;
        }
    }
    else{
       _available_power = available_power();
        dist_buffer.grid = 0;
       dist_buffer.available = _available_power;
    }

    float battery_power = 0;
    auto _battery_manager = battery_manager.lock();
    if(use_battery_power && _battery_manager){
        battery_power = _battery_manager->available_power();
    }

    float power = _available_power + battery_power - _power_grid - power_buffer;
    dist_buffer.power = power;
    dist_buffer.buffer = power_buffer;

    std::unordered_map<std::string, float> _pwr_dist;

    for(const auto& s: sinks){
        auto sink = s.lock();
        if(!sink)
            continue;
        float grant_power = power;
        const PowerRange sink_requesting_power = sink->get_requesting_power();
        if(grant_power >= sink_requesting_power.get_min() && sink_requesting_power.get_max() > 0){
            if(grant_power > sink_requesting_power.get_max())
                grant_power = sink_requesting_power.get_max();
            if( sink->allow_power(grant_power) ){
                // success
            }
            else{
                // failed to accept power
            }
        }
        else{
            sink->allow_power(0); // switch off
        }
        _pwr_dist[sink->name] = sink->using_power();
        power -= sink->using_power();
    }
    power_distribution = _pwr_dist;
    dist_buffer.distribution = power_distribution;
    dist_buffer.remaining = power;
    return power; // return remaining power
}

float PowerManager::available_power(){
    float result = 0.f;

    for(const auto& s: sources){
        if(auto source = s.lock()){
            const auto source_power = source->get_available_power();
            result += source_power;
        }
    }

    return result;
}

void PowerManager::distribute_loop(){
    while(distribute_run){
        distribute();
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(distribute_period*1000)));
    }
}

void PowerManager::start_loop(){
    distribute_run = true;
    distribute_thread = std::make_unique<std::thread>(&PowerManager::distribute_loop, this);
}

void PowerManager::stop_loop(){
    distribute_run = false;
    distribute_thread->join();
}

void PowerManager::register_http_server_functions(httplib::Server* svr){
    svr->Get(
        "/PowerManager/status",
        [this]
        (const httplib::Request &req, httplib::Response &res)
        {
            Json::Value jsonData = dist_buffer.toJson();
            Json::Value jsonGeneration;
            for(const auto& s: sources){
                auto source = s.lock();
                if(!source)
                    continue;
                jsonGeneration[source->name] = source->toJson();
            }
            jsonData["generation"] = jsonGeneration;
            std::string content = convert_to_string(jsonData);
            res.set_content(content, "application/json");
        });
    if(auto bm = battery_manager.lock()){
        svr->Get(
        "/BatteryManager/status",
        [this]
        (const httplib::Request &req, httplib::Response &res)
        {
            Json::Value jsonData;
            if(const auto& bm = battery_manager.lock()){
                jsonData = bm->toJson();
                std::string content = convert_to_string(jsonData);
                res.set_content(content, "application/json");
                res.status = 200;
            }
            else{
                res.set_content("battery manager is not available", "text/plain");
                res.status = 500;
            }
        });

        for(const auto& i: bm->inverters()){
            if(auto inverter = i.lock()){
                svr->Get(
                "/BatteryManager/Inverter/"+inverter->name()+"/status",
                [this, inverter]
                (const httplib::Request &req, httplib::Response &res)
                {
                    Json::Value jsonData = inverter->toJson();
                    std::string content = convert_to_string(jsonData);
                    res.set_content(content, "application/json");
                });
            }
        }
    }
}


void PowerManager::set_use_battery_power(bool value){
    use_battery_power = value;
}

bool PowerManager::get_use_battery_power() const {
    return use_battery_power;
}
