#include <memory>

namespace sample
{
template <size_t n>
sim_param_intervals<n>::sim_param_intervals(std::array<real_interval, n> s_0,
                                            std::array<real_interval, n> D,
                                            std::array<real_interval, n * n> W)
    : s_0_int(s_0), D_int(D), W_int(W)
{
  using parm = std::uniform_real_distribution<sim::RealType>::param_type;
  for (size_t i = 0; i < n; ++i) {
    s_0_range[i].param(parm{s_0[i].begin, s_0[i].end});
    D_range[i].param(parm{D[i].begin, D[i].end});
  }
  for (size_t j = 0; j < n * n; ++j) {
    W_range[j].param(parm{W[j].begin, W[j].end});
  }
}

template <size_t n,
          size_t G,
          size_t CHUNK_SIZE = DEFAULT_CHUNK_SIZE,
          typename Func>
void do_simulated_sample(sim::RealType t_f,
                         sim::RealType epsilon,
                         sim_param_intervals<n> & ints,
                         Func process_chunk)
{
  using lsim_arr = std::array<linear_sim_result<n, G>, CHUNK_SIZE>;
  std::unique_ptr<lsim_arr> arrays(
      new std::array<linear_sim_result<n, G>, CHUNK_SIZE>);
  std::random_device rd;
  std::mt19937 mtrng(rd());
  setup_initial_samples<n, G, CHUNK_SIZE>(*arrays.get(), ints, mtrng);
  do {
    for (size_t i = 0; i < CHUNK_SIZE; ++i) {
      sim::linear_sim<n, G> & cur = (*arrays.get())[i].lsim;
      cur.simulate(t_f, epsilon);
    }
    setup_new_samples<n, G, CHUNK_SIZE>(*arrays.get(), ints, mtrng);
  } while (process_chunk(*arrays.get()));
}

template <size_t n, size_t G, size_t CHUNK_SIZE>
void setup_initial_samples(
    std::array<linear_sim_result<n, G>, CHUNK_SIZE> & samples,
    sim_param_intervals<n> & ints,
    std::mt19937 & rng)
{
  for (size_t i = 0; i < CHUNK_SIZE; ++i) {
    linear_sim_result<n, G> & cur   = samples[i];
    cur.num_times_within_ball       = 0;
    sim::linear_sim<n, G> & cur_sim = cur.lsim;
    auto & cur_s_0                  = cur_sim.s_0;
    auto & cur_s_0_range = ints.s_0_range;
    for (size_t s_0_index = 0; s_0_index < n; ++s_0_index) {
      cur_s_0[s_0_index] = cur_s_0_range[s_0_index](rng);
    }
    auto & cur_D       = cur_sim.D_cells;
    auto & cur_D_range = ints.D_range;
    for (size_t D_index = 0; D_index < n; ++D_index) {
      cur_D[D_index] = cur_D_range[D_index](rng);
    }
    auto & cur_W       = cur_sim.W_cells;
    auto & cur_W_range = ints.W_range;
    for (size_t W_index = 0; W_index < n * n; ++W_index) {
      cur_W[W_index] = cur_W_range[W_index](rng);
    }
  }
}

template <size_t n, size_t G, size_t CHUNK_SIZE>
void setup_new_samples(
    std::array<linear_sim_result<n, G>, CHUNK_SIZE> & samples,
    sim_param_intervals<n> & ints,
    std::mt19937 & rng)
{
  for (size_t i = 0; i < CHUNK_SIZE; ++i) {
    linear_sim_result<n, G> & cur   = samples[i];
    sim::linear_sim<n, G> & cur_sim = cur.lsim;
    auto & cur_s_0                  = cur_sim.s_0;
    auto & cur_s_0_range = ints.s_0_range;
    for (size_t s_0_index = 0; s_0_index < n; ++s_0_index) {
      cur_s_0[s_0_index] = cur_s_0_range[s_0_index](rng);
    }
    auto & cur_D       = cur_sim.D_cells;
    auto & cur_D_range = ints.D_range;
    for (size_t D_index = 0; D_index < n; ++D_index) {
      cur_D[D_index] = cur_D_range[D_index](rng);
    }
    auto & cur_W       = cur_sim.W_cells;
    auto & cur_W_range = ints.W_range;
    for (size_t W_index = 0; W_index < n * n; ++W_index) {
      cur_W[W_index] = cur_W_range[W_index](rng);
    }
  }
}
}
