#include "pid.h"

// First index: X coordinates
// Second index: Y coordinates
PID::PID(double dt, double max, double min, double Kp, double Ki, double Kd)
    : _dt(dt), _max(max), _min(min), _Kp(Kp), _Ki(Ki), _Kd(Kd) {
    _last_error.x = 0, _last_error.y = 0, _integral.x = 0, _integral.y = 0;
}

PID::~PID() {
}

std::pair<double, double> PID::calculateNext(const cv::Point2d& aim,
                                             const cv::Point2d& location) {
    //  Error:
    //  static std::pair < double, double > error_history[5];
    std::pair<double, double> sum_error;
    std::pair<double, double> new_error;
    new_error.first = aim.x - location.x;
    new_error.second = aim.y - location.y;

    sum_error.first = new_error.first, sum_error.second = new_error.second;

    //  Proportional term:
    std::pair<double, double> Pout(_Kp * sum_error.first,
                                   _Kp * sum_error.second);

    //  std::cout << "Pout: (" << Pout.first << ", " << Pout.second << ")" <<
    //  std::endl;

    //  Integral term:
    _integral.x += sum_error.first * _dt;
    _integral.y += sum_error.second * _dt;

    std::pair<double, double> Iout(_Ki * _integral.x, _Ki * _integral.y);

    //  std::cout << "Iout: (" << Iout.first << ", " << Iout.second << ")" <<
    //  std::endl;

    //  Derivative term:
    std::pair<double, double> derivative(
        (new_error.first - sum_error.first) * _dt,
        (new_error.second - sum_error.second) * _dt);
    std::pair<double, double> Dout(_Kd * derivative.first,
                                   _Kd * derivative.second);

    //  std::cout << "Dout: (" << Dout.first << ", " << Dout.second << ")" <<
    //  std::endl;

    //  Calculate total output
    std::pair<double, double> output(Pout.first + Iout.first + Dout.first,
                                     Pout.second + Iout.second + Dout.second);

    //  Restrict to a limit if out of bounds
    if (output.first > _max)
        output.first = _max;
    else if (output.first < _min)
        output.first = _min;
    if (output.second > _max)
        output.second = _max;
    else if (output.second < _min)
        output.second = _min;

    //  Save error to previous error
    //  for (int i = 0; i < 4; i++) error_history[i] = error_history[i+1];
    //  error_history[4] = new_error;

    return output;
}

void PID::reset() {
    _last_error.x = 0, _last_error.y = 0;
    _integral.x = 0, _integral.y = 0;
}

void PID::updateParams(double dt, double max, double min, double Kp, double Ki,
                       double Kd) {
    _dt = dt;
    _max = max;
    _min = min;
    _Kp = Kp;
    _Ki = Ki;
    _Kd = Kd;
    std::cout << "PID parameters updated" << std::endl;
}
