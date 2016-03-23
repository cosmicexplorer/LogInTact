#include <iostream>
#include <atomic>
#include <csignal>

#include "io.hpp"

/* checkout branch old-generic for how we were going to make this a little more
   generic with respect to linear/nonlinear systems before we went ham on
   performance */

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
  using ri     = sample::real_interval;
  using res    = sim::linear_sim<3, 5>;
  using int_rv = std::array<ri, 3>;
  using int_rm = std::array<ri, 9>;
  std::cout << sizeof(res) << std::endl;
  std::cout << static_cast<double>(5e4 * sizeof(res)) / 1e6 << std::endl;
  sample::sim_param_intervals<3> ints(
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
  /* TODO: catch SIGCHLD */
  io::run_sim_async<3, 5, chunk>(3, .3, ints, [&]() {
    ++num_chunks;
    return num_chunks < 5;
    /* return terminate.load(); */
  });
}
