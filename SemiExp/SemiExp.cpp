///////////////////////////////////////////////////////////////////////
// SemiExpression.cpp - collect tokens for analysis                  //
// ver 3.9                                                           //
// Language:    C++, Visual Studio 2015                              //
// Platform:    Dell XPS 8900, Windows 10                            //
// Application: Parser component, CSE687 - Object Oriented Design    //
// Author:      Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <exception>
#include <locale>
#include "SemiExp.h"
#include "../Tokenizer/Tokenizer.h"

using namespace Scanner;
using Token = std::string;

//----< initialize semiExpression with existing toker reference >----

SemiExp::SemiExp(Toker* pToker) : _pToker(pToker) {}

//----< copy ctor but don't copy pointer to toker >------------------
/*
*  Allows us to copy tokens for manipulation in grammer methods
*  but does not give a complete copy.
*/
SemiExp::SemiExp(const SemiExp& se)
{
  _tokens = se._tokens;
  _pToker = nullptr;
  hasFor = false;
}
//----< move constructor >-------------------------------------------

SemiExp::SemiExp(SemiExp&& se)
{
  _tokens = se._tokens;
  _pToker = se._pToker;
  hasFor = se.hasFor;
  se._tokens.clear();
  se._pToker = nullptr;
}
//----< assigns tokens but does not assign pointer to toker >--------

SemiExp& SemiExp::operator=(const SemiExp& se)
{
  if (this != &se)
  {
    _tokens = se._tokens;
    _pToker = nullptr;
  }
  return *this;
}
//----< move assignment >--------------------------------------------

SemiExp& SemiExp::operator=(SemiExp&& se)
{
  if (this != &se)
  {
    _tokens = se._tokens;
    _pToker = se._pToker;
    se._tokens.clear();
    se._pToker = nullptr;
  }
  return *this;
}
//----< return iterator pointing to first token >--------------------

SemiExp::iterator SemiExp::begin() { return _tokens.begin(); }

//----< return iterator pointing one past last token >---------------

SemiExp::iterator SemiExp::end() { return _tokens.end(); }

//----< returns position of tok in semiExpression >------------------

size_t SemiExp::find(const std::string& tok, size_t offSet) const
{
  std::string debug = this->show();
  for (size_t i = offSet; i < length(); ++i)
    if (_tokens[i] == tok)
      return i;
  return length();
}
//----< push token onto back end of SemiExp >------------------------

void SemiExp::push_back(const std::string& tok)
{
  _tokens.push_back(tok);
}
//----< removes token passed as argument >---------------------------

bool SemiExp::remove(const std::string& tok)
{
  std::vector<Token>::iterator iter = _tokens.begin();
  while (iter != _tokens.end())
  {
    if (tok == *iter)
    {
      _tokens.erase(iter);
      return true;
    }
    ++iter;
  }
  return false;
}
//----< removes token at nth position of semiExpression -------------

bool SemiExp::remove(size_t n)
{
  if (n < 0 || n >= length())
    return false;
  std::vector<Token>::iterator iter = _tokens.begin() + n;
  _tokens.erase(iter);
  return true;
}
//----< removes newlines from front of semiExpression >--------------

void SemiExp::trimFront()
{
  while (length() > 1)
  {
    if (_tokens[0] == "\n")
      remove(0);
    else
      break;
  }
}
//----< removes newlines from semiExpression >-----------------------

void SemiExp::trim(bool removeComments)
{
  if (removeComments)
  {
    size_t i = 0;
    while (i < _tokens.size())
    {
      if (isComment(_tokens[i]))
        remove(i);
      else
        ++i;
    }
  }
  // remove junk chars that C# puts in first token of file

  static std::locale loc;

  while (_tokens.size() > 1 && !isalnum(_tokens[0][0], loc) && !ispunct(_tokens[0][0], loc))
  {
    remove(0);
    if (_tokens.size() < 2)
      break;
  }
  // remove newlines

  std::vector<std::string>::iterator new_end;
  new_end = std::remove(_tokens.begin(), _tokens.end(), "\n");
  _tokens.erase(new_end, _tokens.end());
}
//----< transform all tokens to lower case >-------------------------

void SemiExp::toLower()
{
  for (auto& token : _tokens)
  {
    for (auto& chr : token)
    {
      chr = tolower(chr);
    }
  }
}
//----< clear contents of SemiExp >----------------------------------

void SemiExp::clear()
{
  _tokens.clear();
}
//----< is this token a comment? >-----------------------------------

bool SemiExp::isComment(const std::string& tok) const
{
  return _pToker->isComment(tok);
}
//----< return count of newlines retrieved by Toker >----------------

