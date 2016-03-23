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

int main()
{
  using ri     = LogInTact::sample::real_interval;
  using res    = LogInTact::sim::linear_sim<3, 5>;
  using int_rv = std::array<ri, 3>;
  using int_rm = std::array<ri, 9>;
  std::cout << sizeof(res) << " bytes" << std::endl;
  std::cout << static_cast<double>(5e4 * sizeof(res)) / 1e6 << " MB"
            << std::endl;
  LogInTact::sample::sim_param_intervals<3> ints(
      /* s_0 */
      int_rv{ri{0, 1}, ri{0, 1}, ri{0, 1}},
      /* D */
      int_rv{ri{0, 1}, ri{0, 1}, ri{0, 1}},
      /* W */
      int_rm{ri{0, 1}, ri{0, 1}, ri{0, 1}, ri{0, 1}, ri{0, 1}, ri{0, 1},
             ri{0, 1}, ri{0, 1}, ri{0, 1}});
  constexpr size_t chunk = 5;
  /* TODO: read intervals from file, get output file from argv */
  size_t num_chunks = 0;
  signal(SIGINT, catch_sigint);
  try {
    LogInTact::io::run_sim_async<3, 5, chunk>(
        3, .3, ints, "outfile.out", [&]() {
          ++num_chunks;
          return num_chunks < 5 or terminate.load();
        });
  } catch (LogInTact::io::io_exception & e) {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
}
