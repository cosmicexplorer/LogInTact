namespace sim
{
template <typename DParams, typename WParams>
void sim<DParams, WParams>::simulate(double t_f, size_t G, OutFunc on_step)
{
  double delta = t_f / G;
  for (parameters<DParams, WParams> cur :
         /* FIXME: do this part! */
       std::vector<parameters<DParams, WParams>>()) {
    size_t n = cur.s_0.size();
    std::vector<double> state(cur.s_0);
    std::vector<double> new_state(n);
    {
      size_t step;
      double t;
      for (step = 0, t = 0; step < G; ++step, t += delta) {
        for (size_t i = 0; i < n; ++i) {
          double sum_W = 0;
          for (size_t j = 0; j < n; ++j) {
            sum_W += W(t, j, state[j], i, state[i], cur.W);
          }
          new_state[i] += delta * (D(t, i, cur.D) + sum_W);
        }
        on_step(step, t, new_state);
        std::swap(state, new_state);
      }
    }
  }
}
}
