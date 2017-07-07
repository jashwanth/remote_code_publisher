#include "StrongComp.h"
#include <vector>
#include <sstream>
#include <algorithm>
#define NIL -1

using namespace CodeAnalysis;

// A recursive function that finds and prints strongly connected
// components using DFS traversal
// u --> The vertex to be visited next
// disc[] --> Stores discovery times of visited vertices
// low[] -- >> earliest visited vertex (the vertex with minimum
//             discovery time) that can be reached from subtree
//             rooted with current vertex
// *st -- >> To store all the connected ancestors (could be part
//           of SCC)
// stackMember[] --> bit/index array for faster check whether
//                  a node is in stack
void StrongComp::SCCUtil(int u, int disc[], int low[], std::stack<int> *st,
	                bool stackMember[])
{
	// A static variable is used for simplicity, we can avoid use
	// of static variable by passing a pointer.
	static int time = 0;
	// Initialize discovery time and low value
	disc[u] = low[u] = ++time;
	st->push(u);
	stackMember[u] = true;
	Vertex<std::string, std::string> &vert = graph_[u];

	for (size_t i = 0; i < vert.size(); i++)
	{
		Vertex<std::string, std::string>::Edge e1 = vert[i];
		Vertex<std::string, std::string> &vert2 = graph_[e1.first];
		for (size_t j = 0; j < graph_.size(); j++)
		{
			Vertex<std::string, std::string> &v1 = graph_[j];
			if (v1.value() == vert2.value())
			{
				// If v1 is not visited yet, then recur for it
				if (disc[v1.id()] == -1)
				{
					SCCUtil(v1.id(), disc, low, st, stackMember);

					// Check if the subtree rooted with 'v' has a
					// connection to one of the ancestors of 'u'
					// Case 1 (per above discussion on Disc and Low value)
					low[u] = min(low[u], low[v1.id()]);
				}
				// Update low value of 'u' only of 'v' is still in stack
				// (i.e. it's a back edge, not cross edge).
				// Case 2 (per above discussion on Disc and Low value)
				else if (stackMember[v1.id()] == true)
					low[u] = min(low[u], disc[v1.id()]);
				break;
			}
		}
	}

	// head node found, pop the stack and print an SCC
	int w = 0;  // To store stack extracted vertices
	if (low[u] == disc[u])
	{
		std::cout << "\n\n";
		std::stringstream ss;
		ss << "scc" << countStrongCC_;
		while (st->top() != u)
		{
			w = (int)st->top();
			Vertex<std::string, std::string>& vert = graph_[w];
			//std::cout << w << " " << vert.value() << ", \n" ;
			stackMember[w] = false;
			strongMap_[ss.str()].insert(vert.value());
			st->pop();
		}
		w = (int)st->top();
		Vertex<std::string, std::string> &vert = graph_[w];
		//std::cout << w << vert.value() << "\n";
		stackMember[w] = false;
		st->pop();
		strongMap_[ss.str()].insert(vert.value());
		countStrongCC_++;
		ss.str("");
	}
}


void StrongComp::SCC()
{
	int *disc = new int[totalVertex];
	int *low = new int[totalVertex];
	bool *stackMember = new bool[totalVertex];
	std::stack<int> *st = new std::stack<int>();

	// Initialize disc and low, and stackMember arrays
	for (int i = 0; i < totalVertex; i++)
	{
		disc[i] = NIL;
		low[i] = NIL;
		stackMember[i] = false;
	}

	// Call the recursive helper function to find strongly
	// connected components in DFS tree with vertex 'i'
	/*Graph<std::string, std::string>::iterator it = graph_.begin();
	for (it; it != graph_.end(); it++)
	{

	}*/
	for (int i = 0; i < totalVertex; i++)
	{
		//Vertex<std::string, std::string> v = graph_[i];
		if (disc[i] == NIL)
			SCCUtil(i, disc, low, st, stackMember);
	}
}
#ifdef TEST_STRONGCOMP
int main()
{
	return 0;
}
#endif