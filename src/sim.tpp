#include <cmath>
#include <cstring>

namespace LogInTact
{
namespace sim
{
template <size_t n, size_t G>
RealType linear_sim<n, G>::dist_euclid(const RealType * __restrict__ a,
                                       const RealType * __restrict__ b)
{
  RealType sum = 0;
  for (size_t i = 0; i < n; ++i) {
    RealType diff = a[i] - b[i];
    sum += diff * diff;
  }
  return sqrt(sum);
}

template <size_t n, size_t G>
typename linear_sim<n, G>::Vector
    linear_sim<n, G>::average_position(const RealType * __restrict__ states,
                                       size_t step)
{
  size_t l          = G - step;
  RealType divideBy = l;
  Vector results{0};
  states += n * step;
  for (size_t t = 0; t < l; ++t, states += n) {
    for (size_t i = 0; i < n; ++i) {
      results[i] += states[i] / divideBy;
    }
  }
  return results;
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
void linear_sim<n, G>::fill_states(RealType t_f)
{
  RealType delta = t_f / G;
  Vector state(s_0);
  Vector new_state(s_0);
  RealType * __restrict__ states_ptr       = s_t.data();
  RealType * __restrict__ state_single_ptr = state.data();
  memcpy(states_ptr, state_single_ptr, VectorSize);
  /* find t_e, then s_e can be done without sweeping across vector again
     since we're already holding the sum to find t_e */
  for (size_t step = 0; step < G; ++step) {
    for (size_t i = 0; i < n; ++i) {
      RealType sum_W = 0;
      /* NOTE: make sure this iterates across matrix in cache-friendly way */
      for (size_t j = 0; j < n; ++j) {
        sum_W += W(i, state[j], j);
      }
      new_state[i] += delta * (D(i) + sum_W);
    }
    state = new_state;
    states_ptr += n;
    memcpy(states_ptr, state_single_ptr, VectorSize);
  }
}

/* TODO: set failed if can't find good t_e! use gamma-ball */
template <size_t n, size_t G>
void linear_sim<n, G>::set_t_e(RealType epsilon)
{
  /* FIXME: only do this if doesn't fail!!! */
  failed                                   = false;
  const RealType * __restrict__ states_ptr = s_t.data();
  for (size_t cur_t_e = 0; cur_t_e <= G; ++cur_t_e, states_ptr += n) {
    RealType cur_integral             = 0;
    size_t diff                       = G - cur_t_e;
    const RealType * __restrict__ s_t = states_ptr + n;
    for (size_t t = 1; t <= diff && cur_integral < epsilon; ++t, s_t += n) {
      cur_integral += dist_euclid(states_ptr, s_t);
    }
    if (cur_integral < epsilon) {
      t_e = cur_t_e;
      return;
    } else {
      continue;
    }
  }
  t_e = G;
  return;
}

template <size_t n, size_t G>
void linear_sim<n, G>::set_s_e(size_t t)
{
  s_e = average_position(s_t.data(), t);
}

template <size_t n, size_t G>
void linear_sim<n, G>::simulate(RealType t_f, RealType epsilon)
{
  fill_states(t_f);
  set_t_e(epsilon);
  set_s_e(t_e);
}
}
}
