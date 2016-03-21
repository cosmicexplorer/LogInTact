#include "../sim.hpp"
#include <algorithm>

struct test_failure : public std::runtime_error {
  test_failure(const std::string & s) : std::runtime_error(s)
  {
  }
};

template <size_t n, size_t G>
std::string VectorString(const typename sim::linear_sim<n, G>::Vector & v)
{
  if (v.size() == 0) {
    return "[]";
  }
  return std::accumulate(v.begin() + 1, v.end(),
                         std::string("[") + std::to_string(v[0]),
                         [](const std::string & s, sim::RealType d) {
                           return s + ',' + std::to_string(d);
                         }) +
         "]";
}

void TestMultiple()
{
  using simulator = sim::linear_sim<2, 4>;
  simulator lin_sim;
  lin_sim.s_0                = {1, 2};
  lin_sim.D_cells            = {.01, -.02};
  lin_sim.W_cells            = {.5, .2, .3, -.1};
  simulator::Vector res      = lin_sim.simulate(4, 1);
  simulator::Vector expected = {.2, 3};
  if (res != expected) {
    throw test_failure(std::string("failure: ") + VectorString<2, 4>(res) +
                       " does not equal " + VectorString<2, 4>(expected));
  }
}

void RunAllTests()
{
  TestMultiple();
}
