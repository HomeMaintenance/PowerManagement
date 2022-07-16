#pragma once
#include <string>
#include <limits>
#include <httplib.h>
#include <json/json.h>
#include "PowerRange.h"

/**
 * @brief Used to add a sink to the #PowerManager
 *
 */
class PowerSink{
public:
    PowerSink(std::string name);

    const std::string name;

    static const std::string type;

    /**
     * @brief Get the power actually used by the sink
     *
     * @return float
     */
    virtual float using_power() = 0;

    /**
     * @brief Get the allowed power of the sink
     *
     * @return float
     */
    float get_allowed_power() const;
    void set_allowed_power(float power);

    /**
     * @brief Allow the sink to use some power
     *
     * @param power Amount of power the sink can use
     * @return true Sink accepts the power
     * @return false Sink does not accept the power
     */
    virtual bool allow_power(float power) = 0;

    /**
     * @brief Get the requesting power object
     *
     * @return PowerRange
     */
    PowerRange get_requesting_power() const;

    virtual void register_http_server_functions(httplib::Server* svr);

    virtual Json::Value serialize();

protected:
    /**
     * @brief Amount of power the sink is requesting
     *
     * This can be used to specify min and max power the sink can use.
     * By default the sinks requested min is 0 and max is inf
     *
     */
    virtual void set_requesting_power(const PowerRange& range);
    virtual void set_requesting_power(float min, float max);


private:
    PowerRange requesting_power = PowerRange(0,(std::numeric_limits<float>::max)());
    float allowed_power{0.f};
};
