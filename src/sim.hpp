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

typedef double RealType;

template <size_t n, size_t G>
struct linear_sim {
  static constexpr size_t VectorSize = n * sizeof(RealType);
  typedef std::array<RealType, n> Vector;
  typedef std::array<RealType, n * n> Matrix;
  typedef std::array<RealType, n *(G + 1)> StateCells;
  Vector s_0;
  Vector D_cells;
  Matrix W_cells;
  StateCells s_t;

  inline static RealType dist_euclid(const RealType *, const RealType *);
  /* n.b.: don't screw up the indexing here! */
  inline static Vector average_position(const RealType *, size_t);

  inline RealType D(size_t);
  inline RealType W(size_t, RealType, size_t);
  inline void fill_states(RealType);
  /* call these only after fill_states called! */
  /* returns index into s_t, NOT the actual time t_e */
  inline void set_t_e(RealType); /* may change failed */
  inline void set_s_e(size_t);
  size_t t_e;
  Vector s_e;
  bool failed = true;
  /* finds s_e, mean of all points at t > t_e */
  void simulate(RealType, RealType);
};
}

#include "sim.tpp"

#endif /* ___SIM_HPP___ */
