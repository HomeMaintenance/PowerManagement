#include "PowerSink.h"
#include <json/json.h>

PowerSink::PowerSink(std::string _name): name(_name) {}

float PowerSink::get_allowed_power() const { return allowed_power; }

void PowerSink::set_allowed_power(float power) { allowed_power = power; }

// bool PowerSink::allow_power(float power) {
//     static_assert(false, "You should override this");
//     set_allowed_power(power);
//     return true;
// }

PowerRange PowerSink::get_requesting_power() const {
    return requesting_power;
}

void PowerSink::set_requesting_power(const PowerRange& range){
    requesting_power = range;
}

void PowerSink::set_requesting_power(PowerRange& range){
    requesting_power = range;
}

void PowerSink::set_requesting_power(float min, float max){
    requesting_power = PowerRange(min, max);
}

Json::Value PowerSink::serialize(){
    Json::Value jsonData;
    jsonData["using_power"] = using_power();
    jsonData["allowed_power"] = get_allowed_power();

    Json::Value req_pwr;
    req_pwr["min"] = requesting_power.get_min();
    req_pwr["max"] = requesting_power.get_max();
    jsonData["requesting_power"] = req_pwr;
    return jsonData;
}

std::string convert_to_string(const Json::Value& json);
void PowerSink::register_http_server_functions(httplib::Server* svr){
    svr->Get(
        "/sink/"+name+"/status",
        [this]
        (const httplib::Request &req, httplib::Response &res)
        {
            Json::Value jsonData;
            jsonData["using_power"] = using_power();
        });
}
