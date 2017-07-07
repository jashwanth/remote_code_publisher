#ifndef GRAPH_H
#define GRAPH_H
///////////////////////////////////////////////////////////////
// Graph.h - Graph Library                                   //
// Ver 1.6                                                   //
// Language:    Visual C++ 2012                              //
// Platform:    Lenova ThinkPad E420s, Windows 7             //
// Application: Help for CSE687 - OOD, Pr#1, Spring 2013     //
// Author:      Jim Fawcett, CST 4-187, Syracuse University  //
//              (315) 443-3948, jfawcett@twcny.rr.com        //
///////////////////////////////////////////////////////////////
/*
Package Operations:
===================
A graph is a collection of vertices, connected by edges.  Each
vertex holds some kind of information held in an instance of
a type V and an integer id, which should be unique with a given
graph.  Each edge connects a parent vertex with a child
vertex and it too holds information in an instance of a type E.

This package provides two classes that support creation of
instances of a graph: Vertex<V,E> and Graph<V,E>.  These are
template-based with parameters V and E, as described above.

The graph holds an aggregated collection of vertices.  Each
vertex may hold one or more edges.  The edge is implemented
as an instance of std::pair<int,E> where the first parameter
is the id of the child vertex and the second parameter is
an instance of the edge information type E.

Note:
It is important that vertex ids be unique.  If you set any of
them with the second argument of the Vertex constructor or with
the id() function it is up to you to ensure uniqueness.  If you
don't explicitly set any ids then the constructor insures that
vertices get sequential ids in the order constructed.

Maintanence Information:
========================
Required files:
---------------
Graph.h, Graph.cpp,
XmlReader.h, XmlReader.cpp,
XmlWriter.h, XmlWriter.cpp

Build Process:
--------------
Using Visual Studio Command Prompt:
devenv Graph.sln /rebuild debug

Revision History:
-----------------
ver 1.6 : 23 Feb 17
- added inlines to to helper functions at the end of this file
ver 1.5 : 16 Feb 13
- now supports dfs with both function pointers and functors
ver 1.4 : 11 Feb 13
- removed indent function, now part of XmlWriter,
replaced ctor with default param with two ctors,
one supplies sequential ids, the other accepts user defined id
ver 1.3 : 28 Jan 13
- added GraphToXmlString and GraphFromXmlString functions
ver 1.2 : 25 Jan 13
- changed id() return to reference so can be changed
ver 1.1 : 21 Jan 13
- added prologue comments, compile test directive
ver 1.0 : 14 Jan 13
- first release

*/
/*
* To Do:
* - move GraphToXml and GraphFromXml to separate package
* - Strong Components, Topological Sorting
* - Find disconnected graph partitions
*/

#include <vector>
#include <unordered_map>
#include <string>
//#include "XmlWriter.h"
//#include "XmlReader.h"
#include <iostream>
#include <sstream>

namespace CodeAnalysis
{
	///////////////////////////////////////////////////////////////
	// Vertex class

	template<typename V, typename E>
	class Vertex
	{
	public:
		typedef std::pair<int, E> Edge;  // graph index of target vertex, edge type
		typename typedef std::vector<Edge>::iterator iterator;
		iterator begin();
		iterator end();
		Vertex(V v, size_t id);
		Vertex(V v);
		void add(Edge& edge);
		// Vertex(const Vertex<V,E>& v);                  compiler generated is correct
		// Vertex<V,E>& operator=(const Vertex<V,E>& v);  compiler generated is correct
		Edge& operator[](size_t i);
		Edge operator[](size_t i) const;
		V& value();
		size_t& id();
		size_t size();
		bool& mark();
		friend bool operator ==(Vertex& a, Vertex& b)
		{
			bool rVal = false;
			//  if (!(a._name.compare(b._name)))
			if (a.value() == b.value())
				rVal = true;
			return rVal;
		}
	private:
		std::vector<Edge> _edges;
		V _v;
		size_t _id;
		static size_t count;
		bool _mark;
	};
	//----< reserve memory for, and initialize, static count >-----
	template<typename V, typename E>
	size_t Vertex<V, E>::count = 0;

	//----< set and return boolean mark, used for traversal >------
	template<typename V, typename E>
	bool& Vertex<V, E>::mark() { return _mark; }

	//----< return iterator pointing to first edge >---------------
	template<typename V, typename E>
	typename Vertex<V, E>::iterator Vertex<V, E>::begin() { return _edges.begin(); }

	//----< return iterator pointing to one past last edge >-------
	template<typename V, typename E>
	typename Vertex<V, E>::iterator Vertex<V, E>::end() { return _edges.end(); }

	//----< construct instance, specifying id - must be unique >---
	template<typename V, typename E>
	Vertex<V, E>::Vertex(V v, size_t id) : _v(v), _id(id), _mark(false) {}

