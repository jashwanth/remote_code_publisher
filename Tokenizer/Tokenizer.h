

#ifndef TOKENIZER_H
#define TOKENIZER_H
///////////////////////////////////////////////////////////////////////
// Tokenizer.h - read words from a std::stream                       //
// ver 4.2                                                           //
// Language:    C++, Visual Studio 2015                              //
// Platform:    Dell XPS 8900, Windows 10                            //
// Application: Parser component, CSE687 - Object Oriented Design    //
// Author:      Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package provides a public Toker class and private ConsumeState class.
 * Toker reads words from a std::stream, throws away whitespace and optionally
 * throws away comments.
 *
 * Toker returns words from the stream in the order encountered.  Quoted
 * strings and certain punctuators and newlines are returned as single tokens.
 *
 * This is a new version, based on the State Design Pattern.  Older versions
 * exist, based on an informal state machine design.
 *
 * Build Process:
 * --------------
 * Required Files: Tokenizer.h, Tokenizer.cpp
 *
 * Build Command: devenv Tokenizer.sln /rebuild debug
 *
 * Maintenance History:
 * --------------------
 * ver 4.2 : 26 Feb 2017
 * - converted all uses of std::isspace from <cctype> to std::isspace from <locale>
 * ver 4.1 : 19 Aug 2016
 * - added "@" to specialCharTokens
 * - added states EatRawCppString and EatRawCSharpString
 * - rearranged placement of nextState processing
 * - added comments
 * - modified attach() to reinitialze the context allowing the attachment of
 *   more than one file sequentially without creating a new parser for each.
 * ver 4.0 : 16 Feb 2016
 * - modified EatDQString::eatChars() and EatSQString::eatChars() to properly
 *   handle repetition of escape char, '\', using isEscaped(tok) function.
 * ver 3.9 : 15 Feb 2016
 * - added isComment(tok)
 * ver 3.8 : 06 Feb 2016
 * - Converted ver 3.7 design to use references to a context instead of static members.
 *   This is more complex, but now multiple Tokers won't interfere with each other.
 * ver 3.7 : 06 Feb 2016
 * - reverted to the original static design, as implemented in helper code
 * - the next version will be a design using references to a context
 *   instead of many static members.
 * ver 3.6 : 05 Feb 2016
 * - added ConsumeState::currentLineCount() and Toker::currentLineCount
 * - added ConsumeState::collectChar which checks stream state and, 
 *   if possible extracts a new currChar after updating prevChar
 *   and checking for newlines.  All of the eatChars() member functions
 *   were revised to use this function.
 * ver 3.5 : 03 Feb 2016
 * - added collection of single and two character tokens
 * - added collection of single and double quoted strings
 * ver 3.4 : 03 Feb 2016
 * - fixed bug that prevented reading of last character in source by
 *   clearing stream errors at beginning of ConsumeState::nextState()
 * ver 3.3 : 02 Feb 2016
 * - declared ConsumeState copy constructor and assignment operator = delete
 * ver 3.2 : 28 Jan 2016
 * - fixed bug in ConsumeState::nextState() by returning a valid state
 *   pointer if all tests fail due to reaching end of file instead of
 *   throwing logic_error exception.
 * ver 3.1 : 27 Jan 2016
 * - fixed bug in EatCComment::eatChars()
 * - removed redundant statements assigning _pState in derived eatChars() 
 *   functions
 * - removed calls to nextState() in each derived eatChars() and fixed
 *   call to nextState() in ConsumeState::consumeChars()
 * ver 3.0 : 11 Jun 2014
 * - first release of new design
 *
 * Planned Additions and Changes:
 * ------------------------------
 * - merge the oneCharacter and twoCharacter special Tokens into a
 *   single collection with a single setter method.
 * - consider replacing std::vector containers for special characteres
 *   with an unordered_map (hash table).
 */
#include <iosfwd>
#include <string>

namespace Scanner
{
  class ConsumeState;    // private worker class
  struct Context;        // private shared data storage

  class Toker
  {
  public:
    Toker();
    Toker(const Toker&) = delete;
    ~Toker();
    Toker& operator=(const Toker&) = delete;
    bool attach(std::istream* pIn);
    std::string getTok();
    bool canRead();
    void returnComments(bool doReturnComments = true);
    bool isComment(const std::string& tok);
    size_t currentLineCount();
    void setSpecialTokens(const std::string& commaSeparatedString);
  private:
    ConsumeState* pConsumer;
    Context* _pContext;
  };
}
#endif
