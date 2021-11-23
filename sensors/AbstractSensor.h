/*
 * ------------------------------------------------------------------------------
 * Created by Ulysses Carlos on 11/23/2021 at 12:30 PM
 *
 * AbstractSensor.h
 *
 * ------------------------------------------------------------------------------
 */

#ifndef CSC4110_PROJECT_ABSTRACTSENSOR_H
#define CSC4110_PROJECT_ABSTRACTSENSOR_H
#include <cstdint>
#include "../DebugLog.h"

const double RAIN_LIMIT_IN_INCHES = 20;

class AbstractSensor {
public:
	virtual double read_from_rain_sensor()   =0;
	virtual int32_t read_from_float_sensor()  =0;

	[[nodiscard]] virtual bool is_rain_sensor_active()  const =0;
	[[nodiscard]] virtual bool is_float_sensor_active() const =0;

    [[nodiscard]] virtual double get_current_rain_level_in_inches() const =0;
    [[nodiscard]] virtual double get_current_float_level_in_inches() const = 0;

    [[nodiscard]] double get_max_rain_threshold_in_inches() const { return rain_threshold_limit_in_inches; }
    void set_max_rain_threshold_in_inches(double new_val) { rain_threshold_limit_in_inches = new_val; }

protected:
	AbstractSensor() =default;
    constexpr static double INCH_TO_CM{2.54};

	static double inch_to_cm(double inches) { return INCH_TO_CM * inches; }
	static double cm_to_inch(double cm) { return cm / INCH_TO_CM; }

    virtual void set_current_rain_level_in_inches(double new_val)   = 0;
    virtual void set_current_float_level_in_inches(int32_t new_val) = 0;


private:
    double current_rain_level_in_inches{};
    double rain_threshold_limit_in_inches{RAIN_LIMIT_IN_INCHES};
    double current_float_level_in_inches{};
};


#endif //CSC4110_PROJECT_ABSTRACTSENSOR_H
