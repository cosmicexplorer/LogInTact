#include <iostream>

#include "sim.hpp"

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
  using sim::range;
  sim::linear lin_sim(std::vector<range>{range{-1, 1, 5}},
                      std::vector<range>{range{-1, 1, 5}},
                      std::vector<range>{range{-1, 1, 5}});
  lin_sim.simulate(5, 50);
}