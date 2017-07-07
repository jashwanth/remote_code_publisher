#ifndef DEPENDANAL_H
#define DEPENDANAL_H
/////////////////////////////////////////////////////////////////////
// DependAnal.h - Store the type information needed for dependency //
//                between the files                                //
// ver 1.0                                                         //
//-----------------------------------------------------------------//
// Jashwanth Reddy Gangula (c) copyright 2015                      //
// All rights granted provided this copyright notice is retained   //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Application: TYpe Based Dependency Analysis                     //
//              CSE687 - Object Oriented Design                    //
// Author:      Jashwanth Reddy Gangula, Syracuse University       //
//              jgangula@syr.edu                                   //
/////////////////////////////////////////////////////////////////////

//* Operations:
//* This package is used to build Dependency graph from the Type table information.
//* Uisng The Typetable constructed as part of Type Table package, this package
//* parses each file, extracts each token and fetches all the dependencies and populates the 
//* graph.

//* Required Files :
//*-------------- -
//*-TypeAnalysis.h, TypeAnalysis.cpp,
//*Parser.h, Parser.cpp
//*Tokenizer.h,Tokenizer.cpp
//*ScopeStack.h,ScopeStack.cpp
//*SemiExp.h,SemiExp.cpp
//*itokcollection.h


#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <iostream>
#include "../DepthAnalysis/TypeAnalysis.h"
#include "../Parser/Parser.h"
#include "../Analyzer/Executive.h"
#include "../SemiExp/itokcollection.h"
#include "../ScopeStack/ScopeStack.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"
#include "../Graph/Graph.h"

using namespace Scanner;
namespace CodeAnalysis
{
	class DependAnal
	{
		using Path = std::string;
		using Pattern = std::string;
		using Patterns = std::vector<Pattern>;
		using File = std::string;
		using Files = std::vector<File>;
		using Ext = std::string;
		using Options = std::vector<char>;
		using FileMap = std::unordered_map<Pattern, Files>;
		using DependMap = std::unordered_map<std::string, std::set<std::string>>;
	public:
		DependAnal::DependAnal() : typeTable_(Repository::getInstance()->typeTable()),
			                       dependGraph_(Repository::getInstance()->dependencyGraph())
		{
			pToker = new Toker;
			pToker->returnComments(false);
			pSemi = new SemiExp(pToker);
			pParser = new Parser(pSemi);
		}
		DependAnal::~DependAnal()
		{
			delete pParser;
			delete pSemi;
			delete pToker;
			if (pIn != nullptr)
				pIn->close();
			delete pIn;
		}
		//----< Resturns the dependency Table >--------------
		DependMap& getDependencyTable() { return dependMap_;  }
		bool ProcessCommandLine(int argc, char* argv[]);
		//----< Gets al the source files which are required for processing >--------------
		void getSourceFiles();
		//----< Process the sources code and builds the dependency graph >--------------
		void processSourceCode();
		bool Attach(const std::string& name);
		//----< Initilizes all the vertices beforehand >--------------
		void addVerticesToGraph();
		//----< Adds a dependency to the graph for a vertex >--------------
		void addDependencyToGraph(const std::string &fileName1_, const std::string &fileName2_);
		//----< Displays all the elements in the graphs >--------------
		void displayGraphWithDependencies();
		Graph<std::string, std::string>& getDependGraph() { return dependGraph_;  }
		
	private:
		Path path_;
		Patterns patterns_;
		Options options_;
		FileMap fileMap_;
		std::ifstream* pIn;
		Scanner::Toker* pToker;
		Scanner::SemiExp* pSemi;
		Parser* pParser;
		TypeTable &typeTable_;
		DependMap dependMap_;
		Graph<std::string, std::string> &dependGraph_;
		Display<std::string, std::string> dependDisplay_;
	};
}
#endif
