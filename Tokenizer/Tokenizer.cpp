/////////////////////////////////////////////////////////////////////
// Tokenizer.cpp - read words from a std::stream                   //
// ver 4.2                                                         //
// Language:    C++, Visual Studio 2015                            //
// Platform:    Dell XPS 8900, Windows 10                          //
// Application: Parser component, CSE687 - Object Oriented Design  //
// Author:      Jim Fawcett, Syracuse University, CST 4-187        //
//              jfawcett@twcny.rr.com                              //
/////////////////////////////////////////////////////////////////////

#include <iostream>
#include <locale>
#include <string>
#include <vector>
#include "Tokenizer.h"
#include "..\Utilities\Utilities.h"

namespace Scanner
{
  ///////////////////////////////////////////////////////////////////
  // Context is a shared data storage facility.
  /*
   * - with Context we don't need static members to share
   *   state pointes etc.  Context holds and shares all that.
   * - Context is an entirely private facility used by ConsumeState
   *   and its derived instances, so there is no need for encapsulation
   *   and we use a struct.  This won't be appropriate if we handed
   *   the context to clients of Toker.
   */
  struct Context
  {
    Context();
    ~Context();
    std::string token;
    std::istream* _pIn;
    std::vector<std::string> _oneCharTokens =
    {
      "\n", "<", ">", "{", "}", "[", "]", "(", ")", ":", ";", " = ", " + ", " - ", "*", ".", ",", "@"
    };
    std::vector<std::string> _twoCharTokens =
    {
      "<<", ">>", "::", "++", "--", "==", "+=", "-=", "*=", "/="
    };
    int prevChar;
    int currChar;
    bool _doReturnComments;
    bool inCSharpString = false;
    size_t _lineCount;
    ConsumeState* _pState;
    ConsumeState* _pEatCppComment;
    ConsumeState* _pEatCComment;
    ConsumeState* _pEatWhitespace;
    ConsumeState* _pEatPunctuator;
    ConsumeState* _pEatAlphanum;
    ConsumeState* _pEatSpecialCharacters;
    ConsumeState* _pEatDQString;
    ConsumeState* _pEatSQString;
    ConsumeState* _pEatRawCppString;
    ConsumeState* _pEatRawCSharpString;
  };

  ///////////////////////////////////////////////////////////////////
  // private ConsumeState worker class
  /*
   *   Nota Bene:
   *   - At exit the eatChars() member should return with
   *     currChar set to the first non-state character.
   *   - Each derived class must insure that this condition
   *     is meet.
   */
  class ConsumeState
  {
    friend class Toker;
  public:
    using Token = std::string;
    ConsumeState();
    ConsumeState(const ConsumeState&) = delete;
    ConsumeState& operator=(const ConsumeState&) = delete;
    virtual ~ConsumeState();
    void attach(std::istream* pIn);
    virtual void eatChars() = 0;
    void consumeChars() {
      _pContext->_pState->eatChars();
      _pContext->_pState = nextState();
    }
    bool canRead() { return _pContext->_pIn->good(); }
    std::string getTok() { return _pContext->token; }
    bool hasTok() { return _pContext->token.size() > 0; }
    ConsumeState* nextState();
    void returnComments(bool doReturnComments = false);
    size_t currentLineCount();
    void setSpecialTokens(const std::string& commaSeparatedString);
    void setContext(Context* pContext);
  protected:
    Context* _pContext;
    bool collectChar();
    bool isOneCharToken(Token tok);
    bool isTwoCharToken(Token tok);
    Token makeString(int ch);
  };
}

using namespace Scanner;
using Token = std::string;

//----< used by Toker to provide Consumer with Context ptr >---------

void ConsumeState::setContext(Context* pContext)
{
  _pContext = pContext;
}
//----< attach or re-attach to input stream >------------------------

void ConsumeState::attach(std::istream* pIn)
{
  _pContext->_pState = _pContext->_pEatWhitespace;
  _pContext->_lineCount = 0;
  _pContext->_pIn = pIn;
}
//----< replace one and two char tokens >----------------------------

