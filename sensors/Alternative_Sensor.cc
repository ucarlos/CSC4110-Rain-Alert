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

#include "./Alternative_Sensor.h"

//--------------------------------------------------------------------------------
// Functions for the Alternative System
//--------------------------------------------------------------------------------

#ifdef SENSOR_READINGS_RNG



/**
 * Determines whether the simulated sensor is connected. Use it to test how the
 * system works when not connected/connected.
 */
bool check_rain_connection(){ return rain_sensor_status; }



/**
 * Creates a rain_distribution that gets random double values for the rain sensor.
 * @param merse A Mersenne Twister RNG to simulate sensor values.
 * @param dist A rain_distribution of real numbers to be used by merse.
 * 
 */
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

/**
 * Get a random value from a range of [Min_Rain_Amount : Max_Rain_Amount]
 * @param merse A Mersenne Twister RNG to simulate sensor values.
 * @param dist A rain_distribution of real numbers to be used by merse.
 */
double get_rain_sensor_readings(std::mt19937 &merse,
				std::uniform_real_distribution<double> &dist){
    return dist(merse);

}



/**
 * Creates a rain_distribution that get random int values for the float sensor (If
needed).
 * @param merse A Mersenne Twister RNG to simulate sensor values.
 * @param dist A rain_distribution of integer numbers to be used by merse.
 */
void initialize_float_sensor(std::mt19937 &merse,
			     std::uniform_int_distribution<int> &dist){

    std::random_device rand;
    merse = std::mt19937(rand());
    dist = std::uniform_int_distribution<int>{0, 2};

}
			     
/**
 * Returns the value of float_sensor_status as an integer. Use it if you just want the component to work/not work.
 */
int get_float_sensor_readings(){
    return float_sensor_status;
}

/**
 * Just returns a value between 0 and 1. Useful if you want to simulate a
 * sensor that works half of the time.
 * @param merse A Mersenne Twister RNG to simulate sensor values.
 * @param dist A rain_distribution of integer numbers to be used by merse.
 */
int get_float_sensor_readings(std::mt19937 &merse,
			      std::uniform_int_distribution<int> &dist){
    return dist(merse);

}

#endif

