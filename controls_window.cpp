#include "controls_window.h"

ControlsWindow::ControlsWindow(QWidget* parent) : QWidget(parent) {
    ui.setupUi(this);
}

ControlsWindow::~ControlsWindow() {
}

void ControlsWindow::update_tracking_label() {
    if (tracking_enabled)
        ui.tracking_status_label->setText("Tracking: On");
    else
        ui.tracking_status_label->setText("Tracking: Off");
}
void ControlsWindow::update_controller_labels() {
    if (!do_pid) {
        ui.pid_status_label->setText("PID Control: Off");
        ui.predictive_status_label->setText("Predictive Control: Off");
        ui.onepole_status_label->setText("One-Pole Control: Off");
    } else if (do_pid && (choose_control == 0)) {
        ui.pid_status_label->setText("PID Control: On");
        ui.predictive_status_label->setText("Predictive Control: Off");
        ui.onepole_status_label->setText("One-Pole Control: Off");
    } else if (do_pid && (choose_control == 1)) {
        ui.pid_status_label->setText("PID Control: On");
        ui.predictive_status_label->setText("Predictive Control: On");
        ui.onepole_status_label->setText("One-Pole Control: Off");
    } else if (do_pid && (choose_control == 2)) {
        ui.pid_status_label->setText("PID Control: Off");
        ui.predictive_status_label->setText("Predictive Control: Off");
        ui.onepole_status_label->setText("One-Pole Control: On");
    }
}
void ControlsWindow::update_logging_label() {
    if (bool_log_position)
        ui.logging_status_label->setText("Logging: On");
    else
        ui.logging_status_label->setText("logging: Off");
}
void ControlsWindow::update_screen_recording_label() {
    if (screen_recording)
        ui.screen_recording_status_label->setText("Screen Recording: On");
    else
        ui.screen_recording_status_label->setText("Screen Recording: Off");
}

