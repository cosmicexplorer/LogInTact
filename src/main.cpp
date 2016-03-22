#include <iostream>

#include "sample.hpp"

/* checkout branch old-generic for how we were going to make this a little more
   generic with respect to linear/nonlinear systems before we went ham on
   performance */

/* define input of system <s_0, D, W> */
/* simulate with granularity G up to timestep t_f */
/* dump to some file */
/* ??? */
/* $$$ */
int main()
{
  using ri  = sample::real_interval;
  using res = sample::linear_sim_result<3, 50>;
  std::cout << static_cast<double>(5e4 * sizeof(res)) / 1e6 << std::endl;
  sample::sim_param_intervals<3> ints(
      /* s_0 */
      std::array<ri, 3>{ri{0, 1}, ri{0, 1}, ri{0, 1}},
      /* D */
      std::array<ri, 3>{ri{0, 1}, ri{0, 1}, ri{0, 1}},
      /* W */
      std::array<ri, 9>{ri{0, 1}, ri{0, 1}, ri{0, 1}, ri{0, 1}, ri{0, 1},
                        ri{0, 1}, ri{0, 1}, ri{0, 1}, ri{0, 1}});
  sample::do_simulated_sample<3, 50>(
      3, .3, ints, [](const res * const results, size_t num) {
        for (size_t i = 0; i < num; ++i) {
          const sim::linear_sim<3, 50> & cur = results[i].lsim;
          if (!cur.failed) {
            std::cout << '[';
            for (auto el : cur.s_e) {
              std::cout << ',' << el;
            }
            std::cout << ']' << std::endl;
          }
        }
        return true;
      });
}
