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
  using res    = sample::linear_sim_result<3, 50>;
  using int_rv = std::array<ri, 3>;
  using int_rm = std::array<ri, 9>;
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
  io::async_out_handle<3, 50, chunk> handle("outfile.out");
  signal(SIGINT, catch_sigint);
  sample::do_simulated_sample<3, 50, chunk>(3, .3, ints,
                                            handle.get_async_io_fun([&]() {
                                              return true;
                                              /* return terminate.load(); */
                                            }));
}
