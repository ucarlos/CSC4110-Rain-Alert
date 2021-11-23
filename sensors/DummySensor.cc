/*
 * ------------------------------------------------------------------------------
 * Created by Ulysses Carlos on 11/23/2021 at 12:14 AM
 *
 * DummySensor.cpp
 *
 * ------------------------------------------------------------------------------
 */

#include "DummySensor.h"

DummySensor::DummySensor(const double min_rain_inches,
						 const double max_rain_inches) {

	this->min_rain_inches = min_rain_inches;
	this->max_rain_inches = max_rain_inches;

	// Now set up rain_distribution:
	regenerate_rain_distribution();
}

DummySensor::DummySensor(double min_rain_inches,
                         double max_rain_inches,
                         int32_t min_float_level,
                         int32_t max_float_level ) : DummySensor(min_rain_inches, max_rain_inches){
    this->min_float_value = min_float_level;
    this->max_float_value = max_float_level;

    // Now set up float distribution:
    regenerate_float_distribution();
}

double DummySensor::read_from_rain_sensor()  {
    auto& debug_file = DebugLog::instance();
    if (is_rain_sensor_active()) {
        static double temp_value = rain_distribution(merse);
        //debug_file << DebugLevel::INFO << "Reading from Dummy Sensor: " << temp_value << " inches" << std::endl;
        set_current_rain_level_in_inches(temp_value);
        return temp_value;
    }
    else {
        // debug_file << DebugLevel::WARNING << "Could not read from Dummy Sensor; Returning " << FAILED_READ << std::endl;
        return FAILED_READ;
    }
}

int32_t DummySensor::read_from_float_sensor() {
    auto& debug_file = DebugLog::instance();
    if (is_float_sensor_active()) {
        static int32_t temp_value = float_distribution(merse);
        set_current_float_level_in_inches(temp_value);
        return temp_value;
    }
    else
        return FAILED_READ;
}