void ConsumeState::setSpecialTokens(const std::string& commaSeparatedTokens)
{
  std::vector<Token> result = Utilities::StringHelper::split(commaSeparatedTokens);
  _pContext->_oneCharTokens.clear();
  _pContext->_twoCharTokens.clear();
  for (std::string item : result)
  {
    if (item.size() == 1 || item == "\n")
      _pContext->_oneCharTokens.push_back(item);
    if (item.size() >= 2)
      _pContext->_twoCharTokens.push_back(item);
  }
}
//----< return number of newlines collected from stream >------------

size_t ConsumeState::currentLineCount() { return _pContext->_lineCount; }

//----< collect character from stream >------------------------------

bool ConsumeState::collectChar()
{
  if (_pContext->_pIn->good())
  {
    _pContext->prevChar = _pContext->currChar;
    _pContext->currChar = _pContext->_pIn->get();
    if (_pContext->currChar == '\n')
      ++(_pContext->_lineCount);
    return true;
  }
  return false;
}

//----< logs to console if TEST_LOG is defined >---------------------

void testLog(const std::string& msg);  // forward declaration

//----< turn on\off returning comments as tokens >-------------------

void ConsumeState::returnComments(bool doReturnComments)
{
  _pContext->_doReturnComments = doReturnComments;
}
//----< is tok one of the special one character tokens? >------------

bool ConsumeState::isOneCharToken(Token tok)
{
  for (size_t i = 0; i < _pContext->_oneCharTokens.size(); ++i)
    if (_pContext->_oneCharTokens[i] == tok)
      return true;
  return false;
}
//----< is tok one of the special two character tokens? >------------

bool ConsumeState::isTwoCharToken(Token tok)
{
  for (size_t i = 0; i < _pContext->_twoCharTokens.size(); ++i)
    if (_pContext->_twoCharTokens[i] == tok)
      return true;
  return false;
}
//----< make a string with this one integer >------------------------

ConsumeState::Token ConsumeState::makeString(int ch)
{
  Token temp;
  return temp += ch;
}
//----< decide which state to use next >-----------------------------

ConsumeState* ConsumeState::nextState()
{
  std::locale loc;

  if (!(_pContext->_pIn->good()))
  {
    return nullptr;
  }
  int chNext = _pContext->_pIn->peek();
  if (chNext == EOF)
  {
    _pContext->_pIn->clear();
    // if peek() reads end of file character, EOF, then eofbit is set and
    // _pIn->good() will return false.  clear() restores state to good
  }
  //---------------------------------------------------------
  // The following tests must come first

  if (_pContext->currChar == '\"' && _pContext->prevChar != '\\')
  {
    testLog("state: eatDQString");
    return _pContext->_pEatDQString;
  }
  if (_pContext->currChar == '\'' && _pContext->prevChar != '\\')
  {
    testLog("state: eatSQString");
    return _pContext->_pEatSQString;
  }
  if (_pContext->currChar == '/' && chNext == '/')
  {
    testLog("state: eatCppComment");
    return _pContext->_pEatCppComment;
  }
  if (_pContext->currChar == '/' && chNext == '*')
  {
    testLog("state: eatCComment");
    return _pContext->_pEatCComment;
  }
  if (_pContext->currChar == 'R' && chNext == '\"')
  {
    testLog("state: eatRawCppString");
    return _pContext->_pEatRawCppString;
  }
  if (_pContext->currChar == '@' && chNext == '\"')
  {
    testLog("state: eatRawCSharpString");
    return _pContext->_pEatRawCSharpString;
  }
  //---------------------------------------------------------
  // The following tests must come after those above

  if (isOneCharToken(makeString(_pContext->currChar)))
  {
    testLog("state: eatSpecialCharacters");
    return _pContext->_pEatSpecialCharacters;
  }
  if (std::isspace(_pContext->currChar, loc) && _pContext->currChar != '\n')
  {
    testLog("state: eatWhitespace");
    return _pContext->_pEatWhitespace;
  }
  if (std::isalnum(_pContext->currChar, loc) || _pContext->currChar == '_')
  {
    testLog("state: eatAlphanum");
    return _pContext->_pEatAlphanum;
  }
  if (ispunct(_pContext->currChar))
  {
    testLog("state: eatPunctuator");
    return _pContext->_pEatPunctuator;
  }
  if (!_pContext->_pIn->good())
  {
    testLog("state: eatWhitespace");
    return _pContext->_pEatWhitespace;
  }
  //---------------------------------------------------------
  // If _pContext->currChar is not a state transition char
  // then we go to eatPuctuator state

  return _pContext->_pEatPunctuator;
}
//----< class that consumes whitespace >-----------------------------

