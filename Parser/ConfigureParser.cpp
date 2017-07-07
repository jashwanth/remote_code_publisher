/////////////////////////////////////////////////////////////////////
//  ConfigureParser.cpp - builds and configures parsers            //
//  ver 3.2                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2005                                 //
//  Platform:      Dell Dimension 9150, Windows XP SP2             //
//  Application:   Prototype for CSE687 Pr1, Sp06                  //
//  Author:        Jim Fawcett, CST 2-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////

#include <fstream>
#include "Parser.h"
#include "../SemiExp/SemiExp.h"
#include "../Tokenizer/Tokenizer.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"

using namespace CodeAnalysis;
using namespace Scanner;

//----< destructor releases all parts >------------------------------

ConfigParseForCodeAnal::~ConfigParseForCodeAnal()
{
  // when Builder goes out of scope, everything must be deallocated

  delete pHandleBeginScope;
  delete pBeginScope;
  delete pHandleEndScope;
  delete pEndScope;
  delete pHandlePreprocStatement;
  delete pPreprocStatement;
  delete pHandleClassDefinition;
  delete pClassDefinition;
  delete pHandleStructDefinition;
  delete pStructDefinition;
  //delete pPrintFunction;
  delete pHandleCppFunctionDefinition;
  delete pCppFunctionDefinition;
  delete pHandleCSharpFunctionDefinition;
  delete pCSharpFunctionDefinition;
  delete pCppDeclaration;
  delete pHandleCppDeclaration;
  delete pCSharpDeclaration;
  delete pHandleCSharpDeclaration;
  delete pCppExecutable;
  delete pHandleCppExecutable;
  delete pCSharpExecutable;
  delete pHandleCSharpExecutable;
  delete pDefault;
  delete pHandleDefault;
  delete pRepo;
  delete pParser;
  delete pSemi;
  delete pToker;
  if(pIn != nullptr)
    pIn->close();
  delete pIn;
}
//----< attach toker to a file stream or stringstream >------------

bool ConfigParseForCodeAnal::Attach(const std::string& name, bool isFile)
{
  if(pToker == 0)
    return false;
  pIn = new std::ifstream(name);
  if (!pIn->good())
    return false;
  
  // check for Byte Order Mark (BOM)
  char a, b, c;
  a = pIn->get();
  b = pIn->get();
  c = pIn->get();
  if (a != (char)0xEF || b != (char)0xBB || c != (char)0xBF)
    pIn->seekg(0);

  if (!pIn->good())
  {
    // empty file, but openable, so return true, avoids error message
    return true;
  }
  return pToker->attach(pIn);
}
//----< Here's where all the parts get assembled >-----------------

Parser* ConfigParseForCodeAnal::Build()
{
  try
  {
    // add Parser's main parts

    pToker = new Toker;
    pToker->returnComments(false);
    pSemi = new SemiExp(pToker);
    pParser = new Parser(pSemi);
    pRepo = new Repository(pToker);

    // configure to manage scope
    // these must come first - they return true on match
    // so rule checking continues

    pBeginScope = new BeginScope();
    pHandleBeginScope = new HandleBeginScope(pRepo);
    pBeginScope->addAction(pHandleBeginScope);
    pParser->addRule(pBeginScope);

    pEndScope = new EndScope();
    pHandleEndScope = new HandleEndScope(pRepo);
    pEndScope->addAction(pHandleEndScope);
    pParser->addRule(pEndScope);

    // configure to detect and act on function definitions
    // these will stop further rule checking by returning false

    pPreprocStatement = new PreprocStatement;
    pHandlePreprocStatement = new HandlePreprocStatement(pRepo);
    pPreprocStatement->addAction(pHandlePreprocStatement);
    pParser->addRule(pPreprocStatement);

    pNamespaceDefinition = new NamespaceDefinition;
    pHandleNamespaceDefinition = new HandleNamespaceDefinition(pRepo);
    pNamespaceDefinition->addAction(pHandleNamespaceDefinition);
    pParser->addRule(pNamespaceDefinition);

    pClassDefinition = new ClassDefinition;
    pHandleClassDefinition = new HandleClassDefinition(pRepo);
    pClassDefinition->addAction(pHandleClassDefinition);
    pParser->addRule(pClassDefinition);

    pStructDefinition = new StructDefinition;
    pHandleStructDefinition = new HandleStructDefinition(pRepo);
    pStructDefinition->addAction(pHandleStructDefinition);
    pParser->addRule(pStructDefinition);

    pCppFunctionDefinition = new CppFunctionDefinition;
    pHandleCppFunctionDefinition = new HandleCppFunctionDefinition(pRepo);  // no action
    pCppFunctionDefinition->addAction(pHandleCppFunctionDefinition);
    pParser->addRule(pCppFunctionDefinition);

    pCSharpFunctionDefinition = new CSharpFunctionDefinition;
    pHandleCSharpFunctionDefinition = new HandleCSharpFunctionDefinition(pRepo);  // no action
    pCSharpFunctionDefinition->addAction(pHandleCSharpFunctionDefinition);
    pParser->addRule(pCSharpFunctionDefinition);

    // configure to detect and act on declarations and Executables

    pControlDefinition = new ControlDefinition;
    pHandleControlDefinition = new HandleControlDefinition(pRepo);
    pControlDefinition->addAction(pHandleControlDefinition);
    pParser->addRule(pControlDefinition);

    pCppDeclaration = new CppDeclaration;
    pHandleCppDeclaration = new HandleCppDeclaration(pRepo);
    pCppDeclaration->addAction(pHandleCppDeclaration);
    pParser->addRule(pCppDeclaration);

    pCSharpDeclaration = new CSharpDeclaration;
    pHandleCSharpDeclaration = new HandleCSharpDeclaration(pRepo);
    pCSharpDeclaration->addAction(pHandleCSharpDeclaration);
    pParser->addRule(pCSharpDeclaration);

    pCppExecutable = new CppExecutable;
    pHandleCppExecutable = new HandleCppExecutable(pRepo);
    pCppExecutable->addAction(pHandleCppExecutable);
    pParser->addRule(pCppExecutable);

    pCSharpExecutable = new CSharpExecutable;
    pHandleCSharpExecutable = new HandleCSharpExecutable(pRepo);
    pCSharpExecutable->addAction(pHandleCSharpExecutable);
    pParser->addRule(pCSharpExecutable);

    pDefault = new Default;
    pHandleDefault = new HandleDefault(pRepo);
    pDefault->addAction(pHandleDefault);
    pParser->addRule(pDefault);

    return pParser;
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
    return 0;
  }
}

#ifdef TEST_CONFIGUREPARSER

#include <queue>
#include <string>

int main(int argc, char* argv[])
{
  std::cout << "\n  Testing ConfigureParser module\n "
            << std::string(32,'=') << std::endl;

  // collecting tokens from files, named on the command line

  if(argc < 2)
  {
    std::cout 
      << "\n  please enter name of file to process on command line\n\n";
    return 1;
  }

  for(int i=1; i<argc; ++i)
  {
    std::cout << "\n  Processing file " << argv[i];
    std::cout << "\n  " << std::string(16 + strlen(argv[i]),'-');

    ConfigParseForCodeAnal configure;
    Parser* pParser = configure.Build();
    try
    {
      if(pParser)
      {
        if(!configure.Attach(argv[i]))
        {
          std::cout << "\n  could not open file " << argv[i] << std::endl;
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
      std::cout << "\n\n";
    }
    catch(std::exception& ex)
    {
      std::cout << "\n\n    " << ex.what() << "\n\n";
    }
    std::cout << "\n\n";
  }
}

#endif
