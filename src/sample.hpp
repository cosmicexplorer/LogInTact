#ifndef ___LOGINTACT_SAMPLE_HPP___
#define ___LOGINTACT_SAMPLE_HPP___

#include "sim.hpp"

namespace LogInTact
{
namespace sample
{
using namespace math;

template <size_t n>
struct gaussian_sim {
  Vector<n> & mean;
  RealType sigma;
  size_t num_cells;
};

template <size_t n>
inline void get_field_value(const gaussian_sim<n> &, const size_t);

template <size_t n>
void setup_samples(std::vector<sim::linear_sim<n>> &,
                   const gaussian_sim<n>,
                   const gaussian_sim<n * n>);

template <size_t n>
void simulate_samples(std::vector<sim::linear_sim<n>> &);
}
}

#include "sample.tpp"

#endif /* ___LOGINTACT_SAMPLE_HPP___ */
