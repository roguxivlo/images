#ifndef IMAGES_H
#define IMAGES_H

#include <functional>

#include "color.h"
#include "coordinate.h"
#include "math.h"

using Fraction = double;

template <typename T>
using Base_image = std::function<T(const Point)>;

using Region = Base_image<bool>;

using Image = Base_image<Color>;

using Blend = Base_image<Fraction>;

namespace {
auto to_polar_lambda = [](Point p) { return (p.is_polar ? p : to_polar(p)); };
auto to_cartesian_lambda = [](Point p) {
  return (p.is_polar ? from_polar(p) : p);
};
}  // namespace

template <typename T>
Base_image<T> constant(const T& t) {
  return [t](const Point) { return t; };
}

template <typename T>
Base_image<T> rotate(Base_image<T> image, double phi) {
  return [image, phi](const Point p) {
    phi = (phi < 0 ? phi + 2 * M_PI : phi);

    auto h = [image](const Coordinate rho, const Coordinate ang) {
      return image(Point(rho, ang, true));
    };
    auto f1 = [](Point p_polar) { return p_polar.first; };
    auto f2 = [phi](Point p_polar) {
      return (p_polar.second - phi < 0 ? p_polar.second - phi + 2 * M_PI
                                       : p_polar.second - phi);
    };

    return compose(to_polar_lambda, lift(h, f1, f2))(p);
  };
}

template <typename T>
Base_image<T> translate(Base_image<T> image, Vector v) {
  return [image, v](const Point p) {
    auto image_from_cartesian = [image](Point p_cartesian) {
      return image(p_cartesian);
    };

    auto translate_cartesian = [v](Point p_cartesian) {
      return Point(p_cartesian.first - v.first, p_cartesian.second - v.second,
                   false);
    };

    return compose(to_cartesian_lambda, translate_cartesian,
                   image_from_cartesian)(p);
  };
}

template <typename T>
Base_image<T> scale(Base_image<T> image, double s) {
  return [=](const Point p) {
    auto image_from_cartesian = [image](Point p_cartesian) {
      return image(p_cartesian);
    };

    auto scale_cartesian = [s](Point p_cartesian) {
      return Point(p_cartesian.first / s, p_cartesian.second / s, false);
    };

    return compose(to_cartesian_lambda, scale_cartesian,
                   image_from_cartesian)(p);
  };
}

template <typename T>
Base_image<T> circle(Point q, double r, T inner, T outer) {
  return [=](const Point p) { return (distance(p, q) <= r ? inner : outer); };
}

#endif  // IMAGES_H