class EatWhitespace : public ConsumeState
{
public:
  EatWhitespace(Context* pContext)
  {
    _pContext = pContext;
  }
  virtual void eatChars()
  {
    std::locale loc;
    _pContext->token.clear();
    do {
      if (!collectChar())
        return;
    } while (std::isspace(_pContext->currChar, loc) && _pContext->currChar != '\n');
  }
};
//----< class that consumes C++ comment >----------------------------

class EatCppComment : public ConsumeState
{
public:
  EatCppComment(Context* pContext)
  {
    _pContext = pContext;
  }
  virtual void eatChars()
  {
    _pContext->token.clear();
    do {
      if (_pContext->_doReturnComments)
        _pContext->token += _pContext->currChar;
      if (!collectChar())
        return;
    } while (_pContext->currChar != '\n');
  }
};
//----< class that consumes C comment >------------------------------

class EatCComment : public ConsumeState
{
public:
  EatCComment(Context* pContext)
  {
    _pContext = pContext;
  }
  virtual void eatChars()
  {
    _pContext->token.clear();
    do {
      if (_pContext->_doReturnComments)
        _pContext->token += _pContext->currChar;
      if (!collectChar())
        return;
    } while (_pContext->currChar != '*' || _pContext->_pIn->peek() != '/');
    if (_pContext->_doReturnComments)
      _pContext->token += _pContext->currChar;
    if (!collectChar())      // get terminating '/'
      return;
    if (_pContext->_doReturnComments)
      _pContext->token += _pContext->currChar;
    collectChar();           // get first non-comment char
  }
};
//----< class that eats punctuators >--------------------------------

class EatPunctuator : public ConsumeState
{
public:
  EatPunctuator(Context* pContext)
  {
    _pContext = pContext;
  }
  virtual void eatChars()
  {
    _pContext->token.clear();
    int chNext = _pContext->_pIn->peek();
    do {
      if (_pContext->currChar == '\"' && _pContext->prevChar != '\\')    // start of double quoted string
      {
        return;
      }
      if (_pContext->currChar == '@' && chNext == '\"')    // start of raw C# double quoted string
      {
        return;
      }
      if (_pContext->currChar == '\'' && _pContext->prevChar != '\\')    // start of single quoted string
      {
        return;
      }
      Token temp = makeString(_pContext->currChar);
      if (isOneCharToken(temp))
      {
        return;
      }
      _pContext->token += _pContext->currChar;
      if (!_pContext->_pIn->good())  // end of stream
      {
        return;
      }
      if (!collectChar())
        return;
    } while (ispunct(_pContext->currChar));
  }
};
//----< class that eats alphanumeric characters >--------------------
/*
 * treats underscore '_' as alphanumeric character
 */
class EatAlphanum : public ConsumeState
{
public:
  EatAlphanum(Context* pContext)
  {
    _pContext = pContext;
  }
  virtual void eatChars()
  {
    _pContext->token.clear();
    int chNext = _pContext->_pIn->peek();
    do {
      _pContext->token += _pContext->currChar;
      if (!collectChar())
        return;
    } while (isalnum(_pContext->currChar) || _pContext->currChar == '_');
  }
};
//----< class that eats one and two character tokens >---------------

class EatSpecialCharacters : public ConsumeState
{
public:
  EatSpecialCharacters(Context* pContext)
  {
    _pContext = pContext;
  }
  virtual void eatChars()
  {
    _pContext->token.clear();
    _pContext->token += _pContext->currChar;
    Token temp = makeString(_pContext->currChar) += _pContext->_pIn->peek();
    if (isTwoCharToken(temp))
    {
      collectChar();
      _pContext->token += _pContext->currChar;
    }
    collectChar();
  }
};
//----< class that eats double quoted string >-----------------------

