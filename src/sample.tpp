#include <omp.h>

namespace LogInTact
{
namespace sample
{
template <size_t n>
void get_field_value(const gaussian_sim<n> & dist, const size_t index)
{
  /* TODO: make sure this runs at runtime! */
  assert(index < dist.num_cells);
  
}

template <size_t n>
void setup_samples(std::vector<sim::linear_sim<n>> & data,
                   const gaussian_sim<n> dist_C_0,
                   const gaussian_sim<n * n> dist_W){
  const size_t total_num_cells = dist_C_0.num_cells * dist_W.num_cells;
  data.reserve(total_num_cells);
}

template <size_t n, size_t G, size_t CHUNK_SIZE, typename Func>
void do_simulated_sample(sim::RealType t_f,
                         sim::RealType epsilon,
                         size_t steps_to_equil,
                         const sim_param_intervals<n> & ints_other,
                         Func process_chunk)
{
  using lsimr_arr = std::array<sim::linear_sim<n, G>, CHUNK_SIZE>;
  sim_param_intervals<n> ints(ints_other); /* copy over */
  std::unique_ptr<lsimr_arr> arrays(new lsimr_arr);
  lsimr_arr & chunk = *arrays.get();
  std::random_device rd;
  std::mt19937 mtrng;
/* setup parallel(?) compute methods */
#if SIM_METHOD == SIM_USE_OPENMP
#elif SIM_METHOD == SIM_USE_OPENCL
#else
#endif
  do {
    setup_samples<n, G, CHUNK_SIZE>(chunk, ints, mtrng);
/* perform parallel(?) computation */
#if SIM_METHOD == SIM_USE_OPENMP
#pragma omp parallel for schedule(SIM_OMP_SCHEDULE_TYPE) \
    num_threads(SIM_NUM_THREADS)
    for (size_t i = 0; i < CHUNK_SIZE; ++i) {
      /* if (i == 1) { */
      /*   throw std::runtime_error(); */
      /* } */
      sim::linear_sim<n, G> & cur = chunk[i];
      cur.simulate(t_f, epsilon, steps_to_equil);
    }
#elif SIM_METHOD == SIM_USE_OPENCL
#error unimplemented!
#else
    for (size_t i = 0; i < CHUNK_SIZE; ++i) {
      sim::linear_sim<n, G> & cur = chunk[i];
      cur.simulate(t_f, epsilon, steps_to_equil);
    }
#endif
  } while (process_chunk(chunk));
/* cleanup parallel(?) computation */
#if SIM_METHOD == SIM_USE_OPENMP
#elif SIM_METHOD == SIM_USE_OPENCL
#else
#endif
}

template <size_t n, size_t G, size_t CHUNK_SIZE>
void setup_samples(std::array<sim::linear_sim<n, G>, CHUNK_SIZE> & samples,
                   sim_param_intervals<n> & ints __attribute__((unused)),
                   std::mt19937 & rng __attribute__((unused)))
{
  using RealType = sim::RealType;
/* TODO: can do this with openmp too */
#if SIM_METHOD == SIM_USE_OPENMP
#pragma omp parallel for schedule(SIM_OMP_SCHEDULE_TYPE) \
    num_threads(SIM_NUM_THREADS)
#endif
  for (size_t i = 0; i < CHUNK_SIZE; ++i) {
    sim::linear_sim<n, G> & cur = samples[i];
    auto & cur_s_0 = cur.s_0;
    /* auto & cur_s_0_range = ints.s_0_range; */
    for (size_t s_0_index = 0; s_0_index < n; ++s_0_index) {
      /* cur_s_0[s_0_index] = cur_s_0_range[s_0_index](rng); */
      cur_s_0[s_0_index] = .5 * ((RealType)((s_0_index % 2) ? 1 : -1));
    }
    auto & cur_D = cur.D_cells;
    /* auto & cur_D_range = ints.D_range; */
    for (size_t D_index = 0; D_index < n; ++D_index) {
      /* cur_D[D_index] = cur_D_range[D_index](rng); */
      cur_D[D_index] = .5 * ((RealType)((D_index % 2) ? 1 : -1));
    }
    auto & cur_W = cur.W_cells;
    /* auto & cur_W_range = ints.W_range; */
    for (size_t W_index = 0; W_index < n * n; ++W_index) {
      /* cur_W[W_index] = cur_W_range[W_index](rng); */
      cur_W[W_index] = .5 * ((RealType)((W_index % 2) ? 1 : -1));
    }
  }
}
}
}
