/////////////////////////////////////////////////////////////////////
// DependAnal.cpp - Publish the code as HTML files using the       //
//                  dependency between the files                   //
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

#include <vector>
#include <sstream>
#include "PublishCode.h"
#include "../FileSystem/FileSystem.h"
#include "../Analyzer/Executive.h"

using namespace CodeAnalysis;
using namespace std;

CodePublisher::CodePublisher(DependMap& depndencymap_) : dependMap_(depndencymap_)
{}


void CodePublisher::deleteFilesinDirectory(std::vector<std::string> filesList)
{
	try
	{
		for (int i = 0; i < filesList.size(); i++)
		{
			string removePath = storePath_ + "/" + filesList[i];
			if (FileSystem::File::exists(removePath)) {
				FileSystem::File::remove(removePath);
				//	cout << "Deleted the file: " << removePath << "\n";
			}
		}
	}
	catch (exception e) {
		cout << "Exception caught while trying to delete the file\n";
	}
}

void CodePublisher::demonstrateReq7()
{
	Utilities::StringHelper::Title("Demonstrating Requirement7: ");
	/* First generate dependency html file */
	std::string dependencyhtmfile = storePath_ + "/" + "dependencyTable.htm";
	cout << "\nDependency htm file is " << dependencyhtmfile << "\n";

	if (FileSystem::File::exists(dependencyhtmfile))
	{
		FileSystem::File::remove(dependencyhtmfile);
		//	cout << "File: " << dependencyhtmfile << "to be copied to HTML page is not present" << endl;
	}
	Utilities::StringHelper::Title("Created dependencyTable.htm which holds all the dependency information fetched from proj#2");
	ofstream ofs(dependencyhtmfile);
	for (auto item : fileMap_)
	{
		for (auto file : item.second)
		{
			string onlyFileName1 = file.substr(file.find_last_of("\\") + 1);
			for (std::string items : dependMap_[file])
			{
				string onlyFileName2 = items.substr(items.find_last_of("\\") + 1);
				string addhref1 = "<a href=" + onlyFileName1 + ".htm>" + onlyFileName1 + "</a>";
				string addhref2 = "<a href=" + onlyFileName2 + ".htm>" + onlyFileName2 + "</a>";
				ofs << addhref1 << "  -------->  " << addhref2;
				ofs << "<br/><br/>";
			}
		}
	}
	ofs.close();
	Utilities::StringHelper::Title("Requirement7 is met: Open each of the files to check the dependency label at the top section");
}


void CodePublisher::createIISIndex()
{
	Utilities::StringHelper::Title("Demonstrating Requirement7: ");
	/* First generate dependency html file */
	std::string cpath = IISPath_;
	//std::string cpath = "C:/Sumanth/GrammarHelpers";
	std::string dependencyhtmfile = cpath + "/" + "dependencyTable.htm";
	cout << "\nDependency htm file is " << dependencyhtmfile << "\n";

	if (FileSystem::File::exists(dependencyhtmfile))
	{
		FileSystem::File::remove(dependencyhtmfile);
		//	cout << "File: " << dependencyhtmfile << "to be copied to HTML page is not present" << endl;
	}
	Utilities::StringHelper::Title("Created dependencyTable.htm which holds all the dependency information fetched from proj#2");
	ofstream ofs(dependencyhtmfile);
	for (auto item : fileMap_)
	{
		for (auto file : item.second)
		{
			string onlyFileName1 = file.substr(file.find_last_of("\\") + 1);
			for (std::string items : dependMap_[file])
			{
				string onlyFileName2 = items.substr(items.find_last_of("\\") + 1);
				string addhref1 = "<a href=" + onlyFileName1 + ".htm>" + onlyFileName1 + "</a>";
				string addhref2 = "<a href=" + onlyFileName2 + ".htm>" + onlyFileName2 + "</a>";
				ofs << addhref1 << "  -------->  " << addhref2;
				ofs << "<br/><br/>";
			}
		}
	}
	ofs.close();
	Utilities::StringHelper::Title("Requirement7 is met: Open each of the files to check the dependency label at the top section");
}

void CodePublisher::demonstrateReq4()
{
	Utilities::StringHelper::Title("Demonstrating Requirement4: ");
	Utilities::StringHelper::Title("Displaying the HTML Header file added to source files:");
	ifstream ifs("../../Addhtmlheader.txt");
	std::string line;
	char ch;
	stringstream ss;
	ss << "\n";
	while (ifs.get(ch))
		ss << ch;
	cout << ss.str() << "\n";
	ss.str("");
	ifs.close();
	Utilities::StringHelper::Title("Displayed html header with links to CSS stylesheet and javascript file");
	Utilities::StringHelper::Title("Displaying the javascript file to expand or collapse the body of functions:");
	ifstream ifs2("../../javascript.js");
	ss << "\n";
	while (ifs2.get(ch))
		ss << ch;
	cout << ss.str() << "\n";
	ss.str("");
	ifs2.close();
	Utilities::StringHelper::Title("Displaying the stylesheet file to customise the look of html pages:");

	ifstream ifs3("../../stylesheet.css");
	ss << "\n";
	while (ifs3.get(ch))
		ss << ch;
	cout << ss.str() << "\n";
	ifs3.close();
	Utilities::StringHelper::Title("Displayed css for styling of web pages and javascript to expand, collapse function bodies ");
	Utilities::StringHelper::Title("Requirement4 is met: Adding the above header, script, styling to all HTML pages");
}

