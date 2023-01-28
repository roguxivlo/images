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

namespace {}  // namespace

template <typename T>
Base_image<T> constant(const T& t) {
  return [t](const Point) { return t; };
}

template <typename T>
Base_image<T> rotate(Base_image<T> image, double phi) {
  return [image, phi](const Point p) {
    double fi = (phi < 0 ? phi + 2 * M_PI : phi);

    auto h = [image](const Coordinate rho, const Coordinate ang) {
      return image(Point(rho, ang, true));
    };
    auto f1 = [](Point p_polar) { return p_polar.first; };
    auto f2 = [fi](Point p_polar) {
      return (p_polar.second - fi < 0 ? p_polar.second - fi + 2 * M_PI
                                      : p_polar.second - fi);
    };
    auto to_polar_lambda = [](const Point p) {
      return (p.is_polar ? p : to_polar(p));
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
    auto to_cartesian_lambda = [](const Point p) {
      return (p.is_polar ? from_polar(p) : p);
    };
    return compose(translate_cartesian,
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
    auto to_cartesian_lambda = [](const Point p) {
      return (p.is_polar ? from_polar(p) : p);
    };
    return compose(to_cartesian_lambda, scale_cartesian,
                   image_from_cartesian)(p);
  };
}

template <typename T>
Base_image<T> circle(Point q, double r, T inner, T outer) {
  return [=](const Point p) { return (distance(p, q) <= r ? inner : outer); };
}

template <typename T>
Base_image<T> checker(double d, T this_way, T that_way) {
  auto f1 = [=](Point p_cartesian) {
    return std::floor(p_cartesian.first / d);
  };
  auto f2 = [=](Point p_cartesian) {
    return std::floor(p_cartesian.second / d);
  };
  auto h = [=](int x, int y) { return (x + y) % 2 == 0 ? this_way : that_way; };
  auto to_cartesian_lambda = [](const Point p) {
    return (p.is_polar ? from_polar(p) : p);
  };
  return compose(to_cartesian_lambda, lift(h, f1, f2));
}

// template <typename T>
// Base_image<T> polar_checker(double d, int n, T this_way, T that_way) {
//   // Assume that n % 2 == 0
// }

template <typename T>
Base_image<T> vertical_stripe(double d, T this_way, T that_way) {
  return [=](const Point p) {
    return (p.first <= d && p.first >= -d ? this_way : that_way);
  };
}

#endif  // IMAGES_H