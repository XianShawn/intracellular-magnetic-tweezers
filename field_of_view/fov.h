#ifndef FOV_H
#define FOV_H

/**
 * @brief Convert from pixel coordinates to physical coordinates
 */
class FieldOfView {
  public:
    /**
     * Scaling immediately after initialization
     */
    static constexpr double DEFAULT_SCALING = 1;

    FieldOfView();
    void setScaling(double scaling);

    /**
     * Given input point in pixels, scale to metric coordinates.
     * Metric coordinate is either in m or um (e-6)
     */
    template <typename Point>
    Point scalePoint(const Point& pt) const;

  private:
    // assume aspect ratio is maintained so only 1 scale is needed
    double _scaling;
};

template <typename Point>
Point FieldOfView::scalePoint(const Point& pt) const {
    // input is always going to be a 2D point
    // we assume the API allows for .x and .y
    return Point(pt.x * _scaling, pt.y * _scaling);
}

#endif // FOV_H
