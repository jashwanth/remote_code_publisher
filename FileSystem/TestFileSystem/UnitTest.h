#ifndef UNITTEST_H
#define UNITTEST_H
///////////////////////////////////////////////////////////////
// UnitTest.cpp - Demonstrate unit testing framework         //
// ver 1.1                                                   //
// --------------------------------------------------------- //
// copyright © Jim Fawcett, 2012, 2013                       //
// All rights granted provided that this notice is retained  //
// --------------------------------------------------------- //
// Jim Fawcett, CSE687-Object Oriented Design, Spring 2013   //
///////////////////////////////////////////////////////////////

#include <string>
#include <iostream>

namespace Test
{
  void Title(const std::string& title, char ch='=')
  {
    std::cout << "\n  " << title;
    std::cout << "\n " << std::string(title.size() + 2, ch);
  }

  template <typename T>
  class TestBase
  {
  public:

    typedef bool (T::*Test) ();

    virtual ~TestBase() {}

    // Your test driver implements this for each major test
    // - call a sequence of doTest(TestN)s
    //-----------------------------------------------------
    virtual bool test()=0;

    // Executes test functions inside a try block
    //-------------------------------------------
    bool doTest(Test atest) 
    { 
      try
      {
        return (dynamic_cast<T*>(this)->*atest)();
      }
      catch (std::exception& ex)
      {
        std::cout << "\n  failed: " << ex.what();
        return false;
      }
      catch(...)
      {
        std::cout << "\n  failed: thrown structured exception";
        return false;
      }
    }
    // Call this in your test functions to save duplicating output statements
    //-----------------------------------------------------------------------
    bool checkResult(bool predicate, std::string testname)
    {
      std::string name = std::string(typeid(*this).name()) + "::" + testname;
      name = name.substr(6, name.size() - 6);
      if(predicate)
        return passed(name);
      else
        return failed(name);
    }
  protected:
    bool passed(const std::string& name)
    {
      std::cout << "\n  passed: " << name;
      return true;
    }
    bool failed(const std::string& name)
    {
      std::cout << "\n  failed: " << name;
      return false;
    }
  };
}
#endif