bool isEscapted(std::string tok)
{
  size_t backSlashCount = 0;
  for (size_t i = tok.size() - 1; i >= 0; --i)
  {
    if (tok[i] == '\\')
      ++backSlashCount;
    else break;
  }
  return (backSlashCount % 2 == 1);
}
//----< class that eats double quoted string >-----------------------

class EatDQString : public ConsumeState
{
public:
  EatDQString(Context* pContext)
  {
    _pContext = pContext;
  }
  virtual void eatChars()
  {
    _pContext->token.clear();
    do
    {
      _pContext->token += _pContext->currChar;
      if (!collectChar())
        return;
    } while (_pContext->currChar != '\"' || isEscapted(_pContext->token));
    _pContext->token += _pContext->currChar;
    _pContext->inCSharpString = false;
    collectChar();
  }
};
//----< class that eats double quoted string >-----------------------

class EatRawCppString : public ConsumeState
{
public:
  EatRawCppString(Context* pContext)
  {
    _pContext = pContext;
  }
  virtual void eatChars()
  {
    _pContext->token.clear();
    do
    {
      _pContext->token += _pContext->currChar;
      if (!collectChar())
        return;
    } while (_pContext->currChar != '\"' || _pContext->prevChar == 'R');
    _pContext->token += _pContext->currChar;
    collectChar();
  }
};
//----< class that eats double quoted string >-----------------------

class EatRawCSharpString : public ConsumeState
{
public:
  EatRawCSharpString(Context* pContext)
  {
    _pContext = pContext;
  }
  virtual void eatChars()
  {
    _pContext->token.clear();
    do
    {
      _pContext->token += _pContext->currChar;
      if (!collectChar())
        return;
    } while (_pContext->currChar != '\"' || _pContext->prevChar == '@');
    _pContext->token += _pContext->currChar;
    collectChar();
  }
};
//----< class that eats single quoted string >-----------------------

class EatSQString : public ConsumeState
{
public:
  EatSQString(Context* pContext)
  {
    _pContext = pContext;
  }
  virtual void eatChars()
  {
    _pContext->token.clear();
    do
    {
      _pContext->token += _pContext->currChar;
      if (!collectChar())
        return;
    } while (_pContext->currChar != '\'' || isEscapted(_pContext->token));
    _pContext->token += _pContext->currChar;
    collectChar();
  }
};
//----< construct shared data storage >------------------------------

Context::Context()
{
  _pEatAlphanum = new EatAlphanum(this);
  _pEatCComment = new EatCComment(this);
  _pEatCppComment = new EatCppComment(this);
  _pEatPunctuator = new EatPunctuator(this);
  _pEatWhitespace = new EatWhitespace(this);
  _pEatSpecialCharacters = new EatSpecialCharacters(this);
  _pEatDQString = new EatDQString(this);
  _pEatRawCppString = new EatRawCppString(this);
  _pEatRawCSharpString = new EatRawCSharpString(this);
  _pEatSQString = new EatSQString(this);
  _pState = _pEatWhitespace;
  _lineCount = 0;
  _doReturnComments = false;
}
//----< return shared resources >------------------------------------

Context::~Context()
{
  delete _pEatAlphanum;
  delete _pEatCComment;
  delete _pEatCppComment;
  delete _pEatPunctuator;
  delete _pEatWhitespace;
  delete _pEatSpecialCharacters;
  delete _pEatDQString;
  delete _pEatRawCppString;
  delete _pEatRawCSharpString;
  delete _pEatSQString;
}
//----< no longer has anything to do - will be removed >-------------

ConsumeState::ConsumeState() {}

//----< no longer has anything to do - will be removed >-------------

ConsumeState::~ConsumeState() {}

//----< construct toker starting in state EatWhitespace >------------

Toker::Toker()
{
  _pContext = new Context();
  pConsumer = _pContext->_pEatWhitespace;
  pConsumer->setContext(_pContext);
}

//----< destructor deletes the current state >-----------------------
/*
 * deleting any state deletes them all since derived destructor
 * implicitly invokes the base destructor
 */
Toker::~Toker() 
{
  delete _pContext;
  _pContext = nullptr;
}

