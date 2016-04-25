#include "../sim.hpp"
#include <algorithm>

using namespace LogInTact;

struct test_failure : public std::runtime_error {
  test_failure(const std::string & s) : std::runtime_error(s)
  {
  }
};

template <typename Vect>
std::string VectorString(Vect v)
{
  if (v.size() == 0) {
    return "[]";
  }
  return std::accumulate(v.begin() + 1, v.end(),
                         std::string("[") + std::to_string(v[0]),
                         [](const std::string & s, sim::RealType d) {
                           return s + ", " + std::to_string(d);
                         }) +
         "]";
}

#pragma message "FIX TESTS IN THIS FILE"

/* test that we can get the next states correctly, to some degree of accuracy */
/* void TestStates() */
/* { */
/*   using simulator = sim::linear_sim<2, 2>; */
/*   simulator lin_sim; */
/*   lin_sim.s_0     = {1, 2}; */
/*   lin_sim.D_cells = {.01, -.02}; */
/*   lin_sim.W_cells = {.5, .2, .3, -.1}; */
/*   /\* G = 2, t_f = 2 => delta = 1 *\/ */
/*   lin_sim.simulate(2, 0, 500); */
/*   simulator::Vector expected = {3.2786, 2.3692}; */
/*   if (simulator::dist_euclid(lin_sim.s_t, expected) >= .06) { */
/*     throw test_failure( */
/*         std::string("TestStates:\n") + VectorString(lin_sim.s_t) + */
/*         "\ndoes not equal:\n" + VectorString(expected) + "\nby distance: " + */
/*         std::to_string(simulator::dist_euclid(lin_sim.s_t, expected))); */
/*   } */
/* } */

/* tests that we can find t_e and s_e for a given state set correctly */
/* void TestT_E() */
/* { */
/*   using simulator = sim::linear_sim<2, 9>; */
/*   simulator lin_sim; */
/*   lin_sim.s_t = {0, 0}; */
/*   lin_sim.set_t_e(2.88); */
/*   size_t t_e = lin_sim.t_e; */
/*   if (t_e != 5) { */
/*     throw test_failure("TestT_E (for t_e):\n" + std::to_string(t_e) + */
/*                        "\ndoes not equal:\n" + std::to_string(5)); */
/*   } */
/*   simulator::Vector expected_s_e{.17, -.05}; */
/*   lin_sim.set_s_e(t_e); */
/*   auto s_e          = lin_sim.s_e; */
/*   sim::RealType res = simulator::dist_euclid(expected_s_e.data(), s_e.data()); */
/*   if (res >= .05) { */
/*     throw test_failure("TestT_E (for s_e):\n" + VectorString(expected_s_e) + */
/*                        "\nis " + std::to_string(res) + " far from:\n" + */
/*                        VectorString(s_e)); */
/*   } */
/* } */

void RunAllTests()
{
  /* TestStates(); */
  /* TestT_E(); */
}
