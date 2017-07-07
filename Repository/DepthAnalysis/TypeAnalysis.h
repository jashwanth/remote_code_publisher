#pragma once
///////////////////////////////////////////////////////////////
// TypeAnalysis.h -                                          //
// Ver 1.6                                                   //
// Language:     Visual C++ 2015                             //
// Platform:     Mac Machine , Windows 7  Virtual Machine    //
// Application:  for CSE687 - OOD, Pr#2, Spring 2017         //
// Author:       Jashwanth Reddy Gangula, Syracuse University//
//               (315) 949-8857, jgangula@syr.edu            //
///////////////////////////////////////////////////////////////
//*Operations:
//*This packages is used to build the Type table which gives information
//*about the various Interfaces, Structs, Classes and Enums present in the whole package
//*The Type table consists of the type ,name and the file name.
//*Required files
//*ActionsAndRules.h and ActionsAndRules.cpp

#include "../Parser/ActionsAndRules.h"
#include <iostream>
#include <functional>
#include "../Utilities/Utilities.h"
using namespace Utilities;

#pragma warning (disable : 4101)  // disable warning re unused variable x, below

namespace CodeAnalysis
{
	class TypeAnal
	{
	public:
		using SPtr = std::shared_ptr<ASTNode*>;
		//----< Performs the Type Analysis >--------------
		TypeAnal();
		void doTypeAnal();
		//----< Returns the type table which is constructed >--------------
		TypeTable& getTypeTable() { return typeTable_; }
		//----< Displays the Typetable >--------------
		void displayTypeTable() { typeTable_.displayTypeTable(); }
	private:
		//----< Provides a DFS on the graph, a walk >--------------
		void DFS(ASTNode* pNode);
		AbstrSynTree& ASTref_;
		ScopeStack<ASTNode*> scopeStack_;
		Scanner::Toker& toker_;
		TypeTable &typeTable_;
		//TypeTable &Functable;
	};

	inline TypeAnal::TypeAnal() :
		ASTref_(Repository::getInstance()->AST()),
		scopeStack_(Repository::getInstance()->scopeStack()),
		toker_(*(Repository::getInstance()->Toker())),
		typeTable_(Repository::getInstance()->typeTable())
	{
		 
		std::function<void()> test = [] { int x; };  // This is here to test detection of lambdas.
													 // It doesn't do anything useful for dep anal.
		//	  typeTable_.getTypeTable.clear();            // clear the typetable initially

	}


	inline bool doDisplay(ASTNode* pNode)
	{
		static std::string toDisplay[] = {
			"function", "lambda", "class", "struct", "enum", "alias", "typedef"
		};
		for (std::string type : toDisplay)
		{
			if (pNode->type_ == type)
				return true;
		}
		return false;
	}
	inline void TypeAnal::DFS(ASTNode* pNode)
	{
		static std::string path = "";
		if (pNode->path_ != path)
		{
			path = pNode->path_ + "\\" + pNode->package_;
			//std::cout << "\n    -- " << pNode->path_ << "\\" << pNode->package_;
			//path = pNode->path_;
		}
		//if (doDisplay(pNode))
		//{
		//	//std::cout << "\n  " << pNode->name_;
		//	std::cout << ", " << pNode->type_;
		//}

		if ((pNode->type_ == "class") || (pNode->type_ == "struct") || (pNode->type_ == "interface") || (pNode->type_ == "enum") /*|| (pNode->type_ == "function")*/ )
		{
		//	typeTable_.InsertIntoTT(pNode->name_, pNode->package_);

			StoreData s1;
			s1.name = pNode->name_;
			s1.type = pNode->type_;
			s1.filename = pNode->path_;
			typeTable_.InsertIntoTT(pNode->name_,s1);
		}
		for (auto pChild : pNode->children_)
			DFS(pChild);
	}

	inline void TypeAnal::doTypeAnal()
	{
	/*	std::cout << "\n  starting type analysis:\n";
		std::cout << "\n  scanning AST and displaying important things:";
		std::cout << "\n -----------------------------------------------";*/
		StringHelper::Title("Pushing all the classes, structures, enums definitions to type table");
		StringHelper::Title("Start of Depth First Search on ABST");
		ASTNode* pRoot = ASTref_.root();
		DFS(pRoot);
		//std::cout << "\n\n  the rest is for you to complete\n";
	}
}
