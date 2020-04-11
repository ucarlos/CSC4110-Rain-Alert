/*
 * -----------------------------------------------------------------------------
 * Created by Ulysses Carlos on 03/13/2020
 * 
 * csc4110_project
 * This contains the information needed to structure the database
 * used for the project. 
 *
 *
 * -----------------------------------------------------------------------------
 */


create database csc4110_project;


/*
 * For the rain level column, we need to determine whether the units are in 
 * inches/centimeters, and if so, the ability to convert between them should
 * be done by the program. The battery level can be null
 */
 
create table if not exists log(
    id bigserial not null,
    log_date date default current_date,
    log_time time(0) with time zone default current_time,
    log_timestamp timestamp with time zone default current_timestamp,
    float_sensor_connected boolean not null,
    rain_sensor_connected boolean not null,
    solar_panel_functional boolean not null,
    battery_connected boolean not null,
    battery_level real null,
    rain_level real null,
    log_comment text not null,
    primary key (id)
);
	
