#include <cmath>
#include <cstring>

namespace LogInTact
{
namespace sim
{
namespace math
{
template <size_t n>
RealType dist_euclid(const Vector & a, const Vector & b)
{
  RealType sum = 0;
  /* TODO: ensure this gets vectorized! may have to use restrict pointers to
     underlying array */
  for (size_t i = 0; i < n; ++i) {
    RealType diff = a[i] - b[i];
    sum += diff * diff;
  }
  return sqrt(sum);
}

RealType gaussian(const RealType dist, const RealType sigma)
{
  RealType sigma_sq  = sigma * sigma;
  RealType firstTerm = pow(2 * sigma_sq * Pi, -.5);
  RealType expTerm   = (dist * dist) / (2 * sigma_sq);
  return firstTerm * exp(-expTerm);
}

template <size_t n>
RealType distance_gaussian(const Vector<n> & base,
                           const Vector<n> & other,
                           const RealType sigma)
{
  return gaussian(dist_euclid(base, other), sigma);
}
}

template <size_t n>
RealType linear_sim<n>::W(size_t i, RealType j_c, size_t j) const
{
  return W[i * n + j] * j_c;
}

template <size_t n>
void linear_sim<n>::simulate(RealType t_f, size_t steps_to_simulate)
{
  const RealType delta = t_f / steps_to_simulate;
  C_t                  = C_0;
  Vector tmp(C_t);
  for (size_t step = 1; step < steps_to_simulate; ++step) {
    for (size_t i = 0; i < n; ++i) {
      RealType sum_W = 0;
      /* NOTE: make sure this iterates across matrix in cache-friendly way / is
         autovectorized */
      for (size_t j = 0; j < n; ++j) {
        sum_W += W(i, C_t[j], j);
      }
      tmp[i] += delta * sum_W;
    }
    /* TODO: does this do a deep copy? */
    C_t = tmp;
  }
}
}
}
