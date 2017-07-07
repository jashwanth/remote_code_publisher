#ifndef PARSER_H
#define PARSER_H
/////////////////////////////////////////////////////////////////////
//  Parser.h - Analyzes C++ and C# language constructs             //
//  ver 1.5                                                        //
//  Language:      Visual C++, Visual Studio 2015                  //
//  Platform:      Dell XPS 8900, Windows 10                       //
//  Application:   Prototype for CSE687 Pr1, Sp09, ...             //
//  Author:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module defines a Parser class.  Its instances collect 
  semi-expressions from a file for analysis.  Analysis consists of
  applying a set of rules to the semi-expression, and for each rule
  that matches, invokes a set of one or more actions.

  Public Interface:
  =================
  Toker t(someFile);              // create tokenizer instance
  SemiExp se(&t);                 // create a SemiExp attached to tokenizer
  Parser parser(se);              // now we have a parser
  Rule1 r1;                       // create instance of a derived Rule class
  Action1 a1;                     // create a derived action
  r1.addAction(&a1);              // register action with the rule
  parser.addRule(&r1);            // register rule with parser
  while(se.getSemiExp())          // get semi-expression
    parser.parse();               //   and parse it

  Build Process:
  ==============
  Required files
    - Parser.h, Parser.cpp, SemiExpression.h, SemiExpression.cpp,
      tokenizer.h, tokenizer.cpp,
      ActionsAndRules.h, ActionsAndRules.cpp,
      ConfigureParser.h, ConfigureParser.cpp
  Build commands (either one)
    - devenv Parser.sln
    - cl /EHsc /DTEST_PARSER parser.cpp semiexpression.cpp tokenizer.cpp \
         ActionsAndRules.cpp ConfigureParser.cpp /link setargv.obj

  Maintenance History:
  ====================
  ver 1.5 : 19 Aug 16
  - added trimming of semis in Parser::next()
  - changed IRule interface to accept const pointer
  - Now stops processing rules for a SemiExp if a rule returns IRule::Stop.
    It continues by collecting another SemiExp for testing.
  ver 1.4 : 15 Feb 16
  - removed all folding rules code
  - changed solution folder layout which caused changes in many of the
    include paths
  - small modifications to one of the derived actions
  - now preface (new) Toker and SemiExp with Scanner namespace
  ver 1.3 : 02 Jun 11
  - added use of folding rules in get();
  ver 1.2 : 17 Jan 09
  - modified to accept pointer to interface ITokCollection instead of
    SemiExpression
  ver 1.1 : 17 Sep 07
  - cosmetic modifications to comments on this page
  ver 1.0 : 12 Jan 06
  - first release

*/

#include <string>
#include <iostream>
#include <vector>
#include "../SemiExp/itokcollection.h"

namespace CodeAnalysis
{
  ///////////////////////////////////////////////////////////////
  // abstract base class for parsing actions
  //   - when a rule succeeds, it invokes any registered action

  class Parser;

  class IBuilder
  {
  public:
    virtual ~IBuilder() {}
    virtual Parser* Build() = 0;
  };

  ///////////////////////////////////////////////////////////////
  // abstract base class for parsing actions
  //   - when a rule succeeds, it invokes any registered action

  class IAction
  {
  public:
    virtual ~IAction() {}
    virtual void doAction(const Scanner::ITokCollection* pTc) = 0;
  };

  ///////////////////////////////////////////////////////////////
  // abstract base class for parser language construct detections
  //   - rules are registered with the parser for use

  class IRule
  {
  public:
    static const bool Continue = true;
    static const bool Stop = false;
    virtual ~IRule() {}
    void addAction(IAction* pAction);
    void doActions(const Scanner::ITokCollection* pTc);
    virtual bool doTest(const Scanner::ITokCollection* pTc) = 0;
  protected:
    std::vector<IAction*> actions;
  };

  class Parser
  {
  public:
    Parser(Scanner::ITokCollection* pTokCollection);
    ~Parser();
    void addRule(IRule* pRule);
    bool parse();
    bool next();
  private:
    Scanner::ITokCollection* pTokColl;
    std::vector<IRule*> rules;
  };

  inline Parser::Parser(Scanner::ITokCollection* pTokCollection) : pTokColl(pTokCollection) {}

  inline Parser::~Parser() {}
}
#endif
