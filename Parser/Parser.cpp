/////////////////////////////////////////////////////////////////////
//  Parser.cpp - Analyzes C++ language constructs                  //
//  ver 1.5                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell XPS 8900, Windows 10                       //
//  Application:   Prototype for CSE687 Pr1, Sp09, ...             //
//  Author:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include "../Utilities/Utilities.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"
#include "Parser.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"

using namespace CodeAnalysis;
using namespace Scanner;
using namespace Utilities;
using Demo = Logging::StaticLogger<1>;

//----< register parsing rule >--------------------------------

void Parser::addRule(IRule* pRule)
{
  rules.push_back(pRule);
}
//----< get next ITokCollection >------------------------------

bool Parser::next() 
{ 
  bool succeeded = pTokColl->get();

  if (!succeeded)
  {
    return false;
  }
  std::string debug = pTokColl->show();
  //GrammarHelper::showParseDemo("get SemiExp: ", *pTokColl);

  pTokColl->trim();

  return true;
}

//----< parse the SemiExp by applying all rules to it >--------

bool Parser::parse()
{
  for (size_t i = 0; i<rules.size(); ++i)
  {
    std::string debug = pTokColl->show();

    bool doWhat = rules[i]->doTest(pTokColl);
    if (doWhat == IRule::Stop)
      break;
  }
  return true;
}
//----< register action with a rule >--------------------------

void IRule::addAction(IAction *pAction)
{
  actions.push_back(pAction);
}
//----< invoke all actions associated with a rule >------------

void IRule::doActions(const ITokCollection* pTokColl)
{
  if(actions.size() > 0)
    for(size_t i=0; i<actions.size(); ++i)
      actions[i]->doAction(pTokColl);
}

//----< test stub >--------------------------------------------

#include "../FileSystem/FileSystem.h"

#ifdef TEST_PARSER

#include <queue>
#include <string>
#define Util StringHelper

int main(int argc, char* argv[])
{
  Util::Title("Testing Parser Class");
  putline();

  // collecting tokens from files, named on the command line

  if(argc < 2)
  {
    std::cout 
      << "\n  please enter name of file to process on command line\n\n";
    return 1;
  }

  for(int i=1; i<argc; ++i)
  {
    std::string fileSpec = FileSystem::Path::getFullFileSpec(argv[i]);
    std::string msg = "Processing file" + fileSpec;
    Util::title(msg);
    putline();

    ConfigParseForCodeAnal configure;
    Parser* pParser = configure.Build();
    try
    {
      if(pParser)
      {
        if(!configure.Attach(argv[i]))
        {
          std::cout << "\n  could not open file " << fileSpec << std::endl;
          continue;
        }
      }
      else
      {
        std::cout << "\n\n  Parser not built\n\n";
        return 1;
      }
      // now that parser is built, use it

      while(pParser->next())
        pParser->parse();
      std::cout << "\n";

      // show AST
      Repository* pRepo = Repository::getInstance();
      ASTNode* pGlobalScope = pRepo->getGlobalScope();
      TreeWalk(pGlobalScope);
    }
    catch(std::exception& ex)
    {
      std::cout << "\n\n    " << ex.what() << "\n\n";
    }
  }
  std::cout << "\n";
}

#endif
