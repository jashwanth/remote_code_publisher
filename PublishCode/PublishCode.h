#pragma once
#ifndef PUBLISHCODE_H
#define PUBLISHCODE_H
/////////////////////////////////////////////////////////////////////
// DependAnal.h - Publish the code as HTML files using the         //
//                dependency between the files                     //
// ver 1.0                                                         //
//-----------------------------------------------------------------//
// Jashwanth Reddy Gangula (c) copyright 2015                      //
// All rights granted provided this copyright notice is retained   //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Application: Dependency Based Code Publisher, Project 3,        //
//              Spring 2016, CSE687 - Object Oriented Design       //
// Author:      Jashwanth Reddy Gangula, Syracuse University       //
//              jgangula@syr.edu                                   //
/////////////////////////////////////////////////////////////////////

//* Operations:
// This package parses the arguments , gets all the pattern files , and processes
// each file. All the dependencies for each file are added at the top of html
// file for easy  navigation to its dependencies.

#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <iostream>

namespace CodeAnalysis
{
	class CodePublisher
	{
	public:
		using Path = std::string;
		using Pattern = std::string;
		using Patterns = std::vector<Pattern>;
		using File = std::string;
		using Files = std::vector<File>;
		using Ext = std::string;
		using Options = std::vector<char>;
		using FileMap = std::unordered_map<Pattern, Files>;
		using DependMap = std::unordered_map<std::string, std::set<std::string>>;

		CodePublisher(DependMap& depndencymap_);

		bool ProcessCommandLine(int argc, char* argv[]);
		//----< Gets al the source files which are required for processing >--------------
		void getSourceFiles();
		//----< Process the sources code and builds the dependency graph >--------------
		void processSourceCode();

		void copyFiletoHtm(std::string file);
		void copyFiletoHtmToIIS(std::string file);
		void displayDependencies();
		void deleteFilesinDirectory(std::vector<std::string> filesList);
		void demonstrateReq4();
		void demonstrateReq7();
		void createIISIndex();
		void copyContentToHtmlFile(std::string oldfile, std::string newfilename);
	private:
		Path path_;
		Patterns patterns_;
		Options options_;
		FileMap fileMap_;
		DependMap &dependMap_;
		Path storePath_;
		Path IISPath_;
	};
}
#endif /* PUBLISHCODE_H */
