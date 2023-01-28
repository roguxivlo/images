#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

inline auto compose() {
  return [](auto x) { return x; };
}

template <typename Fun>
auto compose(Fun fun) {
  return [=](auto x) { return fun(x); };
}

template <typename F, typename... Fun>
auto compose(F f, Fun... funs) {
  return lift(compose(funs...), f);
}

template <typename H>
auto lift(H h) {
  return [=](auto... x) { return h(x...); };
}

template <typename H, typename... Fs>
auto lift(H h, Fs... fs) {
  return [=](auto x) { return h(fs(x)...); };
}

#endif  // FUNCTIONAL_H
