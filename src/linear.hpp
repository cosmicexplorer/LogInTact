#ifndef ___LINEAR_HPP___
#define ___LINEAR_HPP___

#include "sim.hpp"

namespace sim
{

class simple_matrix
{
  std::vector<double> mem;
  size_t x_max;
  size_t y_max;

public:
  simple_matrix(size_t x, size_t y) : mem(x * y), x_max(x), y_max(y)
  {
  }
  double operator()(size_t x, size_t y) const
  {
    if (x >= x_max) {
      throw dimension_exception(std::string("x index too large: ") +
                                std::to_string(x));
    }
    if (y >= y_max) {
      throw dimension_exception(std::string("y index too large: ") +
                                std::to_string(y));
    }
    return mem[x * x + y];
  }
};

typedef std::vector<double> LinearDParams;
typedef simple_matrix LinearWParams;
class linear : public sim<LinearDParams, LinearWParams>
{
public:
  typedef LinearDParams DParams;
  typedef LinearWParams WParams;

private:
  typedef const std::vector<range> & vrange;
  friend class iterator;
  vrange s_distr;
  vrange d_distr;
  vrange w_distr;

public:
  static double DFunc(double, size_t, const DParams &);
  static double WFunc(double, size_t, double, size_t, double, const WParams &);

  linear(vrange, vrange, vrange);
};
}

#include "linear.tpp"

#endif /* ___LINEAR_HPP___ */
