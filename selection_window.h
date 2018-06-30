#ifndef WINDOWTWO_H
#define WINDOWTWO_H

#include "ui_selection_window.h"
#include "io_communicate.h"
#include "pid.h"
#include "log_location.h"
#include "track_frame.h"
#include "onepolecontrol.h"
#include "predictivecontroller.h"
#include "video_recording.h"

#include "controls_window.h"

#include "field_of_view/fov.h"

#include <iostream>

// openCV
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/tracking.hpp>

#include <QWidget>
// Allows the tracking of what the mouse does
#include <QMouseEvent>
#include <Qt> // For definitions
#include <QTimer> // Allows the timer to trigger the code to execute when the window is opened

extern communicate_with_io io_communicate;

extern int choose_control;

extern PID pid;
extern predictiveController pc;
extern onePoleControl opc;
extern video_recording* recorder;

extern bool do_pid;
extern bool tracking_enabled;
extern bool bool_log_position;
extern bool screen_recording;

namespace Ui {
class SelectionWindow;
}

class SelectionWindow : public QWidget {
    Q_OBJECT

  public:
    explicit SelectionWindow(const FieldOfView& fov, QWidget* parent = 0);
    ~SelectionWindow();

    QImage imdisplay; // This will create QImage which is shown in Qt label
    QTimer* Timer;    // A timer is needed in GUI application

    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* key);

  public slots:
    void TrackFrame();

signals:
    void tracking_status_change();
    void controller_change();
    void logging_status_change();
    void debug_mode_status_change();
    void screen_recording_status_change();

  private:
    /**
     * @brief Reset the capture window to original zoom and origin
     */
    void resetCrop();

    struct TrackedPosition {
        TrackedPosition() : tracked(false) {
        }

        cv::Point2d pos;
        bool tracked;
    };

    Ui::SelectionWindow ui;

    SelectedWindow _selectedWindow;
    // where we are
    TrackedPosition _currentPos;
    // where we want to go
    TrackedPosition _referencePos;

    // allow conversion between pixel and world coordinates
    const FieldOfView& _fov;

    log_position* position_log = NULL;
    std::pair<int, int> temp_pos;
    HWND hDesktopWindow;
    RECT window_size;
    std::vector<cv::Point2d> pastPositions;
    std::pair<double, double> nextVoltage;
    int full_window_scale_factor = 3;
    double selected_window_scale_factor = 0.8;

    enum PointsTimeStatus { PREV = 0, NEXT, NUM_TIME_STATUS };
    /**
     * @brief tracking_point Current and previous points
     * index 0 is prev points; index 1 is current points
     */
    std::vector<cv::Point2f> tracking_point[NUM_TIME_STATUS];
    cv::Point2f _clickedPoint;
    cv::Mat screenshot;
    cv::Mat gray, prevGray, image;

    const int MAX_COUNT = 500;
    bool needToInit = false;
    bool addRemovePt = false;
};

#endif // WINDOWTWO_H
