#ifndef FIELDOFVIEWWINDOW_H
#define FIELDOFVIEWWINDOW_H

#include <QWidget>
#include "fov.h"

namespace Ui {
class FieldOfViewWindow;
}

class FieldOfViewWindow : public QWidget {
    Q_OBJECT

  public:
    explicit FieldOfViewWindow(FieldOfView& fov, QWidget* parent = 0);
    ~FieldOfViewWindow();

  private slots:
    void on_doubleSpinBox_valueChanged(double arg1);

  private:
    Ui::FieldOfViewWindow* ui;
    FieldOfView& _fov;
};

#endif // FIELDOFVIEWWINDOW_H
