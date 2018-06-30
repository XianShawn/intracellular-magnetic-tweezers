#include "track_frame.h"

// Suppressing Errors:
int ErrorHandler(int, const char*, const char*, const char*, int, void*) {
    return 0;
}

cv::Mat hwnd2mat(HWND hwnd) {

    HDC hwindowDC, hwindowCompatibleDC;

    int height, width, srcheight, srcwidth;
    HBITMAP hbwindow;
    cv::Mat src;
    BITMAPINFOHEADER bi;

    hwindowDC = GetDC(hwnd);
    hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

    RECT window_size; // get the height and width of the screen
    GetClientRect(hwnd, &window_size);

    srcheight = window_size.bottom;
    srcwidth = window_size.right;
    height = window_size
                 .bottom; // change this to whatever size you want to resize to
    width = window_size.right;

    src.create(height, width, CV_8UC4);

    // create a bitmap
    hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight =
        -height; // this is the line that makes it draw upside down or not
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    // use the previously created device context with the bitmap
    SelectObject(hwindowCompatibleDC, hbwindow);
    // copy from the window device context to the bitmap device context
    StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0,
               srcwidth, srcheight,
               SRCCOPY); // change SRCCOPY to NOTSRCCOPY for wacky colors !
    GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data,
              (BITMAPINFO*)&bi,
              DIB_RGB_COLORS); // copy from hwindowCompatibleDC to hbwindow

    // avoid memory leak
    DeleteObject(hbwindow);
    DeleteDC(hwindowCompatibleDC);
    ReleaseDC(hwnd, hwindowDC);

    return src;
}

cv::Mat getScreenshot() {
    HWND desktop_window;
    cv::Mat original_screenshot;

    desktop_window = GetDesktopWindow();
    original_screenshot = hwnd2mat(desktop_window);
    return original_screenshot;
}

cv::Mat cropImage(cv::Mat& image, SelectedWindow& selectedArea) {
    HWND hDesktopWindow = GetDesktopWindow();
    RECT window_size;
    GetClientRect(hDesktopWindow, &window_size);
    if ((selectedArea.cropped) &&
        (selectedArea.upperLeft.x < window_size.right) &&
        (selectedArea.lowerRight.x < window_size.right) &&
        (selectedArea.upperLeft.y < window_size.bottom) &&
        (selectedArea.lowerRight.y < window_size.bottom)) {
        cv::Rect ROI;
        ROI.x = selectedArea.upperLeft.x, ROI.y = selectedArea.upperLeft.y;
        ROI.width = abs(selectedArea.lowerRight.x - selectedArea.upperLeft.x);
        ROI.height = abs(selectedArea.lowerRight.y - selectedArea.upperLeft.y);

        return image(ROI);
    } else
        return image;
}

// Helper function to keep track of location where the bead has been in the past
// Put dots on the canvas
void createPathOverlay(cv::Point2d& current_position,
                       std::vector<cv::Point2d>& pastPositions,
                       cv::Mat& canvas) {
    circle(canvas, current_position, 4, cv::Scalar(0, 255, 0), -1);

    for (std::vector<cv::Point2d>::iterator iter = pastPositions.begin();
         iter != pastPositions.end(); iter++) {
        circle(canvas, *iter, 4, cv::Scalar(0, 0, 255), -1);
    }
    pastPositions.push_back(current_position);
}

void drawAim(cv::Point2d& aim, cv::Mat& canvas) {
    circle(canvas, aim, 4, cv::Scalar(255, 0, 0), -1);
}