	//----< construct instance - creates id sequentially >-------
	template<typename V, typename E>
	Vertex<V, E>::Vertex(V v) : _v(v), _id(count++), _mark(false) {}

	//----< add edge to vertex edge collection >-------------------
	template<typename V, typename E>
	void Vertex<V, E>::add(Edge& edge) { _edges.push_back(edge); }

	//----< index non-const vertex's edges >-----------------------
	template<typename V, typename E>
	typename Vertex<V, E>::Edge& Vertex<V, E>::operator[](size_t i) { return _edges[i]; }

	//----< index const vertex's edges >---------------------------
	template<typename V, typename E>
	typename Vertex<V, E>::Edge Vertex<V, E>::operator[](size_t i) const { return _edges[i]; }

	//----< set and read value of vertex's held type, V >----------
	template<typename V, typename E>
	V& Vertex<V, E>::value() { return _v; }

	//----< return vertex's id >-----------------------------------
	template<typename V, typename E>
	size_t& Vertex<V, E>::id() { return _id; }

	//----< return number of edges >-------------------------------
	template<typename V, typename E>
	size_t Vertex<V, E>::size() { return _edges.size(); }

	///////////////////////////////////////////////////////////////
	// Graph class

	template<typename V, typename E>
	class Graph
	{
	public:
		typename typedef std::vector< Vertex<V, E> >::iterator iterator;
		iterator begin();
		iterator end();
		// Graph(const Graph<V,E>& g);                  compiler generated is correct
		// Graph<V,E>& operator=(const Graph<V,E>& g);  compiler generated is correct
		Vertex<V, E>& operator[](size_t i);
		Vertex<V, E> operator[](size_t i) const;
		bool addVertex(Vertex<V, E> v);
		bool addEdge(E eval, Vertex<V, E>& parent, Vertex<V, E>& child);
		size_t findVertexIndexById(size_t id);
		size_t size();
		template<typename F>
		void dfs(Vertex<V, E>& v, F f);
	private:
		std::vector< Vertex<V, E> > adj;
		std::unordered_map<size_t, size_t> idMap; // id maps to graph index
		template<typename F>
		void dfsCore(Vertex<V, E>& v, F f);
	};
	//----< return iterator pointing to first vertex >-------------
	template<typename V, typename E>
	typename Graph<V, E>::iterator Graph<V, E>::begin() { return adj.begin(); }

	//----< return iterator pointing one past last vertex >--------
	template<typename V, typename E>
	typename Graph<V, E>::iterator Graph<V, E>::end() { return adj.end(); }

	//----< index non-const graph's vertex collection >------------
	template<typename V, typename E>
	typename Vertex<V, E>& Graph<V, E>::operator[](size_t i) { return adj[i]; }

	//----< index const graph's vertex collection >----------------
	template<typename V, typename E>
	typename Vertex<V, E> Graph<V, E>::operator[](size_t i) const { return adj[i]; }

	//----< add vertex to graph's vertex collection >--------------
	template<typename V, typename E>
	bool Graph<V, E>::addVertex(Vertex<V, E> v)
	{
		/* check if the vertex is already in the adjaceny list before adding to
		avoid duplicates */
		for (size_t i = 0; i < adj.size(); i++)
		{
			Vertex<V, E> temp = adj[i];
			if (temp == v)
			{
				return false;
			}
		}
		adj.push_back(v);
		idMap[v.id()] = adj.size() - 1;
		return true;
	}
	//----< return number of vertices in graph's collection >------
	template<typename V, typename E>
	size_t Graph<V, E>::size() { return adj.size(); }

	//----< return index of vertex with specified id >-------------
	template<typename V, typename E>
	size_t Graph<V, E>::findVertexIndexById(size_t id)
	{
		return idMap[id];
	}
	//----< add edge from specified parent to child vertices >-----
	template<typename V, typename E>
	bool Graph<V, E>::addEdge(E eVal, Vertex<V, E>& parent, Vertex<V, E>& child)
	{
		size_t childIndex = findVertexIndexById(child.id());
		if (childIndex == adj.size())
			throw std::exception("no edge child");
		size_t parentIndex = findVertexIndexById(parent.id());
		if (parentIndex == adj.size())
			throw std::exception("no edge parent");
		Vertex<V, E>::Edge e;
		e.first = (int)childIndex;
		e.second = eVal;
		/* Check if the edge already exists to avoid duplicate addition */
		for (size_t i = 0; i < adj[parentIndex].size(); i++)
		{
			Vertex<V, E>::Edge e1 = adj[parentIndex][i];
			if (e1.first == e.first) return false;
		}
		adj[parentIndex].add(e);
		return true;
	}
	//----< recursive depth first search with action f >-----------
	template<typename V, typename E>
	template<typename F>
	void Graph<V, E>::dfsCore(Vertex<V, E>& v, F f)
	{
		f(v);
		v.mark() = true;
		for (auto edge : v)
		{
			if (adj[edge.first].mark() == false)
				dfsCore(adj[edge.first], f);
		}
		for (auto& vert : adj)
		{
			if (vert.mark() == false)
				dfsCore(vert, f);
		}
	}
	//----< depth first search, clears marks for next search >-----
	template<typename V, typename E>
	template<typename F>
	void Graph<V, E>::dfs(Vertex<V, E>& v, F f)
	{
		dfsCore(v, f);
		for (auto& vert : adj)
			vert.mark() = false;
	}

