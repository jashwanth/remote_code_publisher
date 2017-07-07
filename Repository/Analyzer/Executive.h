#pragma once
/////////////////////////////////////////////////////////////////////
// Executive.h - Organizes and Directs Code Analysis               //
// ver 1.4                                                         //
//-----------------------------------------------------------------//
// Jashwanth Reddy (c) copyright 2016                              //
// All rights granted provided this copyright notice is retained   //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Platform:    Mac OS, Windows 10 virtual Machine                 //
// Application: Project #3, CSE687 - Object Oriented Design, S2016 //
// Author:      Jashwanth Reddy Gangula, Syracuse University       //
//              jgangula@syr.edu	                               //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  ===================
*  Executive uses Parser, ScopeStack, and AbstrSynTree packages to
*  provide static code analysis for C++ and C# files contained in a
*  specified directory tree.  
*
*  It finds files for analysis using FileMgr and, for each file found,
*  uses parser rules to detect:
*  - namespace scopes
*  - class scopes
*  - struct scopes
*  - control scopes
*  - function definition scopes
*  - data declarations
*  and uses rule actions to build an Abstract Sytax Tree (AST) 
*  representing the entire code set's static structure.  Each detected
*  scope becomes a node in the AST which spans all of the files analyzed, 
*  rooted in a top-level Global Namespace scope.
*
*  Executive supports displays for:
*  - a list of all files processed with their source lines of code count.
*  - an indented representation of the AST.
*  - a list of all functions that are defined, organized by package, along
*    with their starting lines, size in lines of code, and complexity as
*    measured by the number of their descendent nodes.
*  - a list of all functions which exceed specified function size and/or
*    complexity.
*
*  Because much of the important static structure information is contained
*  in the AST, it is relatively easy to extend the application to evaluate
*  additional information, such as class relationships, dependency network,
*  and static design flaws.
*
*  Required Files:
*  ---------------
*  - Executive.h, Executive.cpp
*  - Parser.h, Parser.cpp, ActionsAndRules.h, ActionsAndRules.cpp
*  - ConfigureParser.h, ConfigureParser.cpp
*  - ScopeStack.h, ScopeStack.cpp, AbstrSynTree.h, AbstrSynTree.cpp
*  - ITokenCollection.h, SemiExp.h, SemiExp.cpp, Tokenizer.h, Tokenizer.cpp
*  - IFileMgr.h, FileMgr.h, FileMgr.cpp, FileSystem.h, FileSystem.cpp
*  - Logger.h, Logger.cpp, Utilities.h, Utilities.cpp
*
*  Maintanence History:
*  --------------------
*  ver 1.5 : 1 April 2016
   - added code publisher to publish all the pattern files as html files.
*  ver 1.4 : 26 Feb 2016
*  - added annunciation of version number
*  - ActionsAndRules modified to fix minor bug in operator name collection.
*  - SemiExpression, Tokenizer, and Utilities modified to use isspace from <locale>
*    rather than <cctype> to avoid error handling some non-ASCII characters.
*  ver 1.3 : 28 Oct 2016
*  - added test for interface in AbstrSynTree functions show() and Add(ASTNode*)
*  - wrapped path in quotes for display
*  - added check for Byte Order Mark (BOM) in ConfigureParser.cpp
*  - wrapped path in quotes in Window.cpp in case path has spaces, necessary
*    since path is passed to Executive on command line.
*  ver 1.2 : 27 Oct 2016
*  - fixed bug in displayMetrics(ASTNode*) by adding test for interface
*  ver 1.1 : 28 Aug 2016
*  - many changes: added new functions and modified existing functions
*  ver 1.0 : 09 Aug 2016
*  - first release
*
*/
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <iosfwd>
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../Parser/Parser.h"
#include "../FileMgr/FileMgr.h"
#include "../Parser/ConfigureParser.h"
#include "../Utilities/Utilities.h"


namespace CodeAnalysis
{
  using Utils = Utilities::StringHelper;

  ///////////////////////////////////////////////////////////////////
  // AnalFileMgr class derives from FileManager::FileMgr
  // - provides application specific handling of file and dir events

  class AnalFileMgr : public FileManager::FileMgr
  {
  public:
    using Path = std::string;
    using File = std::string;
    using Files = std::vector<File>;
    using Pattern = std::string;
    using Ext = std::string;
    using FileMap = std::unordered_map<Pattern, Files>;

    AnalFileMgr(const Path& path, FileMap& fileMap);
    virtual void file(const File& f);
    virtual void dir(const Dir& d);
    virtual void done();
    size_t numFiles();
    size_t numDirs();
  private:
    Path d_;
    FileMap& fileMap_;
    bool display_;
    size_t numFiles_;
    size_t numDirs_;
  };

  ///////////////////////////////////////////////////////////////////
  // CodeAnalysisExecutive class directs C++ and C# code analysis

  class CodeAnalysisExecutive
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
    using FileNodes = std::vector<std::pair<File, ASTNode*>>;
    using Slocs = size_t;
    using SlocMap = std::unordered_map<File, Slocs>;
	using DependMap = std::unordered_map<std::string, std::set<std::string>>;
    CodeAnalysisExecutive();
    virtual ~CodeAnalysisExecutive();

    CodeAnalysisExecutive(const CodeAnalysisExecutive&) = delete;
    CodeAnalysisExecutive& operator=(const CodeAnalysisExecutive&) = delete;

    void showCommandLineArguments(int argc, char* argv[]);
    bool ProcessCommandLine(int argc, char* argv[]);
    FileMap& getFileMap();
    std::string getAnalysisPath();
    virtual void getSourceFiles();
    virtual void processSourceCode(bool showActivity);
    void complexityAnalysis();
    std::vector<File>& cppHeaderFiles();
    std::vector<File>& cppImplemFiles();
    std::vector<File>& csharpFiles();
    Slocs fileSLOCs(const File& file);
    size_t numFiles();
    size_t numDirs();
    std::string systemTime();
    virtual void displayMetrics(ASTNode* root);
    virtual void displayMetrics();
    virtual void displayMetricSummary(size_t sMax, size_t cMax);
    virtual void displaySlocs();
    virtual void displayAST();
    virtual void dispatchOptionalDisplays();
    virtual void setDisplayModes();
    void startLogger(std::ostream& out);
    void flushLogger();
    void stopLogger();
    void setLogFile(const File& file);
	/*static void setDepMapForLazyLoading(DependMap& dependMapLL_) {
		depmapforlazyloading = dependMapLL_;
	}*/
	static DependMap depmapforlazyloading;
  private:
    void setLanguage(const File& file);
    void showActivity(const File& file);
    void clearActivity();
    virtual void displayHeader();
    virtual void displayMetricsLine(const File& file, ASTNode* pNode);
    virtual void displayDataLines(ASTNode* pNode, bool isSummary = false);
    std::string showData(const Scanner::ITokCollection* ptc);
    Parser* pParser_;
    ConfigParseForCodeAnal configure_;
    Repository* pRepo_;
    Path path_;
    Patterns patterns_;
    Options options_;
    FileMap fileMap_;
    FileNodes fileNodes_;
    std::vector<File> cppHeaderFiles_;
    std::vector<File> cppImplemFiles_;
    std::vector<File> csharpFiles_;
    size_t numFiles_;
    size_t numDirs_;
    SlocMap slocMap_;
    bool displayProc_ = false;
    std::ofstream* pLogStrm_ = nullptr;
	
	

  };

}
