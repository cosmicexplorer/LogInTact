#include <iostream>

#include "sim.hpp"
#include "linear.hpp"

/* define input of system <s_0, D, W> */
/* simulate with granularity G up to timestep t_f */
/* dump to some file */
/* ??? */
/* $$$ */
int main()
{
  using sim::range;
  sim::linear lin_sim(std::vector<range>{range{-1, 1, 5}},
                      std::vector<range>{range{-1, 1, 5}},
                      std::vector<range>{range{-1, 1, 5}});
  lin_sim.simulate(5, 50, [](size_t, double, const std::vector<double> & v) {
    for (auto i : v) {
      std::cout << i << ',';
    }
    std::cout << '\n';
  });
}
