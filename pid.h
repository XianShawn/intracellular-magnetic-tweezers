#ifndef PID_H
#define PID_H

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/core/types.hpp>

class PID {
  public:
    PID(double dt, double max, double min, double Kp, double Ki, double Kd);
    ~PID();

    /**
     * Get output voltages given reference and input
     * @brief calculateNext
     * @param aim
     * @param location
     * @return
     */
    std::pair<double, double> calculateNext(const cv::Point2d& aim,
                                            const cv::Point2d& location);
    void updateParams(double dt, double max, double min, double Kp, double Ki,
                      double Kd);
    void reset();

  private:
    double _dt;
    double _max;
    double _min;
    double _Kp;
    double _Ki;
    double _Kd;
    cv::Point2d _last_error;
    cv::Point2d _integral;
};

#endif // PID_H
