#ifndef VIDEO_RECORDING_H
#define VIDEO_RECORDING_H

#include <iostream> // for standard I/O
#include <string>   // for strings
#include <time.h>   // For the filename creation

#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat)
#include <opencv2/videoio.hpp>  // Video write
#include <opencv2/highgui.hpp>

class video_recording {
public:
  video_recording(cv::Size video_size);
  ~video_recording();
  void record(cv::Mat &input_frame);

private:
  cv::VideoWriter output_video;
  std::string file_name;
};

#endif // VIDEO_RECORDING_H
