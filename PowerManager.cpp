#include "PowerManager.h"

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

std::unordered_map<std::string, float> PowerManager::get_power_distribution() const {
    return {};
}

void PowerManager::set_power_grid(float (*function)()){
    // Function to calulate the power from the grid.
    // Positive if more power is drawn than generated.
    // If more power is generated set to negative value.
    power_grid = function;
}

float PowerManager::distribute(){
    float power = available_power() - power_grid() - power_buffer;

    for(const auto& s: sinks){
        auto sink = s.lock();
        if(!sink)
            continue;
        float grant_power = power;
        PowerRange sink_requesting_power = sink->get_requesting_power();
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
        power_distribution[sink->name] = sink->using_power();
        power -= sink->using_power();
    }
    return power; // return remaining power
}

float PowerManager::available_power(){
    float result = 0.f;
    for(const auto& s: sources){
        if(auto source = s.lock()){
            result += source->get_available_power();
        }
    }
    return result;
}
