// Test1.cpp

#include "Test1.h"
#include <iostream>
#include <sstream>

Test::Test1::Test1() {}

Test::Test1::~Test1() {}

template<typename T>
std::string toString(const T& t)
{
  std::ostringstream out;
  out << t;
  return out.str();
}

using namespace Scanner;

#ifdef TEST_TEST1

int main()
{
  SemiExp se1;
  std::cout << "\n  " << toString<size_t>(se1.length());
  std::cout << "\n\n";
}
#endif
