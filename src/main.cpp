#include <iostream>

#include "sample.hpp"

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
  sample::do_simulated_sample<3, 50, chunk>(
      3, .3, ints, [&](const auto & results) {
        for (size_t i = 0; i < results.size(); ++i) {
          const sim::linear_sim<3, 50> & cur = results[i].lsim;
          if (!cur.failed) {
            std::cout << PrintVect(cur.s_e) << std::endl;
          }
        }
        return false;
      });
}
