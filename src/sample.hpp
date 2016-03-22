#ifndef ___SAMPLE_HPP___
#define ___SAMPLE_HPP___

#include "sim.hpp"
#include <random>

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

template <size_t n, size_t G>
struct linear_sim_result {
  sim::linear_sim<n, G> lsim;
  size_t num_times_within_ball = 0;
};

constexpr size_t DEFAULT_CHUNK_SIZE             = 5e4;
template <size_t n, size_t G, size_t CHUNK_SIZE = DEFAULT_CHUNK_SIZE>
void do_simulated_sample(
    sim::RealType,
    sim::RealType,
    const sim_param_intervals<n> &,
    /* if returns false, stop iterating */
    const std::function<bool(const linear_sim_result<n, G> * const, size_t)> &);

template <size_t n, size_t G, size_t CHUNK_SIZE>
void setup_initial_samples(const linear_sim_result<n, G> * const,
                           const sim_param_intervals<n> &,
                           std::uniform_real_distribution<sim::RealType> &);

template <size_t n, size_t G, size_t CHUNK_SIZE>
void setup_new_samples(const linear_sim_result<n, G> * const,
                       const sim_param_intervals<n> &,
                       std::uniform_real_distribution<sim::RealType> &);
}

#include "sample.tpp"

#endif /* ___SAMPLE_HPP___ */
