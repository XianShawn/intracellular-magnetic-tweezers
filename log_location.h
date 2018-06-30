#ifndef LOG_LOCATION_H
#define LOG_LOCATION_H

#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
#include <chrono>
#include <opencv2/core/types.hpp>

extern double pid_voltage_offset;
extern double opc_voltage_offset;

class log_position {
public:
  log_position();
  ~log_position();
  void logPIDDataToFile(cv::Point2d &aim, cv::Point2d &position,
			std::pair < double, double > nextVoltage);
  void logOnePoleDataToFile(cv::Point2d &aim, cv::Point2d &position,
			    std::pair < int, double > nextVoltage);
private:
  std::ofstream position_file;
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
};

#endif // LOG_LOCATION_H
