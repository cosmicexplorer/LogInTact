#include "../sim.hpp"

struct test_failure : public std::runtime_error {
  test_failure(const std::string & s) : std::runtime_error(s)
  {
  }
};

void TestSingle()
{
}

void TestMultiple()
{
}

void RunAllTests()
{
  TestSingle();
  TestMultiple();
}
