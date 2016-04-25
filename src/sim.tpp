#include <cmath>
#include <cstring>

namespace LogInTact
{
namespace sim
{
template <size_t n, size_t G>
RealType linear_sim<n, G>::dist_euclid(const Vector & a, const Vector & b)
{
  RealType sum = 0;
  for (size_t i = 0; i < n; ++i) {
    RealType diff = a[i] - b[i];
    sum += diff * diff;
  }
  return sqrt(sum);
}

template <size_t n, size_t G>
RealType linear_sim<n, G>::D(size_t i) const
{
  return D_cells[i];
}

template <size_t n, size_t G>
RealType linear_sim<n, G>::W(size_t i, RealType j_c, size_t j) const
{
  return W_cells[i * n + j] * j_c;
}

template <size_t n, size_t G>
void linear_sim<n, G>::simulate(RealType t_f,
                                RealType epsilon,
                                size_t steps_to_equil)
{
  const RealType delta = t_f / G;
  Vector new_state(s_t);
  Vector prev_equil_state(s_t);
  for (size_t step = 0; step < G; ++step) {
    if (step % steps_to_equil == 0) {
      if (dist_euclid(prev_equil_state, s_t) < epsilon) {
        finished = true;
        return;
      }
      prev_equil_state = s_t;
    }
    for (size_t i = 0; i < n; ++i) {
      RealType sum_W = 0;
      /* NOTE: make sure this iterates across matrix in cache-friendly way */
      for (size_t j = 0; j < n; ++j) {
        sum_W += W(i, s_t[j], j);
      }
      new_state[i] += delta * (D(i) + sum_W);
    }
    s_t = new_state;
  }
}
}
}
