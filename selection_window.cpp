#include "selection_window.h"

SelectionWindow::SelectionWindow(const FieldOfView& fov, QWidget* parent)
    : QWidget(parent), _fov(fov) {
    ui.setupUi(this);

    tracking_enabled = false;
    do_pid = false;
    bool_log_position = false;
    needToInit = false;

    emit logging_status_change();
    emit controller_change();
    emit tracking_status_change();
    emit debug_mode_status_change();

    _currentPos.pos = {INT_MAX, INT_MAX};
    temp_pos.first = -1, temp_pos.second = -1;

    // Start the loop
    Timer = new QTimer(this);
    connect(Timer, SIGNAL(timeout()), this, SLOT(TrackFrame()));
    Timer->start(100);
}

SelectionWindow::~SelectionWindow() {
    if (position_log != NULL) {
        std::cout << "Stopped logging location" << std::endl;
        delete position_log;
        position_log = NULL;
    }
}

void SelectionWindow::mousePressEvent(QMouseEvent* event) {
    const auto click = cv::Point2f((float)event->x(), (float)event->y());
    std::cout << "Mouse press " << click << " " << _fov.scalePoint(click)
              << std::endl;

    if (event->button() == Qt::LeftButton) {
        if (event->modifiers() == Qt::ControlModifier) {
            _selectedWindow.upperLeft.x = event->x() * full_window_scale_factor;
            _selectedWindow.upperLeft.y = event->y() * full_window_scale_factor;
        } else if (event->modifiers() == Qt::AltModifier) {
            std::cout << "Setting aim: " << click << std::endl;
            _referencePos.pos = {(double)event->x(), (double)event->y()};
            _referencePos.tracked = true;
            do_pid = true;
            emit controller_change();

            pid.reset();
        } else {
            _clickedPoint = cv::Point2f((float)event->x(), (float)event->y());
            std::cout << "Tracking object at " << _clickedPoint << std::endl;

            pid.reset();
            addRemovePt = true;
            tracking_enabled = true;
            _currentPos.tracked = true;
            emit tracking_status_change();
        }
    }
}

void SelectionWindow::mouseReleaseEvent(QMouseEvent* event) {
    std::cout << "Mouse release (" << event->x() << "," << event->y() << ")"
              << std::endl;
    /*
     * Crop event
     */
    if (event->button() == Qt::LeftButton &&
        event->modifiers() == Qt::ControlModifier) {
        // if we're already cropped then reset because that's easier than
        // recropping
        if (_selectedWindow.cropped) {
            resetCrop();
            return;
        }

        int click_x = event->x() * full_window_scale_factor;
        int click_y = event->y() * full_window_scale_factor;

        const auto d = cv::Point2d(click_x - _selectedWindow.upperLeft.x,
                                   click_y - _selectedWindow.upperLeft.y);

        // same point if norm of difference is small enough
        if (d.dot(d) < 20) {
            return;
        }

        // dragged towards upper left (inverted rectangle)
        if (_selectedWindow.upperLeft.x > click_x) {
            _selectedWindow.lowerRight.x = _selectedWindow.upperLeft.x;
            _selectedWindow.upperLeft.x = click_x;
        } else {
            _selectedWindow.lowerRight.x = click_x;
        }

        if (_selectedWindow.upperLeft.y > click_y) {
            _selectedWindow.lowerRight.y = _selectedWindow.upperLeft.y;
            _selectedWindow.upperLeft.y = click_y;
        } else {
            _selectedWindow.lowerRight.y = click_y;
        }

        _selectedWindow.cropped = true;
        pastPositions.clear();

        std::cout << "Cropped to top left: " << _selectedWindow.upperLeft
                  << " lower right: " << _selectedWindow.lowerRight
                  << std::endl;
        if (recorder != NULL) {
            std::cout << "Recording stopped." << std::endl;
            delete recorder;
            recorder = NULL;
            screen_recording = false;
            emit screen_recording_status_change();
        }
    }
}

void SelectionWindow::resetCrop() {
    _selectedWindow.cropped = false;
    _selectedWindow.upperLeft.x = 0;
    _selectedWindow.upperLeft.y = 0;
    _selectedWindow.lowerRight.x = window_size.right;
    _selectedWindow.lowerRight.y = window_size.bottom;
}