//----< attach tokenizer to stream >---------------------------------
/*
 * stream can be either a std::fstream or std::stringstream
 */
bool Toker::attach(std::istream* pIn)
{
  if (pIn != nullptr && pIn->good())
  {
    pConsumer->attach(pIn);
    //_pContext->_pIn = pIn;
    return true;
  }
  return false;
}
//----< collect token generated by ConsumeState >--------------------

std::string Toker::getTok()
{
  while(true) 
  {
    if (!pConsumer->canRead())
      return "";
    pConsumer->consumeChars();
    if (pConsumer->hasTok())
      break;
  }
  return pConsumer->getTok();
}
//----< has toker reached the end of its stream? >-------------------

bool Toker::canRead() { return pConsumer->canRead(); }

//----< start/stop returning comments as tokens >--------------------

void Toker::returnComments(bool doReturnComments)
{
  pConsumer->returnComments(doReturnComments);
}
//----< is this token a comment? >-----------------------------------

bool Toker::isComment(const std::string& tok)
{
  if (tok.find("//") < tok.size() || tok.find("/*") < tok.size())
    return true;
  return false;
}
//----< return number of newlines pulled from stream >---------------

size_t Toker::currentLineCount()
{
  return pConsumer->currentLineCount();
}
//----< set one and two char tokens >--------------------------------

void Toker::setSpecialTokens(const std::string& commaSeparatedTokens)
{
  pConsumer->setSpecialTokens(commaSeparatedTokens);
}
//----< debugging output to console if TEST_LOG is #defined >--------

void testLog(const std::string& msg)
{
#ifdef TEST_LOG
  std::cout << "\n  " << msg;
#endif
}

//----< test stub >--------------------------------------------------

using Helper = Utilities::StringHelper;
using namespace Utilities;

#ifdef TEST_TOKENIZER

#include <fstream>
#include <exception>


int main()
{
  Helper::Title("Testing Tokenizer");

  //std::string fileSpec = "../Tokenizer/Tokenizer.cpp";
  //std::string fileSpec = "../Tokenizer/Tokenizer.h";
  std::string fileSpec = "../Tokenizer/Test.txt";
  //std::string fileSpec = "../SemiExp/SemiExp.h";
  try
  {
    std::ifstream in(fileSpec);
    if (!in.good())
    {
      std::cout << "\n  can't open " << fileSpec << "\n\n";
      return 1;
    }

    {
      Toker toker;
      toker.returnComments();
      toker.attach(&in);
      std::cout << "\n  current line count = " << toker.currentLineCount();
      do
      {
        std::string tok = toker.getTok();
        if (tok == "\n")
          tok = "newline";
        std::cout << "\n -- " << tok;
      } while (in.good());
      std::cout << "\n  current line count = " << toker.currentLineCount();
    }
 
    putline();
    Helper::title("Testing change of special characters");
    std::string newSpecialChars = "., :, +, +=, \n { }";

    Toker toker;
    toker.returnComments();
    toker.setSpecialTokens(newSpecialChars);
    in.clear();

    in.seekg(std::ios::beg);
    toker.attach(&in);
    std::cout << "\n  new special tokens: " << newSpecialChars;
    do
    {
      std::string tok = toker.getTok();
      if (tok == "\n")
        tok = "newline";
      std::cout << "\n -- " << tok;
    } while (in.good());
    std::cout << "\n  current line count = " << toker.currentLineCount() << "\n";

    //Helper::title("Testing re-attach:");
    //std::string path = "../Tokenizer/Tokenizer.h";
    //std::ifstream inAgain(path);
    //if (inAgain.good())
    //{
    //  toker.attach(&inAgain);
    //  do
    //  {
    //    std::string tok = toker.getTok();
    //    if (tok == "\n")
    //      tok = "newline";
    //    std::cout << "\n -- " << tok;
    //  } while (inAgain.good());
    //  std::cout << "\n  current line count = " << toker.currentLineCount();
    //}
    //else
    //{
    //  std::cout << "\n  can't attach to " << path << "\n";
    //}
  }
  catch (std::logic_error& ex)
  {
    std::cout << "\n  " << ex.what();
  }
  std::cout << "\n\n";
  return 0;
}
#endif
