/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 04/22/2020 at 03:24 PM
 * 
 * Sensor_Tracking.cc
 * 
 * I've separated all the functions related to the sensor tracking into
 * a single c++ file.
 * -----------------------------------------------------------------------------
 */

#include "./Project.h"


//------------------------------------------------------------------------------
// Tracking() : Enables / Disable Sensor Tracking
// When Tracking is enabled, Make a pthread that handles reading the values
// Of the Float Sensor/Rain Sensor. When an error occurs or when it is time
// To send a daily report, send an email.
// When Tracking is disabled, close the pthread.
// TODO: Allow both threads to run in the background.
//------------------------------------------------------------------------------
void sensor_tracking(void) {
    cout << options[0] << endl;
    
    /*
     * Create two pthreads:
     * One to handle reading from sensor
     * One to handle sending mail at a specfic rate.
     */


    
    return_to_menu();
}
