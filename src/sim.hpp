#ifndef ___LOGINTACT_SIM_HPP___
#define ___LOGINTACT_SIM_HPP___

#include <functional>

namespace LogInTact
{
namespace sim
{
/* throw this if the dimensions don't work out */
struct dimension_exception : std::runtime_error {
  dimension_exception(const std::string & s) : std::runtime_error(s)
  {
  }
};

typedef float RealType;

template <size_t n, size_t G>
struct linear_sim {
  static constexpr size_t VectorSize = n * sizeof(RealType);
  typedef std::array<RealType, n> Vector;
  typedef std::array<RealType, n * n> Matrix;
  Vector s_0;
  Vector D_cells;
  Matrix W_cells;
  Vector s_t;
  bool failed   = false;
  bool finished = true;

  inline static RealType dist_euclid(const Vector &, const Vector &);

  inline RealType D(size_t) const;
  inline RealType W(size_t, RealType, size_t) const;

  void simulate(RealType, RealType, size_t);
};
}
}

#include "sim.tpp"

#endif /* ___LOGINTACT_SIM_HPP___ */
