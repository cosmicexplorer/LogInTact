#include <iostream>
#include "test-sim.hpp"

int main()
{
  try {
    RunAllTests();
    std::cout << "all tests passed!" << std::endl;
  } catch (test_failure & e) {
    std::cerr << e.what() << std::endl;
  }
}
