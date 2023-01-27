#ifndef IMAGES_H
#define IMAGES_H

#include <functional>

#include "color.h"
#include "math.h"

using Fraction = double;

template <typename T>
using Base_image = std::function<T(const Point)>;

using Region = Base_image<bool>;

using Image = Base_image<Color>;

using Blend = Base_image<Fraction>;

template <typename T>
Base_image<T> constant(const T& t) {
  return [t](const Point) { return t; };
}

template <typename T>
Base_image<T> rotate(Base_image<T> image, double phi) {
  return [image, phi](const Point p) {
    Point p_polar = (p.is_polar ? p : to_polar(p));

    Coordinate p_rotated_first = p_polar.first;
    Coordinate p_rotated_second =
        (p_polar.second + phi > 2 * M_PI ? p_polar.second + phi - 2 * M_PI
                                         : p_polar.second + phi);

    (p_rotated_second < 0 ? p_rotated_second += 2 * M_PI : p_rotated_second);

    return image(Point(p_rotated_first, p_rotated_second, true));
  };
}

template <typename T>
Base_image<T> translate(Base_image<T> image, Vector v) {
  return [image, v](const Point p) {
    Point p_cartesian = (p.is_polar ? from_polar(p) : p);
    Coordinate p_translated_first = p_cartesian.first + v.first;
    Coordinate p_translated_second = p_cartesian.second + v.second;

    return image(Point(p_translated_first, p_translated_second));
  };
}



#endif  // IMAGES_H