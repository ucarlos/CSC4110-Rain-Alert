/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 03/13/2020
 * 
 * db_populate.sql
 * This file should be imported after csc4110_project.sql
 * to create the first log in the table.
 * -----------------------------------------------------------------------------
 */

insert into log (rain_sensor_connected, float_sensor_connected, solar_panel_functional,
                 battery_connected, rain_level, log_comment)
values (false, false, false, false, null, 'This is a dummy log that will always be added to this table. Please ignore it.');
