/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/15/2020 at 03:57 PM
 * 
 * Sensor.h
 * This header file provides the libraries needed in order to read from both
 * the float and rain_gague sensors. In case the sensors cannot be read from,
 * I've provided an alternative system where values to the sensors come from
 * an random-number generator.
 * -----------------------------------------------------------------------------
 */

#ifndef CSC4110_SENSORS
#define CSC4110_SENSORS



//------------------------------------------------------------------------------
// Libraries (For alternative sensors)
//------------------------------------------------------------------------------

// As an alternative system, use RNG to simulate readings from the rain
// and float sensors. Comment it out if you can get the sensors connected
// properly.
#define SENSOR_READINGS_RNG
#ifndef SENSOR_READINGS_RNG
// Include whatever libraries needed.
    #include <cmath>
#else
// Otherwise, use the C++ Random Library
    #include <random>
#endif


//------------------------------------------------------------------------------
// Function Declarations
//------------------------------------------------------------------------------

#ifdef SENSOR_READINGS_RNG // RNG Function declarations:

int get_float_sensor_readings(std::uniform_real_distribution<int> &dist);
int get_float_sensor_readings();

double get_rain_sensor_readings(std::mt19937 &merse,
				std::uniform_real_distribution<double> &dist);
std::uniform_real_distribution<double> initialize_rain_sensor();

void initialize_float_sensor(std::mt19937 &merse,
			    std::uniform_int_distribution<int> &dist);

void initialize_rain_sensor(std::mt19937 &merse,
			    std::uniform_real_distribution<double> &dist);

bool check_rain_connection();
#else // If the sensors work, then declare the functions as necessary

double get_float_sensor_readings(void);
double get_rain_sensor_readings(void);

//------------------------------------------------------------------------------
// Testing Functions:
//------------------------------------------------------------------------------
void test_rain_guage(void);
bool rain_guage_is_connected(void);
bool float_guage_is_connected(void);
void test_float_guage(void);
#endif



//------------------------------------------------------------------------------
// Variables:
//------------------------------------------------------------------------------
#ifdef SENSOR_READINGS_RNG
// Rain amounts are in Inches
const double max_rain_amount = 50;
const double min_rain_amount = 0;
static bool rain_sensor_status = true;

// Use 1 if connected, and 0 if not. This is used to test the rest of the
// program.
const int float_sensor_status = 1;
#endif

// Specify the rain limit (in inches) of when the program should report an email.
static double rain_limit = 20;
#endif
