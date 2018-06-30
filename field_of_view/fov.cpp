#include "fov.h"

FieldOfView::FieldOfView() : _scaling(1) {
}

void FieldOfView::setScaling(double scaling) {
    _scaling = scaling;
}
