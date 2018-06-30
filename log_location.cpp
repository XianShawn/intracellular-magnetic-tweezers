#include "log_location.h"

log_position::log_position() {
  std::string fileName = "position_log";

  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];
  time ( &rawtime );

  timeinfo = localtime ( &rawtime );
  strftime (buffer, 80, "_%d.%m.%y_%H.%M.%S.txt", timeinfo);
  fileName.append(buffer);

  FILE * pFile;
  pFile = fopen (fileName.c_str(),"w");
  fclose (pFile);

  std::cout << fileName << std::endl;
  position_file.open (fileName.c_str());

  position_file << "Current_X: Current_Y: Aim_X: Aim_Y: time: left_voltage: right_voltage: up_voltage: down_voltage:" << std::endl;
  start = std::chrono::high_resolution_clock::now();
}

void log_position::logPIDDataToFile(cv::Point2d &aim, cv::Point2d &position,
				    std::pair < double, double > next_voltage) {
  end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff = end - start;

  position_file << position.x << " " << position.y << " " <<
		   aim.x << " " << aim.y << " " <<
		   diff.count() << " " <<
		   pid_voltage_offset - next_voltage.first << " " <<
		   pid_voltage_offset + next_voltage.first << " " <<
		   -pid_voltage_offset + next_voltage.second << " " <<
		   -pid_voltage_offset - next_voltage.second << " " <<
		   std::endl;
}

void log_position::logOnePoleDataToFile(cv::Point2d &aim, cv::Point2d &position,
					std::pair < int, double > next_voltage) {
  end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff = end - start;

  switch (next_voltage.first) {
    case 0:
      position_file << position.x << " " << position.y << " " <<
		       aim.x << " " << aim.y << " " <<
		       diff.count() << " " <<
		       pid_voltage_offset - next_voltage.first << " " <<
		       0 << " " <<
		       0 << " " <<
		       0 << " " <<
		       std::endl;
      break;
    case 1:
      position_file << position.x << " " << position.y << " " <<
		       aim.x << " " << aim.y << " " <<
		       diff.count() << " " <<
		       0 << " " <<
		       next_voltage.second + opc_voltage_offset << " " <<
		       0 << " " <<
		       0 << " " <<
		       std::endl;
      break;
    case 2:
      position_file << position.x << " " << position.y << " " <<
		       aim.x << " " << aim.y << " " <<
		       diff.count() << " " <<
		       0 << " " <<
		       0 << " " <<
		       next_voltage.second + opc_voltage_offset << " " <<
		       0 << " " <<
		       std::endl;
      break;
    case 3:
      position_file << position.x << " " << position.y << " " <<
		       aim.x << " " << aim.y << " " <<
		       diff.count() << " " <<
		       0 << " " <<
		       0 << " " <<
		       0 << " " <<
		       next_voltage.second + opc_voltage_offset << " " <<
		       std::endl;
      break;
  }
}

log_position::~log_position() {
  position_file.close();
}
