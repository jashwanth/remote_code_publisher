#ifndef SCOPESTACK_H
#define SCOPESTACK_H
/////////////////////////////////////////////////////////////////////////////
// ScopeStack.h - implements template stack holding specified element type //
// ver 2.2                                                                 //
// Language:      Visual C++ 2010, SP1                                     //
// Platform:      Dell Precision T7400, Win 7 Pro SP1                      //
// Application:   Code Analysis Research                                   //
// Author:        Jim Fawcett, CST 4-187, Syracuse University              //
//                (315) 443-3948, jfawcett@twcny.rr.com                    //
/////////////////////////////////////////////////////////////////////////////
/*
  Package Operations:
  ===================
  This package contains one class: ScopeStack<element>.  The element type
  is an application specific type designed to hold whatever information you
  need to stack.

  Throws std::exception if stack is popped or peeked when empty.

  Public Interface:
  =================
  ScopeStack<element> stack;
  // define some element type
  element elem;
  stack.push(elem);
  element popped = stack.pop();

  Required Files:
  ===============
  ScopeStack.h, ScopeStack.cpp

  Build Command:
  ==============
  devenv ScopeStack.sln /rebuild debug

  Maintenance History:
  ====================
  ver 2.2 : 29 Oct 2016
  - added throw when popping or peeking empty stack
  ver 2.1 : 02 Jun 2011
  - changed ScopeStack type to single template argument intended to be
    an application specific type (usually a struct that holds whatever
    is needed)
  ver 2.0 : 02 Feb 2011
  - fixed bug in pop() method found by Drumil Bhattad
  ver 1.0 : 31 Jan 2011
  - first release
*/
#include <list>
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"

// element is a application specific type.  It must provide a function:
// std::string show() that returns a string display of its parts. see
// test stub for an example.

namespace CodeAnalysis
{
  template<typename element>
  class ScopeStack {
  public:
    using Rslt = Logging::StaticLogger<0>;    // show program results
    using Demo = Logging::StaticLogger<1>;    // show demonstration outputs
    using Dbug = Logging::StaticLogger<2>;    // show debugging outputs
    typename typedef std::list<element>::iterator iterator;

    ScopeStack();
    void push(const element& item);
    element pop();
    element& top();
    element& predOfTop();
    size_t size();
    iterator begin();
    iterator end();
  private:
    std::list<element> stack;
    element dbElement;
  };

  template<typename element>
  ScopeStack<element>::ScopeStack() {}

  template<typename element>
  typename inline size_t ScopeStack<element>::size() { return stack.size(); }

  template<typename element>
  typename inline ScopeStack<element>::iterator ScopeStack<element>::begin() { return stack.begin(); }

  template<typename element>
  typename inline ScopeStack<element>::iterator ScopeStack<element>::end() { return stack.end(); }

  template<typename element>
  void ScopeStack<element>::push(const element& item)
  {
    Demo::flush();
    stack.push_back(item);
    Dbug::write("\n--- stack size = " + Utilities::Converter<size_t>::toString(size()) + " ---");
    Dbug::flush();
  }

  template<typename element>
  element ScopeStack<element>::pop()
  {
    if (stack.size() == 0)
    {
      throw std::exception("-- attempting to pop empty stack --");
    }
    element item = stack.back();
    stack.pop_back();    
    Dbug::write("\n--- stack size = " + Utilities::Converter<size_t>::toString(size()) + " ---");
    Dbug::flush();

    return item;
  }

  template<typename element>
  element& ScopeStack<element>::top()
  {
    if (stack.size() == 0)
    {
      throw std::exception("-- attempting to peek top on empty stack --");
    }
    return stack.back();
  }

  template<typename element>
  element& ScopeStack<element>::predOfTop()
  {
    if (size() < 2)
    {
      throw std::exception("-- predOfTop() called on stack with less than two elements --");
    }
    iterator iter = --(--end());
    return *iter;
  }

  template<typename element>
  void showStack(ScopeStack<element>& stack, bool indent = true)
  {
    if (stack.size() == 0)
    {
      std::cout << "\n  ScopeStack is empty";
      return;
    }
    ScopeStack<element>::iterator iter = stack.begin();
    while (iter != stack.end())
    {
      std::string strIndent = std::string(2 * stack.size(), ' ');
      if (!indent)
        strIndent = "";
      element temp = *iter;
      std::cout << "\n  " << strIndent << temp->show();
      ++iter;
    }
  }

  template<typename element>
  void showStack(ScopeStack<element*>& stack, bool indent = true)
  {
    if (stack.size() == 0)
    {
      std::cout << "\n  ScopeStack is empty";
      return;
    }
    ScopeStack<element*>::iterator iter = stack.begin();
    while (iter != stack.end())
    {
      std::string strIndent = std::string(2 * stack.size(), ' ');
      if (!indent)
        strIndent = "";
      std::cout << "\n  " << strIndent << ((*iter)->show());
      ++iter;
    }
  }
  //----< walk tree of element nodes >---------------------------------

  template<typename element>
  void TreeWalk(element* pItem, bool details = false)
  {
    static std::string path;
    static size_t indentLevel = 0;
    std::ostringstream out;
    out << "\n  " << std::string(2 * indentLevel, ' ') << pItem->show();
    std::cout << out.str();
    auto iter = pItem->children_.begin();
    ++indentLevel;
    while (iter != pItem->children_.end())
    {
      TreeWalk(*iter);
      ++iter;
    }
    --indentLevel;
  }
}
#endif
