#ifndef ONEPOLECONTROL_H
#define ONEPOLECONTROL_H

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/core/types.hpp>

class onePoleControl {
  public:
    onePoleControl(double Kp, double max, double min);
    ~onePoleControl();
    std::pair<int, double> calculateNext(const cv::Point2d& aim,
                                         const cv::Point2d& location);
    void updateParams(double Kp, double max, double min);

  private:
    double _Kp;
    double _max;
    double _min;
    bool goDown;
    int direction;
};

#endif // ONEPOLECONTROL_H
