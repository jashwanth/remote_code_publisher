///////////////////////////////////////////////////////////////
// UnitTest.cpp - Demonstrate unit testing framework         //
// ver 1.1                                                   //
// --------------------------------------------------------- //
// copyright © Jim Fawcett, 2012, 2013                       //
// All rights granted provided that this notice is retained  //
// --------------------------------------------------------- //
// Jim Fawcett, CSE687-Object Oriented Design, Spring 2013   //
///////////////////////////////////////////////////////////////
/*
 * This is what a test class should look like:
 */
#include "UnitTest.h"
#include <string>
#include <iostream>
using namespace Test;

 class TestStdStringClass : public TestBase<TestStdStringClass>
 {
 public:
   bool TestConstruction()
   {
     bool t1, t2;
     std::string s1("this is a string");
     t1 = (s1 == "this is a string");
     std::string s2;
     t2 = (s2.size() == 0);
     return checkResult(t1 && t2, "TestConstruction");
   }
   bool TestCopy()
   {
     bool t1;
     std::string orig = "a string";
     std::string copy = orig;
     t1 = (copy == orig && &copy != &orig);
     return checkResult(t1, "TestCopy");
   }
   bool TestWillThrowStdException()
   {
     throw std::exception("TestWillThrowStdException");
     return true;
   }
   bool TestWillThrowStructuredException()
   {
     int x=1, y=0;
     x = x/y;
     return true;
   }
   bool test()  // overriding inherited test method
   {
     bool t1, t2, t3, t4;
     t1 = doTest(&TestStdStringClass::TestConstruction);
     t2 = doTest(&TestStdStringClass::TestCopy);
     t3 = doTest(&TestStdStringClass::TestWillThrowStdException);
     t4 = doTest(&TestStdStringClass::TestWillThrowStructuredException);
     return t1 && t2 && !t3 && !t4;
   }
 private:
   // save here data needed in more than one test 
 };
#ifdef TEST_UNITTEST
int main()
{
  Title("Demonstrating UnitTest Package");

  TestStdStringClass test;
  if(test.test())
    std::cout << "\n  all tests passed";
  else
    std::cout << "\n  one or more tests failed";
  std::cout << "\n\n";
}
#endif
