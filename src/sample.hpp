#ifndef ___LOGINTACT_SAMPLE_HPP___
#define ___LOGINTACT_SAMPLE_HPP___

#include "sim.hpp"
#include <random>

namespace LogInTact
{
namespace sample
{
struct real_interval {
  sim::RealType begin;
  sim::RealType end;
};

template <size_t n>
struct sim_param_intervals {
  typedef std::array<std::uniform_real_distribution<sim::RealType>, n>
      VectorRange;
  typedef std::array<std::uniform_real_distribution<sim::RealType>, n * n>
      MatrixRange;
  VectorRange s_0_range;
  VectorRange D_range;
  MatrixRange W_range;
  std::array<real_interval, n> s_0_int;
  std::array<real_interval, n> D_int;
  std::array<real_interval, n * n> W_int;
  sim_param_intervals(std::array<real_interval, n>,
                      std::array<real_interval, n>,
                      std::array<real_interval, n * n>);
};

/* note that Func is performed synchronously! check out io.hpp for "async"
   output to file */
constexpr size_t DEFAULT_CHUNK_SIZE = 5e4;
template <size_t n,
          size_t G,
          size_t CHUNK_SIZE = DEFAULT_CHUNK_SIZE,
          typename Func>
void do_simulated_sample(sim::RealType,
                         sim::RealType,
                         const sim_param_intervals<n> &,
                         /* if returns false, stop iterating */
                         Func);

template <size_t n, size_t G, size_t CHUNK_SIZE>
void setup_samples(std::array<sim::linear_sim<n, G>, CHUNK_SIZE> &,
                   sim_param_intervals<n> &,
                   std::mt19937 &);
}
}

#include "sample.tpp"

#endif /* ___LOGINTACT_SAMPLE_HPP___ */