void SelectionWindow::keyPressEvent(QKeyEvent* key) {
    switch (key->key()) {
    case Qt::Key_T: //'t' has been pressed. this will turn off tracking
        tracking_enabled = false;
        emit tracking_status_change();
        std::cout << "Tracking disabled." << std::endl;
        _currentPos.tracked = false;
        pastPositions.clear();
        tracking_point[0].clear();
        tracking_point[1].clear();
        break;
    case Qt::Key_L: // 'l' pressed, log position to file if position is being
                    // tracked
        bool_log_position = !bool_log_position;
        emit logging_status_change();
        if (bool_log_position) {
            std::cout << "Logging location" << std::endl;
            position_log = new log_position();
        } else {
            if (position_log != NULL) {
                std::cout << "Stopped logging location" << std::endl;
                delete position_log;
                position_log = NULL;
            }
        }
        break;

    case Qt::Key_P: // 'p' to tell the system to stop adding voltage
        do_pid = !do_pid;
        emit controller_change();
        if (!do_pid) {
            std::cout << "PID Stopped" << std::endl;
            io_communicate.stop();
            _referencePos.tracked = false;
            pid.reset();
        } else {
            std::cout << "Click somewhere to aim" << std::endl;
        }
        break;

    case Qt::Key_R: // 'r' has been pressed. Acts as a hard reset, reset
                    // everything
        resetCrop();

        _referencePos.tracked = false;
        pid.reset();
        io_communicate.stop();
        do_pid = false;
        emit controller_change();
        emit tracking_status_change();
        pastPositions.clear();

        if (recorder != NULL) {
            std::cout << "Recording stopped." << std::endl;
            delete recorder;
            recorder = NULL;
            screen_recording = false;
            emit screen_recording_status_change();
        }

        break;

    case Qt::Key_S: // 's' has been pressed. Start recording the screen.
        screen_recording = !screen_recording;
        emit screen_recording_status_change();
        if (screen_recording) {
            std::cout << "Recording..." << std::endl;
            recorder = new video_recording(screenshot.size());
        } else {
            if (recorder != NULL) {
                std::cout << "Recording stopped." << std::endl;
                delete recorder;
                recorder = NULL;
            }
        }
        break;

    case Qt::Key_Z: // 'z' to reset the zoom in the window
        resetCrop();
        pastPositions.clear();
        if (recorder != NULL) {
            std::cout << "Recording stopped." << std::endl;
            delete recorder;
            recorder = NULL;
            screen_recording = false;
            emit screen_recording_status_change();
        }
        break;
    }
}

