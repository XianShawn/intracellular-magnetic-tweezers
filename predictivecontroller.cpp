#include "predictivecontroller.h"

predictiveController::predictiveController(double dt, double max, double min,
                                           double Kp, double Ki, double Kd,
                                           double N, double FPS,
                                           double decrease_factor)
    : PID(dt, max, min, Kp, Ki, Kd) {
    _N_samples = N;
    _counter = 0;
    _sub_counter = 0;
    _step_size = floor(((1 / FPS) * 10) / N);
    _decrease_factor = decrease_factor;

    _last_voltage.first = 0;
    _last_voltage.second = 0;
}

predictiveController::~predictiveController() {
}

std::pair<double, double>
predictiveController::calculateNext(const cv::Point2d& aim,
                                    const cv::Point2d& location) {
    std::pair<double, double> return_voltage;
    calculated_voltage = PID::calculateNext(aim, location);

    double voltage_decrease1 =
        calculated_voltage.first * _decrease_factor * _counter;
    if (fabs(voltage_decrease1) > fabs(calculated_voltage.first))
        return_voltage.first = 0;
    else
        return_voltage.first = calculated_voltage.first - voltage_decrease1;

    double voltage_decrease2 =
        calculated_voltage.second * _decrease_factor * _counter;
    if (fabs(voltage_decrease2) > fabs(calculated_voltage.second))
        return_voltage.second = 0;
    else
        return_voltage.second = calculated_voltage.second - voltage_decrease2;

    increaseCounter();

    _last_voltage = calculated_voltage;

    return return_voltage;
}

void predictiveController::increaseCounter() {
    if ((_counter == (_N_samples - 1)) && (_sub_counter == (_step_size - 1))) {
        _counter = 0, _sub_counter = 0;
        return;
    }
    if (_sub_counter == (_step_size - 1)) {
        _counter++, _sub_counter = 0;
        return;
    }
    if ((calculated_voltage.first != _last_voltage.first) ||
        (calculated_voltage.second != _last_voltage.second)) {
        _counter = 0, _sub_counter = 0;
        return;
    }
    _sub_counter++;
}

void predictiveController::updateControllerParams(double dt, double max,
                                                  double min, double Kp,
                                                  double Ki, double Kd,
                                                  double N, double FPS,
                                                  double decrease_factor) {
    PID::updateParams(dt, max, min, Kp, Ki, Kd);
    _N_samples = N;
    _step_size = floor(((1 / FPS) * 10) / N);
    _counter = 0, _sub_counter = 0;
    _decrease_factor = decrease_factor;
    std::cout << "Predictive controller parameters updated" << std::endl;
}
