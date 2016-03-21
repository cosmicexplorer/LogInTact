#include <cmath>
#include <cstring>

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
  size_t l          = G - step + 1;
  RealType divideBy = l;
  Vector results{0};
  for (size_t t = 0; t < l; ++t) {
    const RealType * __restrict__ cur = states + t;
    for (size_t i = 0; i < n; ++i) {
      results[i] += cur[i] / divideBy;
    }
  }
  return results;
}

template <size_t n, size_t G>
RealType linear_sim<n, G>::D(size_t i)
{
  return D_cells[i];
}

template <size_t n, size_t G>
RealType linear_sim<n, G>::W(size_t i, RealType j_c, size_t j)
{
  return W_cells[i * n + j] * j_c;
}

template <size_t n, size_t G>
void linear_sim<n, G>::fill_states(RealType t_f)
{
  RealType delta = t_f / G;
  Vector state(s_0);
  Vector new_state(s_0);
  RealType * __restrict__ states_ptr = s_t.data();
  memcpy(states_ptr, state.data(), VectorSize);
  /* find t_e, then s_e can be done without sweeping across vector again
     since we're already holding the sum to find t_e */
  for (size_t step = 0; step < G; ++step) {
    for (size_t i = 0; i < n; ++i) {
      RealType sum_W = 0;
      /* N.B.: make sure this iterates across matrix in cache-friendly way */
      for (size_t j = 0; j < n; ++j) {
        sum_W += W(i, state[j], j);
      }
      new_state[i] += delta * (D(i) + sum_W);
    }
    state = new_state;
    states_ptr += n;
    memcpy(states_ptr, state.data(), VectorSize);
  }
}

template <size_t n, size_t G>
size_t linear_sim<n, G>::t_e(RealType epsilon)
{
  const RealType * __restrict__ states_ptr = s_t.data();
  for (size_t t_e = 0; t_e <= G; ++t_e) {
    RealType cur_integral = 0;
    const RealType * s_e = states_ptr + t_e;
    for (size_t t = t_e + 1; t <= G; ++t) {
      RealType cur_diff = dist_euclid(s_e, states_ptr + t);
      cur_integral += cur_diff;
      if (cur_integral >= epsilon) {
        break;
      }
    }
    if (cur_integral >= epsilon) {
      continue;
    } else {
      return t_e;
    }
  }
  return G;
}

template <size_t n, size_t G>
typename linear_sim<n, G>::Vector linear_sim<n, G>::s_e(size_t t)
{
  return average_position(s_t.data(), t);
}

template <size_t n, size_t G>
typename linear_sim<n, G>::Vector linear_sim<n, G>::simulate(RealType t_f,
                                                             RealType epsilon)
{
  fill_states(t_f);
  return s_e(t_e(epsilon));
}
}