void SelectionWindow::TrackFrame() {
    // type of termination criteria: COUNT, EPIS, both
    // max number of iterations / elements
    // accuracy
    cv::TermCriteria termcrit(cv::TermCriteria::COUNT | cv::TermCriteria::EPS,
                              20, 0.03);
    cv::Size subPixWinSize(10, 10), winSize(31, 31);

    // Now take a screenshot
    screenshot = getScreenshot();
    screenshot = cropImage(screenshot, _selectedWindow);

    // Resize them so it fits on the screen
    // Resize before searching for tracking so the mouse click location and the
    // coordinates match up
    if (!_selectedWindow.cropped) {
        hDesktopWindow = GetDesktopWindow();
        GetClientRect(hDesktopWindow, &window_size);
        cv::resize(screenshot, screenshot,
                   cv::Size(window_size.right / full_window_scale_factor,
                            window_size.bottom / full_window_scale_factor),
                   0, 0, cv::INTER_LINEAR);
        this->resize(QSize(window_size.right / full_window_scale_factor,
                           window_size.bottom / full_window_scale_factor));
        ui.display_image->resize(
            QSize(window_size.right / full_window_scale_factor,
                  window_size.bottom / full_window_scale_factor));
    } else {
        cv::resize(screenshot, screenshot, cv::Size(0, 0),
                   selected_window_scale_factor, selected_window_scale_factor,
                   cv::INTER_LINEAR);
        this->resize(QSize(screenshot.cols, screenshot.rows));
        ui.display_image->resize(QSize(screenshot.cols, screenshot.rows));
    }

    screenshot.copyTo(image);

    // image == original image
    // gray == gray version of the same image to track it with
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    cv::cvtColor(screenshot, screenshot, CV_BGR2RGB);

    if (needToInit) {
        // automatic initialization

        /*    void goodFeaturesToTrack( InputArray image, OutputArray corners,
                            int maxCorners, double qualityLevel, double
           minDistance,
                            InputArray mask = noArray(), int blockSize = 3,
                            bool useHarrisDetector = false, double k = 0.04 );
           */
        cv::goodFeaturesToTrack(gray, tracking_point[NEXT], MAX_COUNT, 0.001,
                                10, cv::Mat(), 3, 0, 0.04);

        /*     void cornerSubPix( InputArray image, InputOutputArray corners,
                                Size winSize, Size zeroZone,
                                TermCriteria criteria ); */
        cv::cornerSubPix(gray, tracking_point[NEXT], subPixWinSize,
                         cv::Size(-1, -1), termcrit);

        addRemovePt = false;
    } else if (!tracking_point[PREV].empty()) {
        tracking_enabled = true;
        std::vector<uchar> status;
        std::vector<float> err;
        if (prevGray.empty())
            gray.copyTo(prevGray);

        /*  void calcOpticalFlowPyrLK( InputArray prevImg, InputArray nextImg,
                                        InputArray prevPts, InputOutputArray
           nextPts,
                                        OutputArray status, OutputArray err,
                                        Size winSize = Size(21,21), int maxLevel
           = 3,
                                        TermCriteria criteria =
           TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01),
                                        int flags = 0, double minEigThreshold =
           1e-4 ); */
        cv::calcOpticalFlowPyrLK(prevGray, gray, tracking_point[PREV],
                                 tracking_point[NEXT], status, err, winSize, 3,
                                 termcrit, 0, 0.001);

        // Update the list of tracking_point and draw them on the picture
        // If the point is not close enough to one of the points ignore it
        if (addRemovePt) {
            if (cv::norm(_clickedPoint - tracking_point[NEXT][0]) <= 5) {
                addRemovePt = false;
            }
        }
    }

    if (addRemovePt && tracking_point[NEXT].size() < (size_t)MAX_COUNT) {
        std::vector<cv::Point2f> tmp;
        tmp.push_back(_clickedPoint);
        cv::cornerSubPix(gray, tmp, winSize, cv::Size(-1, -1), termcrit);
        tracking_point[NEXT].resize(1);
        tracking_point[NEXT][0] = tmp[0];
        addRemovePt = false;
    }
    needToInit = false;
    // set prev for next round
    tracking_point[PREV] = tracking_point[NEXT];
    cv::swap(prevGray, gray);

    if (!tracking_point[PREV].empty())
        _currentPos.pos = tracking_point[PREV][0];

    // If you want to record the screen, record it now before the dots are drawn
    // on
    if (screen_recording)
        recorder->record(screenshot);

    // Put the path on top of the screenshot to be shown
    // Update the past positions
    createPathOverlay(_currentPos.pos, pastPositions, screenshot);

    // Draw the aimed point on the screenshots
    if (_referencePos.tracked)
        drawAim(_referencePos.pos, screenshot);

    // Display the screenshots on the screen
    QImage imdisplay((uchar*)screenshot.data, screenshot.cols, screenshot.rows,
                     screenshot.step, QImage::Format_RGB888);
    ui.display_image->setPixmap(QPixmap::fromImage(imdisplay));

    // Update DAQ voltages
    std::pair<double, double> nextVoltagePID;
    std::pair<int, double> nextVoltageOP;

    if (_currentPos.tracked && _referencePos.tracked) {
        // scale to world coordinates so the tuned gains remain accurate
        const auto inputPos = _fov.scalePoint(_currentPos.pos);
        std::cout << "Scaled point in um " << inputPos << std::endl;

        if (do_pid && (choose_control == 0)) {
            nextVoltagePID = pid.calculateNext(_referencePos.pos, inputPos);
            io_communicate.pid_update_voltages(nextVoltagePID);
        } else if (do_pid && (choose_control == 1)) {
            nextVoltagePID = pc.calculateNext(_referencePos.pos, inputPos);
            io_communicate.pid_update_voltages(nextVoltagePID);
        } else if (do_pid && (choose_control == 2)) {
            nextVoltageOP = opc.calculateNext(_referencePos.pos, inputPos);
            io_communicate.one_pole_update_voltages(nextVoltageOP);
        }
    }

    // Log the position of the bead into the file
    if (bool_log_position && _currentPos.tracked && (position_log != NULL)) {
        if (do_pid && ((choose_control == 0) || (choose_control == 1))) {
            position_log->logPIDDataToFile(_referencePos.pos, _currentPos.pos,
                                           nextVoltagePID);
        } else if (do_pid && (choose_control == 2)) {
            position_log->logOnePoleDataToFile(_referencePos.pos,
                                               _currentPos.pos, nextVoltageOP);
        }
    }
}
