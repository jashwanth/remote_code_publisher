/////////////////////////////////////////////////////////////////////
//  ActionsAndRules.cpp - implements new parsing rules and actions //
//  ver 3.3                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Precision T7400, Vista Ultimate SP1        //
//  Application:   Prototype for CSE687 Pr1, Sp09                  //
//  Author:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////

#include "ActionsAndRules.h"
#include "../Utilities/Utilities.h"

using namespace CodeAnalysis;

Repository* Repository::instance;

#ifdef TEST_ACTIONSANDRULES

#include <iostream>
#include "ActionsAndRules.h"
#include "Tokenizer.h"
#include "SemiExpression.h"

int main(int argc, char* argv[])
{
  std::cout << "\n  Testing ActionsAndRules class\n "
            << std::string(30,'=') << std::endl;

  try
  {
    Utilities::StringHelpers::Title(
      "Actions and Rules - these construction tests are no longer valid"
    );
    //PreprocStatement pps;
    //pps.addAction(&ppq);

    //FunctionDefinition fnd;
    //PrettyPrintToQ pprtQ(resultsQ);
    //fnd.addAction(&pprtQ);

    //Toker toker("../ActionsAndRules.h");
    //SemiExp se(&toker);
    //Parser parser(&se);
    //parser.addRule(&pps);
    //parser.addRule(&fnd);
    //while(se.get())
    //  parser.parse();
    //size_t len = resultsQ.size();
    //for(size_t i=0; i<len; ++i)
    //{
    //  std::cout << "\n  " << resultsQ.front().c_str();
    //  resultsQ.pop();
    //}
    std::cout << "\n\n";
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
  }
}
#endif