	///////////////////////////////////////////////////////////////
	// Display class

	template<typename V, typename E>
	class Display
	{
	public:
		typedef Graph<V, E> graph;
		typedef Vertex<V, E> vertex;
		typedef std::pair<int, E> edge;

		static std::vector< Vertex<V, E> > vertsWithNoParents(graph& g)
		{
			std::vector<size_t> parentCount;
			graph::iterator iter = g.begin();
			while (iter != g.end())
			{
				parentCount.push_back(0);
				++iter;
			}
			iter = g.begin();
			while (iter != g.end())
			{
				vertex v = *iter;
				for (size_t i = 0; i<v.size(); ++i)
				{
					edge e = v[i];
					parentCount[e.first]++;
				}
				++iter;
			}
			std::vector<vertex> noParents;
			for (size_t j = 0; j<g.size(); ++j)
				if (parentCount[j] == 0)
					noParents.push_back(g[j]);
			return noParents;
		}

		static void show(graph& g)
		{
			graph::iterator iter = g.begin();
			while (iter != g.end())
			{
				vertex v = *iter;
			//	std::cout << "\n  VertexId  = " << v.id() << ", File = " << (v.value()).c_str();
				cout << endl << "Vertex : " << v.value() << endl;
				for (size_t i = 0; i < v.size(); ++i)
				{
					vertex::Edge edge = v[i];
					cout << "     " << setw(15) << setfill(' ') << "Edge : " << g[edge.first].value() << endl;
				}
				++iter;
			}
		}
	};
	//----< convert integer to string >--------------------------
	inline std::string ToString(int i)
	{
		std::ostringstream in;
		in << i;
		return in.str();
	}
	//----< convert string to integer >--------------------------
	inline int ToInt(const std::string& str)
	{
		std::istringstream in(str);
		int retn;
		in >> retn;
		return retn;
	}
	//----< construct graph from XML string >--------------------
	template<typename V, typename E>
	void GraphFromXmlString(Graph<V, E>& gr, const std::string& Xml)
	{
		// add vertices
		XmlReader vrdr(Xml);
		while (vrdr.next())
		{
			if (vrdr.tag() == "vertex")
			{
				XmlReader::attribElems elems = vrdr.attributes();
				if (elems.size() > 1)
				{
					Vertex<V, E> vert(elems[1].second);
					vert.id() = ToInt(elems[0].second);
					gr.addVertex(vert);
				}
				else
				{
					throw std::exception("ill-formed graph XML");
				}
			}
		}
		// add edges
		XmlReader erdr(Xml);
		int parentId;
		while (erdr.next())
		{
			if (erdr.tag() == "vertex")
			{
				XmlReader::attribElems elems = erdr.attributes();
				if (elems.size() > 0)
				{
					parentId = ToInt(elems[0].second);
				}
				else
				{
					throw std::exception("ill-formed graph XML");
				}
			}
			if (erdr.tag() == "edge")
			{
				XmlReader::attribElems eelems = erdr.attributes();
				if (eelems.size() > 1)
				{
					int childId = ToInt(eelems[0].second);
					E eval = eelems[1].second;
					int parentIndex = gr.findVertexIndexById(parentId);
					Vertex<V, E> parent = gr[parentIndex];
					int childIndex = gr.findVertexIndexById(childId);
					Vertex<V, E> child = gr[childIndex];
					gr.addEdge(eval, parent, child);
				}
				else
				{
					throw std::exception("ill-formed graph XML");
				}
			}
		}
	}
	//----< serialize graph to XML string >----------------------
	template<typename V, typename E>
	std::string GraphToXmlString(Graph<V, E>& gr)
	{
		XmlWriter wrg;
		wrg.indent();
		wrg.start("graph");
		for (auto vert : gr)
		{
			wrg.start("vertex");
			wrg.addAttribute("id", ToString(vert.id()));
			wrg.addAttribute("value", vert.value());
			for (auto edge : vert)
			{
				wrg.start("edge");
				wrg.addAttribute("targetId", ToString(gr[edge.first].id()));
				wrg.addAttribute("value", edge.second);
				wrg.end();
			}
			wrg.end();
		}
		wrg.end();
		return wrg.xml();
	}
}
#endif
