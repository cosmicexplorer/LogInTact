#include <memory>

namespace sample
{
template <size_t n>
sim_param_intervals<n>::sim_param_intervals(std::array<real_interval, n> s_0,
                                            std::array<real_interval, n> D,
                                            std::array<real_interval, n * n> W)
    : s_0_int(s_0), D_int(D), W_int(W)
{
  for (size_t i = 0; i < n; ++i) {
    s_0_range[i].param({s_0[i].begin, s_0[i].end});
    D_range[i].param({D[i].begin, D[i].end});
  }
  for (size_t j = 0; j < n * n; ++j) {
    W_range[i].param({W[j].begin, W[j].end});
  }
}

template <size_t n, size_t G, size_t CHUNK_SIZE = DEFAULT_CHUNK_SIZE>
void do_simulated_sample(
    sim::RealType t_f,
    sim::RealType epsilon,
    const sim_param_intervals<n> & ints,
    const std::function<bool(const linear_sim_result<n, G> * const, size_t)> &
        process_chunk)
{
  using lsim                     = typename linear_sim_result<n, G>;
  std::unique_ptr<lsim[]> arrays = std::make_unique<lsim[]>(CHUNK_SIZE);
  std::random_device rd;
  std::mt19937 mtrng(rd());
  setup_initial_samples<n, G, CHUNK_SIZE>(arrays.get(), ints, mtrng);
  if (!process_chunk(arrays, CHUNK_SIZE)) {
    return;
  }
  do {
    for (size_t i = 0; i < CHUNK_SIZE; ++i) {
      arrays[i].simulate(t_f, epsilon);
    }
    setup_new_samples<n, G, CHUNK_SIZE>(arrays.get(), ints, mtrng);
  } while (process_chunk(arrays, CHUNK_SIZE));
}

template <size_t n, size_t G, size_t CHUNK_SIZE>
void setup_initial_samples(const linear_sim_result<n, G> * const samples,
                           const sim_param_intervals<n> & ints,
                           std::mt19937 & rng)
{
  for (size_t i = 0; i < CHUNK_SIZE; ++i) {
    const linear_sim_result<n, G> & cur                    = samples[i];
    const sim::linear_sim<n, G> & cur_sim                  = cur.lsim;
    cur.num_times_within_ball                              = 0;
    const typename sim::linear_sim<n, G>::Vector & cur_s_0 = cur_sim.s_0;
    const typename sim_param_intervals<n>::VectorRange & cur_s_0_range =
        ints.s_0_range;
    for (size_t s_0_index = 0; s_0_index < n; ++s_0_index) {
      cur_s_0[s_0_index] = cur_s_0_range[s_0_index](rng);
    }
    const typename sim::linear_sim<n, G>::Vector & cur_D = cur_sim.D;
    const typename sim_param_intervals<n>::VectorRange & cur_D_range =
        ints.D_range;
    for (size_t D_index = 0; D_index < n; ++D_index) {
      cur_D[D_index] = cur_D_range[D_index](rng);
    }
    const typename sim::linear_sim<n, G>::Matrix & cur_W = cur_sim.W;
    const typename sim_param_intervals<n>::MatrixRange & cur_W_range =
        ints.W_range;
    for (size_t W_index = 0; W_index < n * n; ++W_index) {
      cur_W[W_index] = cur_W_range[W_index](rng);
    }
  }
}

template <size_t n, size_t G, size_t CHUNK_SIZE>
void setup_new_samples(const linear_sim_result<n, G> * const samples,
                       const sim_param_intervals<n> & ints,
                       std::uniform_real_distribution<sim::RealType> & rng)
{
  for (size_t i = 0; i < CHUNK_SIZE; ++i) {
    const linear_sim_result<n, G> & cur                    = samples[i];
    const sim::linear_sim<n, G> & cur_sim                  = cur.lsim;
    const typename sim::linear_sim<n, G>::Vector & cur_s_0 = cur_sim.s_0;
    const typename sim_param_intervals<n>::VectorRange & cur_s_0_range =
        ints.s_0_range;
    for (size_t s_0_index = 0; s_0_index < n; ++s_0_index) {
      cur_s_0[s_0_index] = cur_s_0_range[s_0_index](rng);
    }
    const typename sim::linear_sim<n, G>::Vector & cur_D = cur_sim.D;
    const typename sim_param_intervals<n>::VectorRange & cur_D_range =
        ints.D_range;
    for (size_t D_index = 0; D_index < n; ++D_index) {
      cur_D[D_index] = cur_D_range[D_index](rng);
    }
    const typename sim::linear_sim<n, G>::Matrix & cur_W = cur_sim.W;
    const typename sim_param_intervals<n>::MatrixRange & cur_W_range =
        ints.W_range;
    for (size_t W_index = 0; W_index < n * n; ++W_index) {
      cur_W[W_index] = cur_W_range[W_index](rng);
    }
  }
}
}
