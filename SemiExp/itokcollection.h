#ifndef ITOKCOLLECTION_H
#define ITOKCOLLECTION_H
/////////////////////////////////////////////////////////////////////
//  ITokCollection.h - package for the ITokCollection interface    //
//  ver 1.4                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell XPS 8900, Windows 10                       //
//  Application:   Code analysis tools                             //
//  Author:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////
/*
  Module Purpose:
  ===============
  ITokCollection is an interface that supports substitution of different
  types of scanners for parsing.  In this solution, we illustrate that
  by binding two different types of collections, SemiExp and XmlParts,
  to this interface.  This is illustrated in the test stubs for the
  SemiExpression and XmlElementParts packages.

  Maintenance History:
  ====================
  ver 1.4 : 28 Aug 2016
  - added default parameter to trim for comment removal
  ver 1.3 : 25 Aug 2016
  - added begin(), end(), clone(const SemiExp&), and iterator
  ver 1.2 : 15 Feb 2016
  - removed merge, added non-const operator[] overload
  ver 1.1 : 02 Jun 11
  - added merge, remove overload, and default param in get
  ver 1.0 : 17 Jan 09
  - first release
*/
#include <string> 
#include <vector>

namespace Scanner
{
  struct ITokCollection
  {
    using iterator = std::vector<std::string>::iterator;
    virtual iterator begin() = 0;
    virtual iterator end() = 0;
    virtual bool get(bool clear = true) = 0;
    virtual ITokCollection* clone() const = 0;
    virtual void clone(const ITokCollection&, size_t offSet = 0) = 0;
    virtual size_t length() const = 0;
    virtual std::string& operator[](size_t n) = 0;
    virtual std::string operator[](size_t n) const = 0;
    virtual size_t find(const std::string& tok, size_t offSet = 0) const = 0;
    virtual void push_back(const std::string& tok) = 0;
    virtual bool remove(const std::string& tok) = 0;
    virtual bool remove(size_t i) = 0;
    virtual void toLower() = 0;
    virtual void trimFront() = 0;
    virtual void trim(bool removeComments = true) = 0;
    virtual void clear() = 0;
    virtual std::string show(bool showNewLines = false) const = 0;
    virtual bool isComment(const std::string& tok) const = 0;
    virtual ~ITokCollection() {};
  };
}
#endif
