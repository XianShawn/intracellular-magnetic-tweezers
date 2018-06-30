#include "fieldofviewwindow.h"
#include "ui_fieldofviewwindow.h"

FieldOfViewWindow::FieldOfViewWindow(FieldOfView& fov, QWidget* parent)
    : QWidget(parent), ui(new Ui::FieldOfViewWindow), _fov(fov) {
    ui->setupUi(this);
    ui->doubleSpinBox->setValue(FieldOfView::DEFAULT_SCALING);
}

FieldOfViewWindow::~FieldOfViewWindow() {
    delete ui;
}

void FieldOfViewWindow::on_doubleSpinBox_valueChanged(double micronsPerPixel) {
    _fov.setScaling(micronsPerPixel);
}
