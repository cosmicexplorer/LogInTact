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

class simple_matrix
{
  std::vector<double> mem;
  size_t x_max;
  size_t y_max;

public:
  simple_matrix(size_t x, size_t y, const std::vector<double> & m)
      : mem(m), x_max(x), y_max(y)
  {
    size_t m_s = m.size();
    if (x * y != m_s) {
      throw dimension_exception(std::string("boundary: x * y = ") +
                                std::to_string(x * y) + " vs m.size() = " +
                                std::to_string(m_s));
    }
  }
  inline double operator()(size_t x, size_t y) const
  {
    /* TODO: consider trashing these ifs if they stop vectorization */
    if (x >= x_max) {
      throw dimension_exception(std::string("x index too large: ") +
                                std::to_string(x));
    }
    if (y >= y_max) {
      throw dimension_exception(std::string("y index too large: ") +
                                std::to_string(y));
    }
    return mem[x * x_max + y];
  }
};

struct parameters {
  const std::vector<double> & s_0;
  const std::vector<double> & D;
  const simple_matrix & W;
};

/* TODO: make this generic enough for nonlinear sims later */
class linear_sim
{
  parameters params;

public:
  linear_sim(const parameters & p) : params(p)
  {
  }

  inline double D(size_t);
  inline double W(size_t, double, size_t);
  /* returns s_e, mean of all points at t > t_e */
  std::vector<double> simulate(double, size_t);
};
}

#include "sim.tpp"

#endif /* ___SIM_HPP___ */
