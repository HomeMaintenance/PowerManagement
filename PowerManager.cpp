#include "PowerManager.h"
#include <cassert>

void PowerManager::DistributionResult::set_power(float pwr)
{
    assert(!(_set&0x1));
    _power = pwr;
    _set |= 0x1;
}

void PowerManager::DistributionResult::set_remaining(float remaining)
{
    assert(!(_set&0x2));
    _remaining = remaining;
    _set |= 0x2;
}

void PowerManager::DistributionResult::set_used(float used)
{
    assert(!(_set&0x4));
    _used = used;
    _set |= 0x4;
}

void PowerManager::DistributionResult::set_distribution(std::unordered_map<std::string, float>& distribution)
{
    assert(!(_set&0x8));
    _distribution = distribution;
    _set |= 0x8;
}

bool PowerManager::DistributionResult::validate() const{
    return _set == 0xF;
}

float PowerManager::DistributionResult::get_power() const
{
    return _power;
}

float PowerManager::DistributionResult::get_remaining() const
{
    return _remaining;
}

float PowerManager::DistributionResult::get_used() const
{
    return _used;
}

std::unordered_map<std::string, float> PowerManager::DistributionResult::get_distribution() const
{
    return _distribution;
}

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

void PowerManager::set_power_buffer(float buffer){
    power_buffer = buffer;
}

float PowerManager::get_power_buffer() const{
    return power_buffer;
}

void PowerManager::set_power_grid(std::weak_ptr<PowerGrid> grid){
    power_grid = grid;
}

PowerManager::DistributionResult PowerManager::distribute(){
    DistributionResult result;

    float _power_grid = 0.f;
    if(auto pwr_grd = power_grid.lock()){
        // Use power grid as reference when set
        log("Power grid is reference");
        _power_grid = pwr_grd->get_power();
    }

    float _available_power_grid = 0.f;
    float _available_power_sources = available_power();
    bool grid_ref = _power_from == PowerFrom::Grid;
    if(grid_ref){
        dist_buffer.grid = _power_grid;
        dist_buffer.available = 0;
        for(const auto& s: sinks){
            // Needed because otherwise the already turned on devices would be ignored
            auto sink = s.lock();
            if(!sink)
                continue;
            _available_power_grid += sink->using_power();
        }
    }
    else{
        // Use power from inverters in sources if power_grid is not set
        log("Sources are reference");
        dist_buffer.grid = 0;
        dist_buffer.available = _available_power_sources;
    }

    float _available_power = grid_ref ? _available_power_grid : _available_power_sources;

    // Get available power from battery
    float battery_power = 0.f;
    float battery_soc = 0.f;
    float battery_discharge = 0.f;
    auto _battery_manager = battery_manager.lock();
    if(_battery_manager){
        battery_soc = _battery_manager->soc();
        battery_power = _battery_manager->available_power();
        battery_discharge = _battery_manager->present_discharge();
    }
    dist_buffer.battery_soc = battery_soc;
    dist_buffer.battery_power = battery_power;
    dist_buffer.battery_discharge = battery_discharge;
    log("Power available from sources: " + std::to_string(_available_power_sources));
    log("Power available from grid: " + std::to_string(_available_power_grid));
    log("Power available: " + std::to_string(_available_power));
    log("Power from Battery: " + std::to_string(battery_power));
    log("Power from Grid: " + std::to_string(_power_grid));

    // Calculate available power from sources, battery and grid
    float power_wo_buffer = _available_power + battery_power + _power_grid;
    log("Power available: " + std::to_string(power_wo_buffer));
    float power = power_wo_buffer - power_buffer;
    result.set_power(power);
    log("Power available with buffer: " + std::to_string(power));
    dist_buffer.power = power;
    dist_buffer.buffer = power_buffer;

    std::unordered_map<std::string, float> _pwr_dist;

    float power_used = 0.f;

    for(const auto& s: sinks){
        auto sink = s.lock();
        if(!sink)
            continue;
        float grant_power = power;
        const PowerRange sink_requesting_power = sink->get_requesting_power();
        if(grant_power >= sink_requesting_power.get_min() && sink_requesting_power.get_max() > 0){
            if(grant_power > sink_requesting_power.get_max())
                grant_power = sink_requesting_power.get_max();
            log("\tGranting " + sink->name + " " + std::to_string(grant_power) + " W power");
            if( sink->allow_power(grant_power) ){
                // success
                log("\t\taccepted");
            }
            else{
                // failed to accept power
                log("\t\trejected");
            }
        }
        else{
            log("\tSwitching "+ sink->name + " off");
            sink->allow_power(0); // switch off
        }
        const float _allowed_power = sink->get_allowed_power();
        power_used += _allowed_power;
        _pwr_dist[sink->name] = _allowed_power;
        power -= _allowed_power;
    }
    result.set_used(power_used);
    result.set_distribution(_pwr_dist);
    power_distribution = _pwr_dist;
    dist_buffer.distribution = power_distribution;
    result.set_remaining(power);
    dist_buffer.remaining = power;
    log("remaining Power: " + std::to_string(power));
    assert(result.validate());
    return result; // return remaining power
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

void PowerManager::use_power_from_sources(){
    _power_from = PowerFrom::Sources;
}

void PowerManager::use_power_from_grid(){
    _power_from = PowerFrom::Grid;
}

void PowerManager::enable_log(){
    _enable_log = true;
}

void PowerManager::disable_log(){
    _enable_log = false;
}

void PowerManager::log(std::string message) const
{
    if(_enable_log)
        std::cout << "-- Powermanager: " << message << std::endl;
}
