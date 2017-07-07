#ifndef SEMIEXPRESSION_H
#define SEMIEXPRESSION_H
///////////////////////////////////////////////////////////////////////
// SemiExpression.h - collect tokens for analysis                    //
// ver 3.9                                                           //
// Language:    C++, Visual Studio 2015                              //
// Platform:    Dell XPS 8900, Windows 10                            //
// Application: Parser component, CSE687 - Object Oriented Design    //
// Author:      Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides a public SemiExp class that collects and makes
* available sequences of tokens.  SemiExp uses the services of a Toker
* class to acquire tokens.  Each call to SemiExp::get() returns a 
* sequence of tokens that ends in {, }, ;, and '\n' if the line begins
* with #.  There are three additiona termination conditions: a sequence
* of tokens that ends with : and the immediately preceding token is
* public, protected, or private.
*
* Each semiexpression returns just the right tokens to analyze one
* C++ grammatical construct, e.g., class definition, function definition,
* declaration, etc.
* 
* Build Process:
* --------------
* Required Files: 
*   SemiExpression.h, SemiExpression.cpp, Tokenizer.h, Tokenizer.cpp, 
*   Utilities.h, Utilities.cpp
* 
* Build Command: devenv Project1.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 3.9 : 26 Feb 2017
* - converted all uses of std::isspace from <cctype> to std::isspace from <locale>
* ver 3.8 : 27 Aug 2016
* - modified SemiExp::trim() to remove leading characters that are not
*   alphanumerics or punctuators 
*   (C# adds some of these on first line of source code file)
* ver 3.7 : 25 Aug 2016
* - added begin(), end(), clone(const SemiExp&), and iterator
* - added optional parameter in trim() to remove comments
* ver 3.6 : 31 Jul 2016
* - added clone function
* - qualified length() as const
* ver 3.5 : 15 Feb 2016
* - modifications to implement ITokCollection:
*   - added member functions: const indexer, push_back, remove(tok), 
*     toLower, isComment, clear
*   - changed trim to trimFront
* ver 3.4 : 06 Feb 2016
* - added some additional comments
* ver 3.3 : 03 Feb 2016
* - completed addition of terminators
* - added trim() and remove()
* ver 3.2 : 02 Feb 2016
* - declared SemiExp copy constructor and assignment operator = delete
* - added default argument for Toker pointer to nullptr so SemiExp
*   can be used like a container of tokens.
* - if pToker is nullptr then get() will throw logic_error exception
* ver 3.1 : 30 Jan 2016
* - changed namespace to Scanner
* - fixed bug in termination due to continually trying to read
*   past end of stream if last tokens didn't have a semiExp termination
*   character
* ver 3.0 : 29 Jan 2016
* - built in help session, Friday afternoon
*
* Planned Additions and Changes:
* ------------------------------
* - change the ITokCollection interface to match what is needed for the
*   parser, and make SemiExp implement that interface
* - add public :, protected :, private : as terminators
* - move creation of tokenizer into semiExp constructor so
*   client doesn't have to write that code
*/

#include <vector>
#include <string>
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/itokcollection.h"

namespace Scanner
{
  class SemiExp : public ITokCollection
  {
  public:
    using iterator = std::vector<std::string>::iterator;
    SemiExp(Toker* pToker = nullptr);
    SemiExp(const SemiExp& se);
    SemiExp(SemiExp&& se);
    SemiExp& operator=(const SemiExp&);
    SemiExp& operator=(SemiExp&& se);
    iterator begin() override;
    iterator end() override;
    bool get(bool clear = true) override;
    ITokCollection* clone() const override;
    void clone(const ITokCollection& se, size_t offSet = 0) override;
    std::string& operator[](size_t n);
    std::string operator[](size_t n) const;
    size_t length() const;
    size_t find(const std::string& tok, size_t offSet = 0) const;
    void trimFront();
    void trim(bool removeComments = true);
    void toLower();
    bool remove(const std::string& tok);
    bool remove(size_t n);
    void push_back(const std::string& tok);
    void clear();
    bool isComment(const std::string& tok) const;
    std::string show(bool showNewLines = false) const;
    size_t currentLineCount() const;
  private:
    bool isTerminator(const std::string& tok) const;
    bool getHelper(bool clear = false);
    bool isSemiColonBetweenParens() const;
    bool hasFor = false;
    std::vector<std::string> _tokens;
    Toker* _pToker;
  };
}
#endif
