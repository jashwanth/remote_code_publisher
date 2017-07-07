#ifndef STRONGCOMP_H
#define STRONGCOMP_H
///////////////////////////////////////////////////////////////
// StrongComp.h - Tarjan Algorithm to find SCC               //
// Ver 1.6                                                   //
// Language:     Visual C++ 2015                             //
// Platform:     Mac Machine , Windows 7  Virtual Machine    //
// Application:  for CSE687 - OOD, Pr#2, Spring 2017         //
// Author:       Jashwanth Reddy Gangula, Syracuse University//
//              (315) 949-8857, jgangula@syr.edu             //
///////////////////////////////////////////////////////////////
//*Operations:
//*This package uses the Graph which is obtained from the DependencyGraph
//*package to find the strong components in the graph.
//*To achive this,Tarjan Algorithm is used.
//*Refer the link from geeksforgeeks.com for further details.
//*Required Files:
//*ActionsAndRules.h and ActionsAndRules.cpp
//*Graph.h and Graph.cpp


#include "../Graph/Graph.h"
#include "../Parser/ActionsAndRules.h"
#include <stack>
#include <set>

namespace CodeAnalysis
{
	class StrongComp
	{
		using StrongMap = std::unordered_map<std::string, std::set<std::string>>;
	private:
		
		Graph<std::string, std::string> &graph_;
		StrongMap strongMap_;
		/* Count the total Strongly Connected Components */
		int countStrongCC_;
		size_t totalVertex;
	public:
		//----< Initializes the grah of the Strong components from the Repository >--------------
		StrongComp::StrongComp() : graph_(Repository::getInstance()->dependencyGraph()) {
			countStrongCC_ = 0;
			totalVertex = graph_.size();
		}
		//----< Returns the StrongMap >--------------
		StrongMap& getStrongMap() { return strongMap_; }
		//----< Core function which find the Strong components >--------------
		void SCC();
		// A Recursive DFS based function used by SCC()
		void SCCUtil(int u, int disc[], int low[], std::stack<int> *st, bool stackMember[]);
	};
}
#endif