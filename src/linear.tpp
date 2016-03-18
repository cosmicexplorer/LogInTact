namespace sim
{
linear::linear(vrange s, vrange d, vrange w)
    : sim(DFunc, WFunc), s_distr(s), d_distr(d), w_distr(w)
{
  size_t s_d = d.size();
  if (s.size() != s_d or w.size() != s_d * s_d) {
    throw dimension_exception("dimensions of input state do not match");
  }
}

double linear::DFunc(double, size_t i, const DParams & D)
{
  return D[i];
}

double linear::WFunc(
    double, size_t j, double j_c, size_t i, double, const WParams & W)
{
  return W(j, i) * j_c;
}
}
