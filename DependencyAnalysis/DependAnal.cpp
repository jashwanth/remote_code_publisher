/////////////////////////////////////////////////////////////////////
// DependAnal.cpp - Store the type information needed for          //
//                  dependency between the files                   //
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

// This cpp file implements all the declarations in header files 
// of DependAnal.h

#include "DependAnal.h"
#include "../FileSystem/FileSystem.h"
#include "../Parser/Parser.h"
#include <vector>
#include <sstream>

using namespace CodeAnalysis;
void DependAnal::displayGraphWithDependencies()
{
	dependDisplay_.show(dependGraph_);
}

void DependAnal::addVerticesToGraph()
{
	static int i = 0;
	for (auto item : fileMap_)
	{
		for (auto file : item.second)
		{
			Vertex<std::string, std::string> vert(file, i);
			if (dependGraph_.addVertex(vert) == true)
			{
				i++;
			}
		}
	}

}

void DependAnal::addDependencyToGraph(const std::string &fileName1_, const std::string &fileName2_)
{
	static int i = 0;
	int index1 = -1;
	int index2 = -1;
	for (size_t j = 0; j < dependGraph_.size(); j++)
	{
		Vertex<std::string, std::string>& v1 = dependGraph_[j];
		if (v1.value() == fileName1_)
		{
			index1 = (int)j;
		}
		if (v1.value() == fileName2_)
		{
			index2 = (int)j;
		}
	}

	std::stringstream ss;
	ss.str("");
	ss << "edge" << i;
	if ((index1 != -1) && (index2 != -1))
	{
		if (dependGraph_.addEdge(ss.str(), dependGraph_[index1], dependGraph_[index2]) == true)
		{
			i++;
		}
	}
}


/*
* Arguments are:
* - path: possibly relative path to folder containing all analyzed code,
*   e.g., may be anywhere in the directory tree rooted at that path
* - patterns: one or more file patterns of the form *.h, *.cpp, and *.cs
* - options: /m (show metrics), /s (show file sizes), and /a (show AST)
*/
bool DependAnal::Attach(const std::string& name)
{
	if (pToker == 0)
		return false;
	if (pIn != nullptr)
	{
		pIn->close();
		delete pIn;
	}
	pIn = new std::ifstream(name);
	if (!pIn->good())
		return false;

	// check for Byte Order Mark (BOM)
	char a, b, c;
	a = pIn->get();
	b = pIn->get();
	c = pIn->get();
	if (a != (char)0xEF || b != (char)0xBB || c != (char)0xBF)
		pIn->seekg(0);

	if (!pIn->good())
	{
		// empty file, but openable, so return true, avoids error message
		return true;
	}
	return pToker->attach(pIn);
}


bool DependAnal::ProcessCommandLine(int argc, char* argv[])
{
	if (argc < 2)
	{
		return false;
	}
	try {
		path_ = FileSystem::Path::getFullFileSpec(argv[1]);
		if (!FileSystem::Directory::exists(path_))
		{
			std::cout << "\n\n  path \"" << path_ << "\" does not exist\n\n";
			return false;
		}
	/*	Utilities::StringHelper::Title("Requirement8 is met: Processing the source code based on arguments");
		for (int i = 0; i < argc; i++)
		{
			cout << endl << "Argument " << i << ": " << argv[i];
		}*/
		for (int i = 2; i < argc; ++i)
		{
			if (argv[i][0] == '/')
				options_.push_back(argv[i][1]);
			else
				patterns_.push_back(argv[i]);
		}
		if (patterns_.size() == 0)
		{
			return false;
		}
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n  command line argument parsing error:";
		std::cout << "\n  " << ex.what() << "\n\n";
		return false;
	}
	/*Utilities::StringHelper::Title("Processed all the files in the directory path provided");
	Utilities::StringHelper::Title("All the files with matching patterns were processed");*/
	//if (argc <= 6)
	//{
	//	Utilities::StringHelper::Title("Default files to store xml for dependency analysis is : storedependency.xml");
	//}
	//else
	//{
	//    string dependfile = "File provided to store the dependecy in xml format is: ";
	//	dependfile.append(argv[6]);
	//	Utilities::StringHelper::Title(dependfile);
	//}
	//Utilities::StringHelper::Title("End of Requirement 8");
	return true;
}
void DependAnal::getSourceFiles()
{
	AnalFileMgr fm(path_, fileMap_);
	for (auto patt : patterns_)
		fm.addPattern(patt);
	fm.search();
}
void DependAnal::processSourceCode()
{
	addVerticesToGraph();
	for (auto item : fileMap_) {
		for (auto file : item.second) {
			Attach(file);
			std::string fileName_;
			if (file.find_last_of("\\") < file.length()) {
				fileName_ = file.substr(file.find_last_of("\\") + 1);}
			while (pParser->next()) {
				std::vector<std::string>::iterator it = pSemi->begin();
				for (it; it != pSemi->end(); it++) {
					/* check if the token is present in the TypeTable */
					if (typeTable_.getTypeTable().find(*it) != typeTable_.getTypeTable().end()) {
						if (file != typeTable_.getTypeTable().find(*it)->second.filename) {
							if (dependMap_[file].insert(typeTable_.getTypeTable().find(*it)->second.filename).second 
								!= false) {
								std::string fileName2_ = typeTable_.getTypeTable().find(*it)->second.filename;
								addDependencyToGraph(file, fileName2_);
								std::string str1 = file.substr(file.find_last_of("\\") + 1);
								std::string str2 = fileName2_.substr(fileName2_.find_last_of("\\") + 1);
								if ((str1.find_last_of(".cpp") < str1.length()) 
									&& (str2.find_last_of(".h") < str2.length())) {
									if (str1.substr(0, str1.find_last_of(".cpp") - 4).compare
									(str2.substr(0, str2.find_last_of(".h") - 2)) == 0) {
										dependMap_[fileName2_].insert(file);
										addDependencyToGraph(fileName2_, file);}}			}}}}}
		}
	}
	Utilities::StringHelper::Title("Processed the Source code and added the dependencies to graph class and dependency table");
}

#ifdef TEST_DEPENDANAL
int main(int argc, char* argv)
{
	/* Get the typetable and dependGraph from Repository singleton instance */
	DependAnal dan;
	dan.ProcessCommandLine(argc, argv);
	dan.getSourceFiles();
	dan.processSourceCode();
	dan.displayGraphWithDependencies();
	return 0;
}
#endif