#ifndef TRACK_FRAME_H
#define TRACK_FRAME_H

#include <iostream>
#include <omp.h>
#include <windows.h>

#include <opencv2/core/core.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/shape.hpp>
#include <opencv2/videoio.hpp>

struct SelectedWindow {
    SelectedWindow() : cropped(false) {
    }

    /**
     * Coordinates in original desktop pixels defining view rectangle
     */
    cv::Point2d upperLeft, lowerRight;
    /**
     * Whether the window is of the original size or if it's been cropped
     * Note that the displayed window either has a zoom of 1x or 3x
     * 1x if we're cropped
     * 3x initially (should not be tracking in this state)
     */
    bool cropped;
};

int ErrorHandler(int, const char*, const char*, const char*, int, void*);
cv::Mat hwnd2mat(HWND hwnd);
cv::Mat getScreenshot();
cv::Mat cropImage(cv::Mat& image, SelectedWindow& selectedArea);

void createPathOverlay(cv::Point2d& current_position,
                       std::vector<cv::Point2d>& pastPositions,
                       cv::Mat& canvas);
void drawAim(cv::Point2d& aim, cv::Mat& canvas);

#endif // TRACK_FRAME_H
