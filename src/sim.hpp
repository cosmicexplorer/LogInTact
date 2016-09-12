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

namespace math
{
using RealType = float;
template <size_t n>
using Vector = std::array<RealType, n>;
template <size_t n>
using Matrix = std::array<RealType, n * n>;

RealType Pi = 3.141592659;

template <size_t n>
inline RealType dist_euclid(const Vector<n> &, const Vector<n> &);
inline RealType gaussian(const RealType, const RealType);
template <size_t n>
inline RealType
    distance_gaussian(const Vector<n> &, const Vector<n> &, const RealType);
}

template <size_t n>
struct linear_sim {
  using namespace math;

  /* data */
  Vector C_0;
  Matrix W;
  Vector C_t;

  /* T, granularity */
  void simulate(const RealType, const size_t);

private:
  /* returns derivative */
  inline RealType W(const size_t, const RealType, const size_t) const;
};
}
}

#include "sim.tpp"

#endif /* ___LOGINTACT_SIM_HPP___ */
