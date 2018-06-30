#include <QApplication>

#include "controls_window.h"
#include "selection_window.h"
#include "io_communicate.h"
#include "pid.h"
#include "track_frame.h"
#include "onepolecontrol.h"
#include "predictivecontroller.h"
#include "video_recording.h"
#include "field_of_view/fieldofviewwindow.h"

#include "field_of_view/fov.h"

// All these numbers must match the text in WindowOne for the initial window to
// make sense
// Before the first interaction with the window these variables are not
// connected!!

// PID Variables
double dt = 0.1;
double max_voltage_factor = 2.2;
double min_voltage_factor = -2.2;
double pid_proportion_factor = 0.1;
double pid_integral_factor = 0.01;
double pid_differential_factor = 0;

// Predictive Controller variables
double number_of_samples = 5;
double fps = 4;
double decrease_factor = 0.333;

// One Pole Controller Variables
double opc_proportion_factor = 0.001;

double pid_voltage_offset = 0.3;
double opc_voltage_offset = 0.3;

bool do_pid = false;
bool tracking_enabled = false;
bool bool_log_position = false;
bool screen_recording = false;

PID pid = PID(dt, max_voltage_factor, min_voltage_factor, pid_proportion_factor,
              pid_integral_factor, pid_differential_factor);
predictiveController pc = predictiveController(
    dt, max_voltage_factor, min_voltage_factor, pid_proportion_factor,
    pid_integral_factor, pid_differential_factor, number_of_samples, fps,
    decrease_factor);
onePoleControl opc = onePoleControl(opc_proportion_factor, max_voltage_factor,
                                    min_voltage_factor);
communicate_with_io io_communicate;
video_recording* recorder;

// Choose control method:
// 0: PID
// 1: Predictive
// 2: One_Pole
int choose_control;

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    FieldOfView fov;

    ControlsWindow controls;
    SelectionWindow selection(fov);
    FieldOfViewWindow fovWindow(fov);

    QObject::connect(&selection, SIGNAL(tracking_status_change()), &controls,
                     SLOT(update_tracking_label()));
    QObject::connect(&selection, SIGNAL(controller_change()), &controls,
                     SLOT(update_controller_labels()));
    QObject::connect(&selection, SIGNAL(logging_status_change()), &controls,
                     SLOT(update_logging_label()));
    QObject::connect(&selection, SIGNAL(screen_recording_status_change()),
                     &controls, SLOT(update_screen_recording_label()));

    controls.show();
    selection.show();
    fovWindow.show();

    // move windows into easy to use locations
    controls.move(0, 0);
    fovWindow.move(0, controls.y() + controls.frameGeometry().height());
    selection.move(controls.x() + controls.frameGeometry().width(), 0);

    return a.exec();
}
