/*
 * ------------------------------------------------------------------------------
 * Created by Ulysses Carlos on 11/23/2021 at 12:14 AM
 *
 * DummySensor.h
 *
 * ------------------------------------------------------------------------------
 */


#ifndef CSC4110_PROJECT_DUMMYSENSOR_H
#define CSC4110_PROJECT_DUMMYSENSOR_H
#include "AbstractSensor.h"
#include <random>


using RealDistribution = std::uniform_real_distribution<double>;
using IntDistribution = std::uniform_int_distribution<int32_t>;

class DummySensor : public AbstractSensor {
public:
    DummySensor() =default;
    explicit DummySensor(const std::mt19937 _merse, RealDistribution dist) :
        merse{_merse}, rain_distribution{dist} {  }

	DummySensor(double min_rain_inches, double max_rain_inches);

    DummySensor(double min_rain_inches, double max_rain_inches, int32_t min_float_level, int32_t max_float_level);

    DummySensor(const std::mt19937 _merse,
                const RealDistribution _dist,
                const double _min_rain,
                const double _max_rain) :
        merse{_merse}, rain_distribution{_dist},
        min_rain_inches{_min_rain}, max_rain_inches{_max_rain} { regenerate_rain_distribution();}

    double read_from_rain_sensor() override;
    int32_t read_from_float_sensor() override;

    [[nodiscard]] double get_max_rain_in_inches() const { return max_rain_inches; }
    void set_max_rain_in_inches(double new_val) {
		max_rain_inches = new_val;
        regenerate_rain_distribution();
	}

    static DummySensor& instance() {
        static DummySensor dummy_sensor{};
        return dummy_sensor;
    }

    [[nodiscard]] double get_max_rain_in_cm() const { return inch_to_cm(max_rain_inches); }


    [[nodiscard]] double get_min_rain_in_inches() const { return min_rain_inches; }
    [[nodiscard]] double get_min_rain_in_cm() const { return inch_to_cm(min_rain_inches); }
    void set_min_rain_in_inches(double new_val) {
		min_rain_inches = new_val;
        regenerate_rain_distribution();
	}

    [[nodiscard]] bool is_rain_sensor_active() const override { return active_rain_sensor; }
    void disable_rain_sensor() { active_rain_sensor = false; }
    void enable_rain_sensor() { active_rain_sensor = true; }

    [[nodiscard]] bool is_float_sensor_active() const override {return active_float_sensor; }
    void disable_float_sensor() { active_float_sensor = false; }
    void enable_float_sensor() { active_float_sensor = true; }

    [[nodiscard]] double get_current_rain_level_in_inches() const override { return current_rain_level_in_inches; }
    void set_current_rain_level_in_inches(double new_val) override{ current_rain_level_in_inches = new_val; }

    [[nodiscard]] double get_current_float_level_in_inches() const override { return current_float_level_in_inches; }
    void set_current_float_level_in_inches(int32_t new_val) override { current_float_level_in_inches = new_val; }

private:
    double max_rain_inches{25};
    double min_rain_inches{1};

    double battery_level_percentage{100};
    double current_rain_level_in_inches{};
    double current_float_level_in_inches{};

    std::random_device random_device{};
    std::mt19937 merse{random_device()};
    RealDistribution rain_distribution{min_rain_inches, max_rain_inches};
    IntDistribution float_distribution{};

    int32_t min_float_value{0};
    int32_t max_float_value{2};

    bool active_float_sensor{true};
    bool active_rain_sensor{true};

	const static int FAILED_READ{-1};
	void regenerate_rain_distribution() { rain_distribution = RealDistribution{min_rain_inches, max_rain_inches}; }
    void regenerate_float_distribution() { float_distribution = IntDistribution{min_float_value, max_float_value}; }
};


#endif //CSC4110_PROJECT_DUMMYSENSOR_H
