/*
 * ------------------------------------------------------------------------------
 * Created by Ulysses Carlos on 11/23/2021
 *
 * RealSensor.cc
 *
 * ------------------------------------------------------------------------------
 */

#include "RealSensor.h"

double RealSensor::read_from_rain_sensor() {
    return 0;
}

int32_t RealSensor::read_from_float_sensor() {
    return 0;
}

bool RealSensor::is_rain_sensor_active() const {
    return false;
}

bool RealSensor::is_float_sensor_active() const {
    return false;
}

double RealSensor::get_current_rain_level_in_inches() const {
    return 0;
}

double RealSensor::get_current_float_level_in_inches() const {
    return 0;
}
