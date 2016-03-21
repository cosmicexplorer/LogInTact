#include <cmath>

namespace sim
{
double linear_sim::dist_euclid(const std::vector<double> & a,
                               const std::vector<double> & b)
{
  size_t s = a.size();
  if (s != b.size()) {
    throw dimension_exception("vectors are different sizes!");
  }
  double sum = 0;
  for (size_t i = 0; i < s; ++i) {
    double diff = a[i] - b[i];
    sum += diff * diff;
  }
  return sqrt(sum);
}

std::vector<double> linear_sim::average_position(
    const std::vector<std::vector<double>> & states, size_t step)
{
  size_t l = states.size();
  if (0 == l) {
    return std::vector<double>(l, 0);
  }
  size_t n = states[0].size();
  std::vector<double> results(n, 0);
  for (size_t t = step; t < l; ++t) {
    std::vector<double> cur = states[t];
    for (size_t i = 0; i < n; ++i) {
      results[i] += cur[i];
    }
  }
  return divide(results, l - step);
}

std::vector<double> linear_sim::divide(const std::vector<double> & v, double d)
{
  size_t n = v.size();
  std::vector<double> ret(n, 0);
  for (size_t i = 0; i < n; ++i) {
    ret[i] = v[i] / d;
  }
  return ret;
}

double linear_sim::D(size_t i)
{
  return params.D[i];
}

double linear_sim::W(size_t i, double j_c, size_t j)
{
  return params.W(i, j) * j_c;
}

std::vector<double> linear_sim::simulate(double t_f, size_t G, double epsilon)
{
  double delta = t_f / G;
  size_t n = params.s_0.size();
  std::vector<double> state(params.s_0);
  std::vector<double> new_state(n);
  std::vector<std::vector<double>> states;
  {
    /* find t_e, then s_e can be done without sweeping across vector again
       since we're already holding the sum to find t_e */
    size_t step;
    double t;
    for (step = 0, t = 0; step < G; ++step, t += delta) {
      for (size_t i = 0; i < n; ++i) {
        double sum_W = 0;
        /* note: make sure this iterates across matrix in cache-friendly way */
        for (size_t j = 0; j < n; ++j) {
          sum_W += W(i, state[j], j);
        }
        new_state[i] += delta * (D(i) + sum_W);
      }
      std::swap(state, new_state);
      states.push_back(state);
    }
  }
  /* now find s_e */
  for (size_t t_e = 0; t_e < G; ++t_e) {
    double cur_integral             = 0;
    const std::vector<double> & s_e = states[t_e];
    for (size_t t = t_e; t < G; ++t) {
      double cur_diff = dist_euclid(s_e, states[t]);
      cur_integral += cur_diff;
      if (cur_integral >= epsilon) {
        break;
      }
    }
    if (cur_integral >= epsilon) {
      continue;
    } else {
      return average_position(states, t_e);
    }
  }
  throw std::runtime_error("should never get here!");
}
}
