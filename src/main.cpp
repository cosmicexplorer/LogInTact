#include "io.hpp"

#include <iostream>
#include <atomic>
#include <csignal>

template <typename Vect>
std::string PrintVect(const Vect & v)
{
  std::vector<std::string> ret;
  for (auto el : v) {
    ret.push_back(std::to_string(el));
    ret.push_back(",");
  }
  ret.pop_back();
  return std::accumulate(ret.begin(), ret.end(), std::string("["),
                         [](auto a, auto b) { return a + b; }) +
         "]";
}

std::atomic<bool> terminate(false);

void catch_sigint(int)
{
  terminate.store(true);
}

int main(int argc, char ** argv)
{
  if (argc <= 1) {
    throw std::runtime_error("NOT ENOUGH ARGS: ./sim FILE");
  }
  using ri     = LogInTact::sample::real_interval;
  using res    = LogInTact::sim::linear_sim<3, SIM_CHUNK_SIZE>;
  using int_rv = std::array<ri, 3>;
  using int_rm = std::array<ri, 9>;
  std::cout << sizeof(res) << " bytes" << std::endl;
  LogInTact::sample::sim_param_intervals<3> ints(
      /* s_0 */
      int_rv{ri{0, 1}, ri{0, 1}, ri{0, 1}},
      /* D */
      int_rv{ri{0, 1}, ri{0, 1}, ri{0, 1}},
      /* W */
      int_rm{ri{0, 1}, ri{0, 1}, ri{0, 1}, ri{0, 1}, ri{0, 1}, ri{0, 1},
             ri{0, 1}, ri{0, 1}, ri{0, 1}});
  /* TODO: read intervals from file, get output file from argv */
  size_t num_chunks = 0;
  signal(SIGINT, catch_sigint);
  const size_t steps_to_equil  = 50;
  constexpr size_t granularity = 500;
  std::cerr << SIM_CHUNK_SIZE << ',' << SIM_CHUNK_NUM << std::endl;
  try {
    LogInTact::io::run_sim_async<3, granularity, SIM_CHUNK_SIZE>(
        3, 0, steps_to_equil, ints, argv[1], [&]() {
          ++num_chunks;
          return num_chunks < SIM_CHUNK_NUM or terminate.load();
        });
  } catch (LogInTact::io::io_exception & e) {
    std::cerr << "WTF: " << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
  return 0;
}
