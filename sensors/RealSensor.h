//--------------------------------------------------------------------------------------------------
// Created by Ulysses Carlos on 11/23/21 using Ulysses-H270-HD3.
//--------------------------------------------------------------------------------------------------


#ifndef CSC4110_PROJECT_REALSENSOR_H
#define CSC4110_PROJECT_REALSENSOR_H
#include "AbstractSensor.h"

class RealSensor : public AbstractSensor {
public:
    double read_from_rain_sensor() override;
    int32_t read_from_float_sensor() override;

    bool is_rain_sensor_active()  const override;
    [[nodiscard]] bool is_float_sensor_active() const override;

    [[nodiscard]] double get_current_rain_level_in_inches() override;
    [[nodiscard]] double get_current_float_level_in_inches() override;
private:
};


#endif //CSC4110_PROJECT_REALSENSOR_H
