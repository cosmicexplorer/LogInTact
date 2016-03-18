#ifndef ___SIM_HPP___
#define ___SIM_HPP___

#include <vector>
#include <functional>

namespace sim
{
/* throw this if the dimensions don't work out */
struct dimension_exception : std::runtime_error {
  dimension_exception(const std::string & s) : std::runtime_error(s)
  {
  }
};

struct range {
  double beg;
  double end;
  size_t G;
};

template <typename DParams, typename WParams>
struct parameters {
  const std::vector<double> & s_0;
  const DParams & D;
  const WParams & W;
};

template <typename DParams, typename WParams>
class sim
{
public:
  typedef std::function<double(double, size_t, const DParams &)> DType;
  typedef std::function<double(
      double, size_t, double, size_t, double, const WParams &)> WType;
  typedef std::function<void(size_t, double, const std::vector<double> &)>
      OutFunc;

  sim(DType d, WType w) : D(d), W(w)
  {
  }
  void simulate(double, size_t, OutFunc);

private:
  DType D;
  WType W;
};
}

#include "sim.tpp"

#endif /* ___SIM_HPP___ */