size_t SemiExp::currentLineCount() const
{
  if (_pToker == nullptr)
    return 0;
  /* 
   *  Tokenizer has collected first non-state char when exiting eatChars()
   *  so we need to subtract 1 from the Toker's line count.
   */
  return _pToker->currentLineCount() - 1;
}
//----< helps folding for expressions >------------------------------

bool SemiExp::isSemiColonBetweenParens() const
{
  size_t openParenPosition = find("(");
  size_t semiColonPosition = find(";");
  size_t closedParenPosition = find(")");

  if (openParenPosition < semiColonPosition && semiColonPosition < closedParenPosition)
    return true;
  return false;
}
//----< fills semiExpression collection from attached toker >--------

bool SemiExp::get(bool clear)
{
  bool ok = getHelper(clear);

  if (hasFor && isSemiColonBetweenParens())
  {
    getHelper(false);  // add test for loop termination
    getHelper(false);  // add counter increment
  }
  return ok;
}
//----< is token a SemiExpression terminator? >----------------------

bool SemiExp::isTerminator(const std::string& token) const
{
  if (token == "{" || token == "}" || token == ";")
    return true;

  if (token == "\n")
  {
    for (size_t i = 0; i < length(); ++i)
   {
      if (_tokens[i] == "\n")
        continue;
      if (_tokens[i] == "#")
        return true;
      else 
        return false;
    }
  }

  if (length() < 2)
    return false;

  if (token == ":" && length() > 1 && _tokens[length() - 2] == "public")
    return true;

  if (token == ":" && length() > 1 && _tokens[length() - 2] == "protected")
    return true;

  if (token == ":" && length() > 1 && _tokens[length() - 2] == "private")
    return true;

  return false;
}
//----< does all the work of collecting tokens for collection >------

bool SemiExp::getHelper(bool clear)
{
  hasFor = false;
  if (_pToker == nullptr)
    throw(std::logic_error("no Toker reference"));
  if(clear)
    _tokens.clear();
  while (true)
  {
    std::string token = _pToker->getTok();
    if (token == "")
      break;
    _tokens.push_back(token);

    if (token == "for")
      hasFor = true;
    
    if (isTerminator(token))
      return true;
  }
  return false;
}
//----< clone tokens of existing SemiExp >---------------------------
/*
*  Note:
*  - Returns pointer to new SemiExp on heap.  Application is
*    responsible for deleting.
*  - Does not clone pointer to _toker or hasToken members!
*/
ITokCollection* SemiExp::clone() const
{
  SemiExp* pClone = new SemiExp;
  for (size_t i = 0; i < length(); ++i)
    pClone->push_back(_tokens[i]);
  return pClone;
}
//----< clone replaces tokens in caller with tokens of argument >----

void SemiExp::clone(const ITokCollection& se, size_t offSet)
{
  _tokens.clear();
  for (size_t i = offSet; i < se.length(); ++i)
  {
    push_back(se[i]);
  }
}
//----< read only indexing of SemiExpression >-----------------------

Token SemiExp::operator[](size_t n) const
{
  if (n < 0 || n >= _tokens.size())
    throw(std::invalid_argument("index out of range"));
  return _tokens[n];
}
//----< writeable indexing of SemiExpression >-----------------------

Token& SemiExp::operator[](size_t n)
{
  if (n < 0 || n >= _tokens.size())
    throw(std::invalid_argument("index out of range"));
  return _tokens[n];
}
//----< return number of tokens in semiExpression >------------------

size_t SemiExp::length() const
{
  return _tokens.size();
}
//----< display collection tokens on console >-----------------------

std::string SemiExp::show(bool showNewLines) const
{
  std::ostringstream out;
  if(showNewLines)
    out << "\n  ";
  for (auto token : _tokens)
    if(token != "\n" || showNewLines == true)
      out << token << " ";
  if(showNewLines)
    out << "\n";
  return out.str();
}

#ifdef TEST_SEMIEXP

int main()
{
  std::cout << "\n  Testing SemiExp class";
  std::cout << "\n =======================\n";

  Toker toker;
  //std::string fileSpec = "../Tokenizer/Tokenizer.cpp";
  std::string fileSpec = "SemiExp.cpp";

  std::fstream in(fileSpec);
  if (!in.good())
  {
    std::cout << "\n  can't open file " << fileSpec << "\n\n";
    return 1;
  }
  else
  {
    std::cout << "\n  processing file \"" << fileSpec << "\"\n";
  }
  toker.attach(&in);

  SemiExp semi(&toker);
  while(semi.get())
  {
    std::cout << "\n  -- semiExpression -- at line number " << semi.currentLineCount();
    std::cout << semi.show();
  }
  /*
     May have collected tokens, but reached end of stream
     before finding SemiExp terminator.
   */
  if (semi.length() > 0)
  {
    std::cout << "\n  -- semiExpression --";
    std::cout << semi.show(true);
  }
  std::cout << "\n\n";
  return 0;
}
#endif
