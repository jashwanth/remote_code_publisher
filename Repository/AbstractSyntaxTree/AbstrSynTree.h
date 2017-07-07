#pragma once
/////////////////////////////////////////////////////////////////////
//  AbstrSynTree.h - Represents an Abstract Syntax Tree            //
//  ver 1.4                                                        //
//  Language:      Visual C++ 2015                                 //
//  Platform:      Dell XPS 8900, Windows 10                       //
//  Application:   Used to support parsing source code             //
//  Author:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////
/*
  Package Operations:
  ==================
  This package defines an AbstrSynTree class and an ASTNode struct.  
  Those provides support for building Abstract Syntax Trees during 
  static source code analysis.

  Public Interface:
  =================
  AbstrSynTree ast(scopeStack);       // create instance, passing in ScopeStack
  ASTNode* pNode = ast.root();        // get or set root of AST
  ast.add(pNode);                     // add ASTNode to tree, linked to current scope
  ASTNode* pNode = ast.find(myType);  // retrieve ptr to ASTNode representing myType
  ast.pop();                          // close current scope by poping top of scopeStack

  Build Process:
  ==============
  Required files
  - AbstrSynTree.h, AbstrSynTree.cpp, Utilities.h, Utilities.cpp

  Build commands (either one)
  - devenv CodeAnalysis.sln
  - cl /EHsc /DTEST_ABSTRSYNTREE AbstrSynTree.cpp Utilities.cpp /link setargv.obj

  Maintenance History:
  ====================
  ver 1.4 : 26 Feb 2017
  - added parentType_ member to support better grammar analysis
  ver 1.3 : 29 Oct 2016
  - added throw in ScopeStack if pop or peek empty stack
  ver 1.2 : 28 Oct 2016
  - added test for interface in show() and Add(ASTNode*)
  ver 1.1 : 22 Aug 2016
  - added access type and declaration type enumerations
  - added processing to store access level in AST
  - added more prologue comments
  ver 1.0 : 05 Aug 2016
  - first release

*/

#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <functional>
#include "../SemiExp/itokcollection.h"
#include "../ScopeStack/ScopeStack.h"

namespace CodeAnalysis
{
  enum Access 
  { 
    publ, prot, priv 
  };

  enum DeclType 
  { 
    dataDecl, functionDecl, lambdaDecl, usingDecl 
  };

  struct DeclarationNode
  {
    Scanner::ITokCollection* pTc = nullptr;
    Access access_;
    DeclType declType_;
    std::string package_;
    size_t line_;
  };

  struct ASTNode
  {
    using Type = std::string;
    using Name = std::string;
    using Package = std::string;
    using Path = std::string;

    ASTNode();
    ASTNode(const Type& type, const Name& name);
    ~ASTNode();
    Type type_;
    Type parentType_;
    Name name_;
    Package package_;
    Path path_;
    size_t startLineCount_;
    size_t endLineCount_;
    size_t complexity_;
    std::vector<ASTNode*> children_;
    std::vector<DeclarationNode> decl_;
    std::vector<Scanner::ITokCollection*> statements_;
    std::string show(bool details = false);
  };

  class AbstrSynTree
  {
  public:
    using ClassName = std::string;
    using TypeMap = std::unordered_map<ClassName, ASTNode*>;

    AbstrSynTree(ScopeStack<ASTNode*>& stack);
    ~AbstrSynTree();
    ASTNode*& root();
    void add(ASTNode* pNode);
    ASTNode* find(const ClassName& type);
    ASTNode* pop();
    TypeMap& typeMap();
  private:
    TypeMap typeMap_;
    ScopeStack<ASTNode*>& stack_;
    ASTNode* pGlobalNamespace_;
  };
  //----< traverse AST and execute callobj on every node >-------------

  template <typename CallObj>
  void ASTWalk(ASTNode* pItem, CallObj co)
  {
    static size_t indentLevel = 0;
    co(pItem, indentLevel);
    auto iter = pItem->children_.begin();
    ++indentLevel;
    while (iter != pItem->children_.end())
    {
      ASTWalk(*iter, co);
      ++iter;
    }
    --indentLevel;
  }
  //----< traverse AST and execute callobj on every node >-------------

  template <typename CallObj>
  void ASTWalkNoIndent(ASTNode* pItem, CallObj co)
  {
    co(pItem);
    auto iter = pItem->children_.begin();
    while (iter != pItem->children_.end())
    {
      ASTWalkNoIndent(*iter, co);
      ++iter;
    }
  }
  //----< compute complexities for each ASTNode >--------------------

  inline void complexityWalk(ASTNode* pItem, size_t& count)
  {
    size_t inCount = ++count;
    auto iter = pItem->children_.begin();
    while (iter != pItem->children_.end())
    {
      complexityWalk(*iter, count);
      ++iter;
    }
    pItem->complexity_ = count - inCount + 1;
  }
  //----< compute complexities for each ASTNode >--------------------

  inline void complexityEval(ASTNode* pNode)
  {
    size_t initialCount = 0;
    complexityWalk(pNode, initialCount);
  }
}

struct foobar {

};
