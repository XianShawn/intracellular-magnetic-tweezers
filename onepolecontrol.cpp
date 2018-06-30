#include "onepolecontrol.h"

onePoleControl::onePoleControl(double Kp, double max, double min)
    : _Kp(Kp), _max(max), _min(min) {
    goDown = true;
}

onePoleControl::~onePoleControl() {
}

// x > 0 if it's to the left of the target
// y > 0 when it's above the target
std::pair<int, double>
onePoleControl::calculateNext(const cv::Point2d& aim,
                              const cv::Point2d& location) {
    double returnVoltage = 0;
    if (abs(aim.x - location.x) >
        abs(aim.y - location.y)) { // Go in the x direction
        if ((aim.x - location.x) > 0) {
            returnVoltage = aim.x - location.x;
            direction = 1;
        } else {
            returnVoltage = location.x - aim.x;
            direction = 0;
        }
    } else if (abs(aim.x - location.x) <
               abs(aim.y - location.y)) { // Go in the y direction
        if ((aim.y - location.y) > 0) {
            returnVoltage = aim.y - location.y;
            direction = 3;
        } else {
            returnVoltage = location.y - aim.y;
            direction = 2;
        }
    } else if (abs(aim.x - location.x) ==
               abs(aim.y - location.y)) { // Go in either direction
        goDown = !goDown;
        if (goDown) {
            if ((aim.y - location.y) > 0) {
                returnVoltage = aim.y - location.y;
                direction = 3;
            } else {
                returnVoltage = location.y - aim.y;
                direction = 2;
            }
        } else {
            if ((aim.x - location.x) > 0) {
                returnVoltage = aim.x - location.x;
                direction = 1;
            } else {
                returnVoltage = location.x - aim.x;
                direction = 0;
            }
        }
    }

    returnVoltage *= _Kp;

    //  Restrict to a limit if out of bounds
    if (returnVoltage > _max)
        returnVoltage = _max;
    else if (returnVoltage < _min)
        returnVoltage = _min;

    return std::make_pair(direction, returnVoltage);
}

void onePoleControl::updateParams(double Kp, double max, double min) {
    _Kp = Kp;
    _max = max;
    _min = min;
}
