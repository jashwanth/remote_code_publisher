///////////////////////////////////////////////////////////////////////////////
// ScopeStack.cpp - implements template stack holding specified element type //
// ver 2.2                                                                   //
// Language:      Visual C++ 2010, SP1                                       //
// Platform:      Dell Precision T7400, Win 7 Pro SP1                        //
// Application:   Code Analysis Research                                     //
// Author:        Jim Fawcett, CST 4-187, Syracuse University                //
//                (315) 443-3948, jfawcett@twcny.rr.com                      //
///////////////////////////////////////////////////////////////////////////////

//----< test stub >------------------------------------------------------

#ifdef TEST_SCOPESTACK

#include "ScopeStack.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using namespace CodeAnalysis;

struct element
{
  std::string type;
  std::string name;
  size_t startLineCount = 0;
  size_t endLineCount = 0;
  std::string path_;
  std::string show()
  {
    std::ostringstream temp;
    temp << "(";
    temp << type;
    temp << ", ";
    temp << name;
    temp << ", ";
    temp << startLineCount;
    temp << ", ";
    temp << endLineCount;
    temp << ")"; 
    return temp.str();
  }
  std::vector<element*> children_;
};

int main()
{
  ScopeStack<element*>::Rslt::attach(&std::cout);
  ScopeStack<element*>::Rslt::start();

  using Utils = Utilities::StringHelper;

  Utils::Title("Testing Scope Stack");

  Utils::title("pushing items onto ScopeStack instance after adding startLineCount");

  ScopeStack<element*> testStack;
  element* pItem = new element;

  pItem->type = "function";
  pItem->name = "fun1";
  pItem->startLineCount = 33;
  testStack.push(pItem);

  pItem = new element;
  pItem->type = "if";
  pItem->name = "";
  pItem->startLineCount = 38;
  (testStack.top()->children_).push_back(pItem);
  testStack.push(pItem);

  pItem = new element;
  pItem->type = "for";
  pItem->name = "";
  pItem->startLineCount = 40;
  (testStack.top()->children_).push_back(pItem);
  testStack.push(pItem);

  showStack<element>(testStack, false);
  std::cout << std::endl;

  Utils::title("top and predecessor are:");
  std::cout << "\n  " << testStack.top()->show();
  std::cout << "\n  " << testStack.predOfTop()->show();
  std::cout << "\n";

  Utils::title("Popping two items off ScopeStack after adding endLineCount");

  testStack.top()->endLineCount = 50;
  std::cout << "\n  " << testStack.pop()->show();
  testStack.top()->endLineCount = 55;
  std::cout << "\n  " << testStack.pop()->show();
  std::cout << "\n";

  Utils::title("pushing another item onto ScopeStack after adding startLineElement");
  pItem = new element;
  pItem->type = "while";
  pItem->name = "";
  pItem->startLineCount = 60;
  std::cout << "\n  " << pItem->show();
  (testStack.top()->children_).push_back(pItem);
  testStack.push(pItem);
  std::cout << "\n";

  Utils::title("stack now contains:");
  showStack<element>(testStack, false);
  std::cout << "\n";

  Utils::title("popping last elements off stack after adding endLineCount");
  testStack.top()->endLineCount = 70;
  element* pTop = testStack.pop();
  std::cout << "\n  " << pTop->show();
  testStack.top()->endLineCount = 75;
  pTop = testStack.pop();
  std::cout << "\n  " << pTop->show();
  std::cout << "\n";

  Utils::title("walking simulated AST tree:");
  TreeWalk(pTop);
  std::cout << "\n\n";
}

#endif
