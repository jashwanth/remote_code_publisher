#pragma once
// Test1.h

#include "../SemiExp/SemiExp.h"
#include "../Tokenizer/Tokenizer.h"

namespace Test
{
  class Test1
  {
  public:
    Test1();
    Test1(const Test1&) = delete;
    Test1& operator=(const Test1&) = default;
    ~Test1();
    void doThing1() { /* don't do anything */ }
  private:
    Scanner::SemiExp se;
  };
}