// Link Checkboxes to toggle between PID and One Pole Controller
// Choose control method:
// 0: PID
// 1: Predictive
// 2: One_Pole
void ControlsWindow::on_enable_pid_box_stateChanged(int State) {
    switch (State) {
    case Qt::Unchecked:
        ui.enable_opc_box->setChecked(true);
        if (ui.enable_predictive_box->checkState() == Qt::Checked) {
            ui.enable_predictive_box->setChecked(false);
        }
        choose_control = 2;
        break;
    case Qt::Checked:
        ui.enable_opc_box->setChecked(false);
        if (ui.enable_predictive_box->checkState() == Qt::Checked)
            choose_control = 1; // This should never happen
        else
            choose_control = 0;
        break;
    }
    update_controller_labels();
}
void ControlsWindow::on_enable_opc_box_stateChanged(int State) {
    switch (State) {
    case Qt::Unchecked:
        ui.enable_pid_box->setChecked(true);
        choose_control = 0;
        break;
    case Qt::Checked:
        ui.enable_pid_box->setChecked(false);
        if (ui.enable_predictive_box->checkState() == Qt::Checked) {
            ui.enable_predictive_box->setChecked(false);
        }
        choose_control = 2;
        break;
    }
    update_controller_labels();
}
void ControlsWindow::on_enable_predictive_box_stateChanged(int State) {
    switch (State) {
    case Qt::Unchecked:
        if (ui.enable_pid_box->checkState() ==
            Qt::Unchecked) { // This should never happen
            choose_control = 2;
        }
        if (ui.enable_pid_box->checkState() == Qt::Checked) {
            choose_control = 0;
        }
        break;
    case Qt::Checked:
        if (ui.enable_pid_box->checkState() == Qt::Unchecked)
            ui.enable_pid_box->setChecked(true);
        choose_control = 1;
        break;
    }
    update_controller_labels();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Validate manual input inputs
// Must be between -10 and 10, to 3 decimal places
void ControlsWindow::on_up_input_textChanged(const QString&) {
    ui.up_input->setValidator(new QDoubleValidator(-10, 10, 3, ui.up_input));
}
void ControlsWindow::on_down_input_textChanged(const QString&) {
    ui.down_input->setValidator(
        new QDoubleValidator(-10, 10, 3, ui.down_input));
}
void ControlsWindow::on_left_input_textChanged(const QString&) {
    ui.left_input->setValidator(
        new QDoubleValidator(-10, 10, 3, ui.left_input));
}
void ControlsWindow::on_right_input_textChanged(const QString&) {
    ui.right_input->setValidator(
        new QDoubleValidator(-10, 10, 3, ui.right_input));
}
void ControlsWindow::on_out_of_dish_input_textChanged(const QString&) {
    ui.out_of_dish_input->setValidator(
        new QDoubleValidator(-10, 10, 3, ui.out_of_dish_input));
}
void ControlsWindow::on_into_dish_input_textChanged(const QString&) {
    ui.into_dish_input->setValidator(
        new QDoubleValidator(-10, 10, 3, ui.into_dish_input));
}

// Link "Stop" and "Execute" button functions, and return keys
int ControlsWindow::on_execute_button_clicked() {
    if (io_communicate.set_voltage(ui.up_input->text().toDouble(), 2))
        return 1;
    if (io_communicate.set_voltage(ui.down_input->text().toDouble(), 3))
        return 1;
    if (io_communicate.set_voltage(ui.left_input->text().toDouble(), 0))
        return 1;
    if (io_communicate.set_voltage(ui.right_input->text().toDouble(), 1))
        return 1;
    if (io_communicate.set_voltage(ui.out_of_dish_input->text().toDouble(), 4))
        return 1;
    if (io_communicate.set_voltage(ui.into_dish->text().toDouble(), 5))
        return 1;
    return 0;
}
int ControlsWindow::on_stop_button_clicked() {
    if (io_communicate.stop())
        return 1;
    return 0;
}
void ControlsWindow::on_up_input_returnPressed() {
    on_execute_button_clicked();
}
void ControlsWindow::on_down_input_returnPressed() {
    on_execute_button_clicked();
}
void ControlsWindow::on_left_input_returnPressed() {
    on_execute_button_clicked();
}
void ControlsWindow::on_right_input_returnPressed() {
    on_execute_button_clicked();
}
void ControlsWindow::on_out_of_dish_input_returnPressed() {
    on_execute_button_clicked();
}
void ControlsWindow::on_into_dish_input_returnPressed() {
    on_execute_button_clicked();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Validate PID settings inputs
// Must be double
void ControlsWindow::on_kp_input_textChanged(const QString&) {
    ui.kp_input->setValidator(new QDoubleValidator(
        std::numeric_limits<double>::min(), std::numeric_limits<double>::max(),
        3, ui.kp_input));
}
void ControlsWindow::on_ki_input_textChanged(const QString&) {
    ui.ki_input->setValidator(new QDoubleValidator(
        std::numeric_limits<double>::min(), std::numeric_limits<double>::max(),
        3, ui.ki_input));
}
void ControlsWindow::on_kd_input_textChanged(const QString&) {
    ui.kd_input->setValidator(new QDoubleValidator(
        std::numeric_limits<double>::min(), std::numeric_limits<double>::max(),
        3, ui.kd_input));
}
void ControlsWindow::on_dt_input_textChanged(const QString&) {
    ui.dt_input->setValidator(new QDoubleValidator(
        std::numeric_limits<double>::min(), std::numeric_limits<double>::max(),
        3, ui.dt_input));
}
void ControlsWindow::on_pid_max_voltage_input_textChanged(const QString&) {
    ui.pid_max_voltage_input->setValidator(new QDoubleValidator(
        std::numeric_limits<double>::min(), std::numeric_limits<double>::max(),
        3, ui.pid_max_voltage_input));
}
void ControlsWindow::on_offset_voltage_input_textChanged(const QString&) {
    ui.offset_voltage_input->setValidator(new QDoubleValidator(
        std::numeric_limits<double>::min(), std::numeric_limits<double>::max(),
        3, ui.offset_voltage_input));
}

// Link "Apply Pid Settings" button function, and return keys
void ControlsWindow::on_update_PID_button_clicked() {
    std::cout << "kd: " << ui.kd_input->text().toDouble() << std::endl;
    std::cout << "kp: " << ui.kp_input->text().toDouble() << std::endl;
    std::cout << "ki: " << ui.ki_input->text().toDouble() << std::endl;
    pid_voltage_offset = ui.offset_voltage_input->text().toDouble();
    pid.updateParams(
        ui.dt_input->text().toDouble(),
        ui.pid_max_voltage_input->text().toDouble() - pid_voltage_offset,
        pid_voltage_offset - ui.pid_max_voltage_input->text().toDouble(),
        ui.kp_input->text().toDouble(), ui.ki_input->text().toDouble(),
        ui.kd_input->text().toDouble());
    ui.opc_max_voltage_input->setText(ui.pid_max_voltage_input->text());
}
void ControlsWindow::on_kp_input_returnPressed() {
    on_update_PID_button_clicked();
}
void ControlsWindow::on_ki_input_returnPressed() {
    on_update_PID_button_clicked();
}
void ControlsWindow::on_kd_input_returnPressed() {
    on_update_PID_button_clicked();
}
void ControlsWindow::on_dt_input_returnPressed() {
    on_update_PID_button_clicked();
}
void ControlsWindow::on_pid_max_voltage_input_returnPressed() {
    on_update_PID_button_clicked();
}
void ControlsWindow::on_offset_voltage_input_returnPressed() {
    pid_voltage_offset = ui.offset_voltage_input->text().toDouble();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Validate predictive settings inputs
// Must be positive int
void ControlsWindow::on_FPS_input_textChanged(const QString&) {
    ui.FPS_input->setValidator(new QIntValidator(0, INT_MAX, ui.FPS_input));
}
void ControlsWindow::on_refresh_rate_input_textChanged(const QString&) {
    ui.refresh_rate_input->setValidator(
        new QIntValidator(0, INT_MAX, ui.refresh_rate_input));
}
// Must be a fraction between 0 and 1
void ControlsWindow::on_decrease_factor_input_textChanged(const QString&) {
    ui.decrease_factor_input->setValidator(
        new QDoubleValidator(0, 1, 4, ui.decrease_factor_input));
}

// Link "Apply Predictive Settings" button function, and return keys
void ControlsWindow::on_update_predictive_button_clicked() {
    pc.updateControllerParams(
        ui.dt_input->text().toDouble(),
        ui.pid_max_voltage_input->text().toDouble() - pid_voltage_offset,
        pid_voltage_offset - ui.pid_max_voltage_input->text().toDouble(),
        ui.kp_input->text().toDouble(), ui.ki_input->text().toDouble(),
        ui.kd_input->text().toDouble(),
        ui.refresh_rate_input->text().toDouble(),
        ui.FPS_input->text().toDouble(),
        ui.decrease_factor_input->text().toDouble());
}
void ControlsWindow::on_refresh_rate_input_returnPressed() {
    on_update_predictive_button_clicked();
}
void ControlsWindow::on_FPS_input_returnPressed() {
    on_update_predictive_button_clicked();
}
void ControlsWindow::on_decrease_factor_input_returnPressed() {
    on_update_predictive_button_clicked();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Validate One Pole Controller settings inputs
// Must be double
void ControlsWindow::on_opc_proportion_factor_input_textChanged(
    const QString&) {
    ui.opc_proportion_factor_input->setValidator(new QDoubleValidator(
        std::numeric_limits<double>::min(), std::numeric_limits<double>::max(),
        3, ui.opc_proportion_factor_input));
}
void ControlsWindow::on_opc_max_voltage_input_textChanged(const QString&) {
    ui.opc_max_voltage_input->setValidator(new QDoubleValidator(
        std::numeric_limits<double>::min(), std::numeric_limits<double>::max(),
        3, ui.opc_max_voltage_input));
}
void ControlsWindow::on_opc_offset_voltage_input_textChanged(const QString&) {
    ui.opc_offset_voltage_input->setValidator(new QDoubleValidator(
        std::numeric_limits<double>::min(), std::numeric_limits<double>::max(),
        3, ui.opc_offset_voltage_input));
}

// Link "Apply One-Pole Settings" button function, and return keys
void ControlsWindow::on_update_opc_button_clicked() {
    opc_voltage_offset = ui.opc_offset_voltage_input->text().toDouble();
    ui.pid_max_voltage_input->setText(ui.opc_max_voltage_input->text());
    opc.updateParams(
        ui.opc_proportion_factor_input->text().toDouble(),
        ui.opc_max_voltage_input->text().toDouble() - opc_voltage_offset,
        opc_voltage_offset - ui.opc_max_voltage_input->text().toDouble());
}
void ControlsWindow::on_opc_proportion_factor_input_returnPressed() {
    on_update_opc_button_clicked();
}
void ControlsWindow::on_opc_max_voltage_input_returnPressed() {
    on_update_opc_button_clicked();
}
void ControlsWindow::on_opc_offset_voltage_input_returnPressed() {
    on_update_opc_button_clicked();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