bool CodePublisher::ProcessCommandLine(int argc, char* argv[])
{
	if (argc < 2) {return false;}
	try {
		path_ = FileSystem::Path::getFullFileSpec(argv[1]);
		if (!FileSystem::Directory::exists(path_))
		{
			std::cout << "\n\n  path \"" << path_ << "\" does not exist\n\n";
			return false;
		}
		for (int i = 2; i < argc; ++i)
		{
			if (argv[i][0] == '/')
				options_.push_back(argv[i][1]);
			else
				patterns_.push_back(argv[i]);
		}
		if (patterns_.size() == 0) {return false;}
		string convPathtoString(argv[6]);
		string convPathtoStringForIIS(argv[7]);
		storePath_ = convPathtoString;
		IISPath_ = convPathtoStringForIIS;
		cout << " The IIS physical Directory path is: " << endl;
		cout << IISPath_ << endl;
		cout << " The IIS virtual Directory path is: " << argv[8] << endl;
		if (!FileSystem::Directory::exists(convPathtoString)) {
			FileSystem::Directory::create(convPathtoString);
			cout << "\nCreated the destination directory " << convPathtoString << " to store HTM files"; }
		if (!FileSystem::Directory::exists(convPathtoStringForIIS)) {
			FileSystem::Directory::create(convPathtoStringForIIS); 
			cout << "\nCreated the virtual directory " << convPathtoStringForIIS << " to store HTM files"; }
		else
		{
			/* clear all the htm files in the directory */
			deleteFilesinDirectory(FileSystem::Directory::getFiles(convPathtoString, "*.*"));
			FileSystem::Directory::remove(convPathtoString);
			cout << "\nRemoved the existing directory " << convPathtoString << "\n";
			FileSystem::Directory::create(convPathtoString);
		}
		cout << "\n The Path to store HTML files is " << storePath_ << " \n";
		Utilities::StringHelper::Title("Processed all the files in the directory path provided");
		Utilities::StringHelper::Title("All the files with matching patterns were processed");
	}
	catch (std::exception& ex) {
		std::cout << "\n\n  command line argument parsing error:";
		std::cout << "\n  " << ex.what() << "\n\n";
		return false; }

	return true;
}

void CodePublisher::getSourceFiles()
{
	AnalFileMgr fm(path_, fileMap_);
	for (auto patt : patterns_)
		fm.addPattern(patt);
	fm.search();
}

/* This function utility is only used by copyFiletoHtm, copyFiletoHtmToIIS to 
reduce the function line size to 50. */
void CodePublisher::copyContentToHtmlFile(std::string oldfile, std::string newfilename)
{
	static int counter = 0;
	//ifstream ifs1(oldfile);
	//while (getline(ifs1, line))
	//{
	////	ifs1 >> line;
	//	ofs << line << "\n";
	//}
	//ifs1.close();
	ofstream ofs(newfilename, ios_base::out | ios_base::app);
	char ch;
	fstream fin(oldfile, fstream::in);
	ofs << "\n";
	while (fin >> noskipws >> ch)
	{
		if (ch == '<')
		{
			ofs << "&lt;";
			continue;
		}
		if (ch == '>')
		{
			ofs << "&gt;";
			continue;
		}
		ofs << ch;
		if (ch == '{')
		{
			ofs << "\n";
			//	cout << "I saw a char" << endl;
			string divid = "simplediv" + std::to_string(counter);
			string buttonid = "myButton" + std::to_string(counter);
			counter++;
			//	cout << "divid is: " << divid << endl;
			//	cout << "buttonid is: " << buttonid << endl;
			ofs << "<input onclick=\"expand_collapse(" << divid << ".id, this.id)\" type=\"button\" value=\" ^ \" id=\"" << buttonid << "\" />";
			ofs << "<div id=\"" << divid << "\" style=\"display:block; \">";
		}
		if (ch == '}')
		{
			ofs << "</div>";
		}
	}
	ofs << "</pre>\n</body>\n</html>\n";
	ofs.close();
}

