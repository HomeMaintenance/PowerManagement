#include "PowerRange.h"
#include <sstream>

PowerRange::PowerRange(float _min, float _max): min(_min), max(_max) {}

void PowerRange::set_min(float value){
    if(value > max)
        min = max;
    else if(value < 0)
        min = 0.f;
    else
        min = value;
}

float PowerRange::get_min() const { return min; }

void PowerRange::set_max(float value){
    if(value > min) {
        max = value;
    }
    else{
        max = min;
    }
}

float PowerRange::get_max() const { return max; }

bool PowerRange::contains(const PowerRange& other) const {
    bool result = true;
    result &= min <= other.min;
    result &= max >= other.max;
    return result;
}

void PowerRange::floor(){
    max = min;
}

void PowerRange::ceil(){
    min = max;
}

void PowerRange::mean(){
    float avg = (max + min)/2;
    max = avg;
    min = avg;
}


std::string PowerRange::to_string() const{
    std::stringstream result;
    result << "{\"min\":" << min << ", \"max\":" << max << "}";
    return result.str();
}
