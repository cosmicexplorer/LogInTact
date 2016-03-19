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
  sim::parameters p{{0}, {-.5}, sim::simple_matrix(1, 1, {1})};
  sim::linear_sim lin_sim(p);
  std::cout << lin_sim.simulate(5, 50, .1)[0] << std::endl;
}
