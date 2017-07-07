#pragma once
/////////////////////////////////////////////////////////////////////////
// GrammarHelpers.h - Functions providing base grammatical analyses    //
// ver 1.3                                                             //
// Language:    C++, Visual Studio 2015                                //
// Application: Support for Parsing, CSE687 - Object Oriented Design   //
// Author:      Jim Fawcett, Syracuse University, CST 4-187            //
//              jfawcett@twcny.rr.com                                  //
/////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides a single class, GrammarHelper, with all static methods.
* Each method provides a useful piece of analysis on a semiexpression, designed
* to support source code parsing activities.
*
* The purpose of this package is to factor out of the ActionsAndRules package
* some complex processing so it gets implemented in only one place.  It also
* factors a lot of grammar processing into small testable chunks.
*
* Build Process:
* --------------
* Required Files:
*   GrammarHelpers.h, GrammarHelpers.cpp,
*   SemiExpression.h, SemiExpression.cpp, Tokenizer.h, Tokenizer.cpp,
*   Logger.h, Logger.cpp, Utilities.h, Utilities.cpp
*
* Build Command: devenv Analyzer.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.3 : 26 Feb 2017
* - modified isFunctionDeclaration to support a bug fix in ActionsAndRules
*   associated with failure to detect some public data declarations
* Ver 1.2 : 28 Aug 2016
* - fixed errors found in last round of testing
* Ver 1.1 : 25 Aug 2016
* - added many new functions
* - changed some of the function signatures
* Ver 1.0 : 10 Aug 2016
* - first release
*
* Planned Additions and Changes:
* ------------------------------
* - These functions may result in scanning a SemiExp instance several times.
*   It should be straight forward to reduce some of that.
*/


#include <string>
#include "../SemiExp/itokcollection.h"

namespace CodeAnalysis
{
  class GrammarHelper
  {
  public:
    static bool isControlKeyWord(const std::string& tok);
    static bool hasControlKeyWord(const Scanner::ITokCollection& se);
    static bool isFunction(const Scanner::ITokCollection& se);
    static bool hasArgs(const Scanner::ITokCollection& se);
    static bool isFunctionDefinition(const Scanner::ITokCollection& se);
    static bool isFunctionDeclaration(const Scanner::ITokCollection& se, const std::string& parentType);
    static bool isFunctionInvocation(const Scanner::ITokCollection& se, const std::string& parentType);
    static bool hasFunctionInvocation(const Scanner::ITokCollection& se, const std::string& parentType);
    static bool isDataDeclaration(const Scanner::ITokCollection& se);
    static bool isExecutable(const Scanner::ITokCollection& se, const std::string& parentType);
    static size_t findLast(const Scanner::ITokCollection& se, const std::string& token);
    static bool isQualifierKeyWord(const std::string& tok);
    static void removeQualifiers(Scanner::ITokCollection& tc);
    static void removeCallingArgQualifiers(Scanner::ITokCollection& tc);
    static bool isFirstArgDeclaration(const Scanner::ITokCollection& tc, const std::string& parentType);
    static bool hasReturnType(const Scanner::ITokCollection& tc);
    static void removeCppInitializers(Scanner::ITokCollection& tc);
    static void removeCSharpInitializers(Scanner::ITokCollection& tc);
    static void removeComments(Scanner::ITokCollection& tc);
    static void condenseTemplateTypes(Scanner::ITokCollection& tc);
    static void removeFunctionArgs(Scanner::ITokCollection& tc);
    static void showParse(const std::string& msg, const Scanner::ITokCollection& se);
    static void showParseDemo(const std::string& msg, const Scanner::ITokCollection& se);
  };
}
