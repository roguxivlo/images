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

    // auto h = [image](const Coordinate rho, const Coordinate ang) {
    //   return image(Point(rho, ang, true));
    // };
    auto f1 = [](Point p_polar) { return p_polar.first; };
    auto f2 = [fi](Point p_polar) {
      return (p_polar.second - fi < 0 ? p_polar.second - fi + 2 * M_PI
                                      : p_polar.second - fi);
    };
    auto to_polar_lambda = [](const Point p) {
      return (p.is_polar ? p : to_polar(p));
    };
    auto to_cartesian_lambda = [](const double rho, const double ang) {
      return from_polar(Point(rho, ang, true));
    };
    return compose(to_polar_lambda, lift(to_cartesian_lambda, f1, f2), image)(p);
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
    return compose(scale_cartesian,
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
  return lift(h, f1, f2);
}

template <typename T>
Base_image<T> rings(Point q, double d, T this_way, T that_way) {
  auto to_polar_lambda = [](const Point p) {
    return (p.is_polar ? p : to_polar(p));
  };
  auto to_cartesian_lambda = [q]() {
    return q.is_polar ? from_polar(q) : q;
  };
  Point q_cartesian = to_cartesian_lambda();
  Vector q_vec = Vector(q_cartesian.first, q_cartesian.second);
  Base_image<T> im = compose(to_polar_lambda, checker(d, this_way, that_way));
  return translate(im, q_vec);
}

template<typename T>
Base_image<T> polar_checker(double d, int n, T this_way, T that_way) {
  auto to_polar_lambda = [](const Point p) {
    return (p.is_polar ? p : to_polar(p));
  };

  auto f = [=](Point p_polar) {
    return Point(p_polar.first, (d * p_polar.second) / (2 * M_PI / n));
  };
  return compose(to_polar_lambda, f, checker(d, this_way, that_way));
}

template <typename T>
Base_image<T> vertical_stripe(double d, T this_way, T that_way) {
  return [=](const Point p) {
    return (p.first <= d/2 && p.first >= -d/2 ? this_way : that_way);
  };
}

inline Image cond(Region region, Image this_way, Image that_way) {
  return [=](const Point p) {
    return (region(p) ? this_way(p) : that_way(p));
  };
}

inline Image lerp(Blend blend, Image this_way, Image that_way) {
  return [=](const Point p) {
    Color this_ = this_way(p);
    Color that_ = that_way(p);
    Fraction blend_ = blend(p);
    Color res = this_.weighted_mean(that_, blend_);
    return res;
  };
}

inline Image darken(Image image, Blend blend) {
  return [=](const Point p) {
    Image black = constant<Color>(Colors::black);
    return lerp(blend, image, black)(p);
  };
}

inline Image lighten(Image image, Blend blend) {
  return [=](const Point p) {
    Image white = constant<Color>(Colors::white);
    return lerp(blend, image, white)(p);
  };
}
#endif  // IMAGES_H