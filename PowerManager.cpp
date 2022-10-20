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

float sumUp(const std::unordered_map<std::string, float>& power_map){
    float result = 0;
    std::for_each(power_map.begin(), power_map.end(),
        [&result](const std::pair<std::string, float> &n) mutable
        {
            result += n.second;
        }
    );
    return result;
}

float sumUp(const std::vector<float>& power_vector){
    float result = 0;
    std::for_each(power_vector.begin(), power_vector.end(),
        [&result](const float &n) mutable
        {
            result += n;
        }
    );
    return result;
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

std::unordered_map<std::string, float> PowerManager::get_power_generation() const {
    return power_generation;
}

float PowerManager::get_grid_power() const {
    return grid_power_value;
}

void PowerManager::set_power_grid(std::weak_ptr<PowerGrid> grid){
    power_grid = grid;
    use_power_from_grid();
}

PowerManager::DistributionResult PowerManager::distribute(){
    DistributionResult result;

    power_generation = available_power();
    const float _available_power_sources = sumUp(power_generation);

    if(auto pwr_grd = power_grid.lock()){
        grid_power_value = pwr_grd->get_power();
    }
    dist_buffer.grid = grid_power_value;



    float power_grid_and_sinks = grid_power_value;
    const bool grid_ref = _power_from == PowerFrom::Grid;
    dist_buffer.power_reference = get_power_reference_str();
    if(grid_ref){
        log("Power grid is reference");
        float _already_used_by_sinks = 0.f;
        for(const auto& s: sinks){
            // Needed because otherwise the already turned on devices would be ignored
            if(auto sink = s.lock()){
                _already_used_by_sinks += sink->using_power();
            }
        }
        power_grid_and_sinks += _already_used_by_sinks;
        log("already used by sinks: " + std::to_string(_already_used_by_sinks) + " W");
    }
    else{
        // Use power from inverters in sources if power_grid is not set
        log("Sources are reference");
    }

    const float _available_power = grid_ref ? power_grid_and_sinks : _available_power_sources;
    dist_buffer.available = _available_power;

    // Get available power from battery
    float battery_power = 0.f;
    float battery_soc = 0.f;
    float battery_discharge = 0.f;
    auto _battery_manager = battery_manager.lock();
    if(_battery_manager){
        battery_power = _battery_manager->available_power();
        battery_soc = _battery_manager->soc();
        battery_discharge = _battery_manager->present_discharge();
    }
    dist_buffer.battery_power = battery_power;
    dist_buffer.battery_soc = battery_soc;
    dist_buffer.battery_discharge = battery_discharge;
    log("Power available from sources: " + std::to_string(_available_power_sources));
    log("Power available from grid: " + std::to_string(power_grid_and_sinks));
    log("Power available: " + std::to_string(_available_power));
    log("Power from Battery: " + std::to_string(battery_power));
    log("Power from Grid: " + std::to_string(grid_power_value));

    // Calculate available power from sources, battery and grid
    float power_wo_buffer = _available_power + battery_power + grid_power_value;
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

std::unordered_map<std::string, float> PowerManager::available_power(){
    std::unordered_map<std::string, float> result{};

    for(const auto& s: sources){
        if(auto source = s.lock()){
            const auto source_power = source->get_available_power();
            result.emplace(source->name, source_power);
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
            jsonData["power_reference"] = get_power_reference_str();
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

std::string PowerManager::get_power_reference_str() const {
    return _powerFrom_LUT[static_cast<int>(_power_from)];
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
