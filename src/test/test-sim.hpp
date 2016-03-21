#include "../sim.hpp"
#include <algorithm>

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

template <size_t n, size_t G>
typename sim::RealType
    GetFirstDistLargerThan(typename sim::RealType epsilon,
                           typename sim::linear_sim<n, G>::StateCells a,
                           typename sim::linear_sim<n, G>::StateCells b)
{
  for (size_t t = 0; t <= G; ++t) {
    typename sim::RealType res = sim::linear_sim<n, G>::dist_euclid(
        a.data() + (n * t), b.data() + (n * t));
    if (res >= epsilon) {
      return res;
    }
  }
  return 0;
}

template <size_t n, size_t G>
bool VectorsWithin(typename sim::RealType epsilon,
                   typename sim::linear_sim<n, G>::StateCells a,
                   typename sim::linear_sim<n, G>::StateCells b)
{
  return GetFirstDistLargerThan<n, G>(epsilon, a, b) == 0;
}

/* test that we can get the next states correctly, to some degree of accuracy */
void TestStates()
{
  using simulator = sim::linear_sim<2, 2>;
  simulator lin_sim;
  lin_sim.s_0     = {1, 2};
  lin_sim.D_cells = {.01, -.02};
  lin_sim.W_cells = {.5, .2, .3, -.1};
  /* G = 2, t_f = 2 => delta = 1 */
  lin_sim.fill_states(2);
  simulator::StateCells expected = {/* t = 0 (s(t = 0) == s_0) */
                                    1, 2,
                                    /* t = 1 * delta */
                                    1.91, 2.08,
                                    /* t = 2 * delta */
                                    3.2786, 2.3692};
  if (!VectorsWithin<2, 2>(.06, lin_sim.s_t, expected)) {
    throw test_failure(std::string("TestStates:\n") +
                       VectorString(lin_sim.s_t) + "\ndoes not equal:\n" +
                       VectorString(expected) +
                       "\nby first largest distance: " +
                       std::to_string(GetFirstDistLargerThan<2, 2>(
                           .01, lin_sim.s_t, expected)));
  }
}

/* tests that we can find t_e and s_e for a given state set correctly */
void TestT_E()
{
  using simulator = sim::linear_sim<2, 9>;
  simulator lin_sim;
  lin_sim.s_t = {/* t = 0 */
                 -2, 0,
                 /* t = 1 * delta */
                 -1.7, .7,
                 /* t = 2 * delta */
                 -1, 1,
                 /* t = 3 * delta */
                 -.3, 1.1,
                 /* t = 4 * delta */
                 .1, .8,
                 /* t = 5 * delta */
                 .3, .5,
                 /* t = 6 * delta */
                 .3, 0,
                 /* t = 7 * delta */
                 .25, -.25,
                 /* t = 8 * delta */
                 0, -.5,
                 /* t = 9 * delta */
                 0, 0};
  lin_sim.set_t_e(2.88);
  size_t t_e = lin_sim.t_e;
  if (t_e != 5) {
    throw test_failure("TestT_E (for t_e):\n" + std::to_string(t_e) +
                       "\ndoes not equal:\n" + std::to_string(5));
  }
  simulator::Vector expected_s_e{.17, -.05};
  lin_sim.set_s_e(t_e);
  auto s_e          = lin_sim.s_e;
  sim::RealType res = simulator::dist_euclid(expected_s_e.data(), s_e.data());
  if (res >= .05) {
    throw test_failure("TestT_E (for s_e):\n" + VectorString(expected_s_e) +
                       "\nis " + std::to_string(res) + " far from:\n" +
                       VectorString(s_e));
  }
}

void RunAllTests()
{
  TestStates();
  TestT_E();
}
