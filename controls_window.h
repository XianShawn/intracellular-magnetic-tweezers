#ifndef WINDOWONE_H
#define WINDOWONE_H

#include "ui_controls_window.h"
#include "io_communicate.h"
#include "PID.h"
#include "onepolecontrol.h"
#include "predictivecontroller.h"
#include "video_recording.h"

#include "selection_window.h"

#include <limits>
#include <QWidget>

extern double max_voltage_factor;
extern double min_voltage_factor;

extern double pid_voltage_offset;
extern double opc_voltage_offset;

extern communicate_with_io io_communicate;
extern int choose_control;
extern onePoleControl opc;
extern PID pid;
extern predictiveController pc;
extern video_recording* recorder;

extern bool tracking_enabled;
extern bool do_pid;
extern bool bool_log_position;
extern bool screen_recording;

namespace Ui {
class ControlsWindow;
}

class ControlsWindow : public QWidget {
    Q_OBJECT

  public:
    explicit ControlsWindow(QWidget* parent = 0);
    ~ControlsWindow();

  private:
    Ui::ControlsWindow ui;

  public slots:
    void update_tracking_label();
    void update_controller_labels();
    void update_logging_label();
    void update_screen_recording_label();

  private slots:
    // Checkboxes
    void on_enable_pid_box_stateChanged(int State);
    void on_enable_opc_box_stateChanged(int State);
    void on_enable_predictive_box_stateChanged(int State);

    // Manual Control
    void on_up_input_textChanged(const QString&);
    void on_down_input_textChanged(const QString&);
    void on_left_input_textChanged(const QString&);
    void on_right_input_textChanged(const QString&);
    void on_out_of_dish_input_textChanged(const QString&);
    void on_into_dish_input_textChanged(const QString&);
    int on_execute_button_clicked();
    int on_stop_button_clicked();
    void on_up_input_returnPressed();
    void on_down_input_returnPressed();
    void on_left_input_returnPressed();
    void on_right_input_returnPressed();
    void on_out_of_dish_input_returnPressed();
    void on_into_dish_input_returnPressed();

    // PID Settings
    void on_kp_input_textChanged(const QString&);
    void on_ki_input_textChanged(const QString&);
    void on_kd_input_textChanged(const QString&);
    void on_dt_input_textChanged(const QString&);
    void on_pid_max_voltage_input_textChanged(const QString&);
    void on_offset_voltage_input_textChanged(const QString&);
    void on_update_PID_button_clicked();
    void on_kp_input_returnPressed();
    void on_ki_input_returnPressed();
    void on_kd_input_returnPressed();
    void on_dt_input_returnPressed();
    void on_pid_max_voltage_input_returnPressed();
    void on_offset_voltage_input_returnPressed();

    // Predictive Controller Settings
    void on_FPS_input_textChanged(const QString&);
    void on_refresh_rate_input_textChanged(const QString&);
    void on_decrease_factor_input_textChanged(const QString&);
    void on_update_predictive_button_clicked();
    void on_refresh_rate_input_returnPressed();
    void on_FPS_input_returnPressed();
    void on_decrease_factor_input_returnPressed();

    // One Pole Control Settings
    void on_opc_proportion_factor_input_textChanged(const QString&);
    void on_opc_max_voltage_input_textChanged(const QString&);
    void on_opc_offset_voltage_input_textChanged(const QString&);
    void on_update_opc_button_clicked();
    void on_opc_proportion_factor_input_returnPressed();
    void on_opc_max_voltage_input_returnPressed();
    void on_opc_offset_voltage_input_returnPressed();
};

#endif // WINDOWONE_H