void CodePublisher::copyFiletoHtm(std::string file)
{
	if (!FileSystem::File::exists(file))
	{
		cout << "File: " << file << "to be copied to HTML page is not present" << endl;
	}
	std::string fileName_ = file.substr(file.find_last_of("\\") + 1);
	std::string newFilename_ = storePath_;

	newFilename_ += "/";
	newFilename_ += fileName_ + ".htm";
	ofstream ofs(newFilename_);
	cout << "\n Created a new HTML file: " << newFilename_ << "\n";

	ifstream ifs("../../Addhtmlheader.txt");
	if (!ifs.is_open())
	{
		cout << "Couldnot open Addhtmlheader.txt file\n";
		return;
	}
	std::string line;
	char ch;
	while (ifs.get(ch))
	{
		ofs << ch;
		//	ofs << line << "\n";
		//		cout << line << endl;
	}
	//	cout << "\nAdded header to the new htm file created\n";
	ifs.close();
	ofs << "<hr />";
	ofs << "<div class=\"indent\" >\n" << "<h4>Dependencies:</h4>\n";
	for (std::string items : dependMap_[file])
	{
		string onlyFileName = items.substr(items.find_last_of("\\") + 1);
		//	cout << "Dependency for file: " << fileName_ << " is " << onlyFileName << "\n";
		//	string addhref = "<a href=" + storePath_ + onlyFileName + ".htm>" + onlyFileName + "</a>";
		//	string addhref = "<a href=" + newFilename_ + ".htm>" + "</a>";
		string addhref = "<a href=" + onlyFileName + ".htm>" + onlyFileName + "</a>";
		ofs << addhref << "\n";
	}
	ofs << "</div>\n  <hr />\n <pre>";
	ofs.close();
	copyContentToHtmlFile(file, newFilename_);
	Utilities::StringHelper::Title("Requirement 3 is met: Repository created HTML file from source code C++ files");
}


void CodePublisher::copyFiletoHtmToIIS(std::string file)
{
	if (!FileSystem::File::exists(file))
	{
		cout << "File: " << file << "to be copied to HTML page is not present" << endl;
	}
	std::string fileName_ = file.substr(file.find_last_of("\\") + 1);
	std::string newFilename_ = IISPath_;
	//std::string newFilename_ = "C:/Sumanth/GrammarHelpers/";

	newFilename_ += "/";
	newFilename_ += fileName_ + ".htm";
	ofstream ofs(newFilename_);
	cout << "Created a new HTML file: " << newFilename_ << "\n";

	ifstream ifs("../../Addhtmlheader.txt");
	if (!ifs.is_open())
	{
		cout << "Couldnot open Addhtmlheader.txt file\n";
		return;
	}
	std::string line;
	char ch;
	while (ifs.get(ch))
	{
		ofs << ch;
		//	ofs << line << "\n";
		//		cout << line << endl;
	}
	//	cout << "\nAdded header to the new htm file created\n";
	ifs.close();
	ofs << "<hr />";
	ofs << "<div class=\"indent\" >\n" << "<h4>Dependencies:</h4>\n";
	for (std::string items : dependMap_[file])
	{
		string onlyFileName = items.substr(items.find_last_of("\\") + 1);
		//	cout << "Dependency for file: " << fileName_ << " is " << onlyFileName << "\n";
		//	string addhref = "<a href=" + storePath_ + onlyFileName + ".htm>" + onlyFileName + "</a>";
		//	string addhref = "<a href=" + newFilename_ + ".htm>" + "</a>";
		string addhref = "<a href=" + onlyFileName + ".htm>" + onlyFileName + "</a>";
		ofs << addhref << "\n";
	}
	ofs << "</div>\n  <hr />\n <pre>";
	ofs.close();
	copyContentToHtmlFile(file, newFilename_);
	Utilities::StringHelper::Title("Requirement 3 is met: Copied the content of source code to HTML file");
}


void CodePublisher::displayDependencies()
{
	unordered_map<std::string, std::set<std::string>>::iterator it = dependMap_.begin();
	cout << "\nSize of map is " << dependMap_.size() << "\n";
	for (it; it != dependMap_.end(); it++)
	{
		cout << "\nDependency for file : " << it->first << " is \n";
		for (std::string str : it->second)
		{
			cout << std::setw(8) << " " << str << "\n";
		}
	}
}

void CodePublisher::processSourceCode()
{
	demonstrateReq7();
	createIISIndex();
//	Utilities::StringHelper::Title("Demonstrating Requirement9# : Trying to publish files from packages based on command arguments.");
	cout << "\n";
	for (auto item : fileMap_)
	{
		for (auto file : item.second)
		{
			cout << "\nFound a file : " << file << endl;
			copyFiletoHtm(file);
			copyFiletoHtmToIIS(file);
			//std::string newHtmFile = file + ".htm";
		}
	}
//	Utilities::StringHelper::Title("Requirement9 is met : Created all the html files by publisher package for project3 packages.");
}

#ifdef TEST_PUBLISHCODE
int main()
{

}
#endif