#ifndef PREDICTIVECONTROLLER_H
#define PREDICTIVECONTROLLER_H

#include "pid.h"

#include <iostream>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/core/types.hpp>

class predictiveController : public PID {
  public:
    predictiveController(double dt, double max, double min, double Kp,
                         double Ki, double Kd, double N, double FPS,
                         double decrease_factor);
    ~predictiveController();

    std::pair<double, double> calculateNext(const cv::Point2d& aim,
                                            const cv::Point2d& location);

    void updateControllerParams(double dt, double max, double min, double Kp,
                                double Ki, double Kd, double N, double FPS,
                                double decrease_factor);

  private:
    double _N_samples;
    double _counter;
    double _sub_counter;
    double _step_size;
    double _decrease_factor;
    std::pair<double, double> _last_voltage;
    std::pair<double, double> calculated_voltage;

    void increaseCounter();
};

#endif // PREDICTIVECONTROLLER_H
