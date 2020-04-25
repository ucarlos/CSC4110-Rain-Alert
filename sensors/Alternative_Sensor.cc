/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 03/05/2020
 * 
 * Alternative_Sensor.cc
 * This file provides random output that can be read by the sensor tracking
 * functions in Basic_Menu/Menu.cc. This is a system of last resort that 
 * should provide the needed functionality.
 * -----------------------------------------------------------------------------
 */

#include "./Sensor.h"

//--------------------------------------------------------------------------------
// Functions for the Alternative System
//--------------------------------------------------------------------------------

#ifdef SENSOR_READINGS_RNG


//------------------------------------------------------------------------------
// check_rain_connection() : Simulates whether the sensor is connected.
// Use it to test how the system works when not connected/connected.
//------------------------------------------------------------------------------

bool check_rain_connection(){ return rain_sensor_status; }


//------------------------------------------------------------------------------
// Initialize_rain_sensor(): Creates a distribution that gets random double
// values for the rain sensor.
//------------------------------------------------------------------------------

void initialize_rain_sensor(std::mt19937 &merse,
			    std::uniform_real_distribution<double> &dist){
    
    std::random_device rand;
    // Using Mersenne Twiser as RNG.
    merse = std::mt19937(rand());
    
    dist = std::uniform_real_distribution<double>{
	(min_rain_amount, max_rain_amount)};
    

}

//------------------------------------------------------------------------------
// get_rain_sensor_readings(): Get a random value from 2: 80
//------------------------------------------------------------------------------

double get_rain_sensor_readings(std::mt19937 &merse,
				std::uniform_real_distribution<double> &dist){
    return dist(merse);

}

//------------------------------------------------------------------------------
// initialize_float_sensor(): Creates a distribution that get random int
// values for the float sensor (If needed).
//------------------------------------------------------------------------------

void initialize_float_sensor(std::mt19937 &merse,
			     std::uniform_int_distribution<int> &dist){

    std::random_device rand;
    merse = std::mt19937(rand());
    dist = std::uniform_int_distribution<int>{0, 2};

}
			     
//------------------------------------------------------------------------------
// get_float_sensor_readings(): Two functions that get psuedo output.
// get_float_sensor_readings(void) just returns the value of
// float_sensor_status. Use it if you just want the component to work/not work
//
// int get_float_sensor_readings(merse, distribution): Just returns a value
// between 0 and 1. Useful if you want to simulate a sensor that works half
// of the time. 
//------------------------------------------------------------------------------

int get_float_sensor_readings(void){
    return float_sensor_status;
}

int get_float_sensor_readings(std::mt19937 &merse,
			      std::uniform_int_distribution<int> &dist){
    return dist(merse);

}

#endif

