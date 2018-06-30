#include "video_recording.h"

video_recording::video_recording(cv::Size video_size) {
  file_name = "capture_";
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];
  time ( &rawtime );

  timeinfo = localtime ( &rawtime );
  strftime (buffer, 80, "%d.%m.%y_%H.%M.%S.avi", timeinfo);
  file_name.append(buffer);

  output_video.open(file_name, 0x00000000, 30, video_size, true);
  if (!output_video.isOpened()) std::cout << "Error: video unable to be opened" << std::endl;
}

video_recording::~video_recording() {
  output_video.release();
}

void video_recording::record(cv::Mat &input_frame) {
  if (!input_frame.empty()) {
    output_video.write(input_frame);
  } else {
    std::cout << "Error: empty frame" << std::endl;
  }
}
