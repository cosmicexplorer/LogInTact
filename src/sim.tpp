namespace sim
{
double linear_sim::D(size_t i)
{
  return params.D[i];
}

double linear_sim::W(size_t i, double j_c, size_t j)
{
  return params.W(i, j) * j_c;
}

std::vector<double> linear_sim::simulate(double t_f, size_t G)
{
  double delta = t_f / G;
  size_t n = params.s_0.size();
  std::vector<double> state(params.s_0);
  std::vector<double> new_state(n);
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
    }
  }
  return std::move(state);
}
}
