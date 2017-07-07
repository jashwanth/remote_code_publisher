#ifndef FILESYSTEM_H
#define FILESYSTEM_H
/////////////////////////////////////////////////////////////////////////////
// FileSystem.h - Support file and directory operations                    //
// ver 2.6                                                                 //
// ----------------------------------------------------------------------- //
// copyright © Jim Fawcett, 2012                                           //
// All rights granted provided that this notice is retained                //
// ----------------------------------------------------------------------- //
// Language:    Visual C++, Visual Studio 2010                             //
// Platform:    Dell XPS 8300, Core i7, Windows 7 SP1                      //
// Application: Summer Projects, 2012                                      //
// Author:      Jim Fawcett, CST 4-187, Syracuse University                //
//              (315) 443-3948, jfawcett@twcny.rr.com                      //
/////////////////////////////////////////////////////////////////////////////
/*
 * Module Operations:
 * ==================
 * This module provides classes, File, FileInfo, Path, Directory, and 
 * FileSystemSearch.
 *
 * The File class supports opening text and binary files for either input 
 * or output.  File objects have names, get and put lines of text, get and
 * put blocks of bytes if binary, can be tested for operational state,
 * cleared of errors, and output File objects can be flushed to their streams.
 *
 * FileInfo class accepts a fully qualified filespec and supports queries
 * concerning name, time and date, size, and attributes.  You can compare
 * FileInfo objects by name, date, and size.
 *
 * Path class provides static methods to turn a relative filespec into an
 * absolute filespec, return the path, name, or extension of the filespec,
 * and build filespecs from path and name.  Path also provides methods to
 * convert filespecs to uppercase and to lowercase.
 *
 * The Directory class supports getting filenames or directories from a 
 * fully qualified filespec, e.g., path + filename pattern using static
 * methods.  It also provides non-static methods to get and set the current
 * directory.
 *
 * Public Interface:
 * =================
 * File f(filespec,File::in,File::binary);
 * File g(filespec,File::out,File::binary);
 * while(f.isGood()) {
 *   block b = f.getBlock();
 *   g.putBlock(b);
 * }
 * File h(filespec,File::in);
 * h.readLine();
 *
 * FileInfo fi("..\foobar.txt");
 * if(fi.good())
 *   ...
 * std::string filespec = "..\temp.txt";
 * std::string fullyqualified = Path::getFullFileSpec(filename);
 *  -- This uses the current path to expand a relative path.
 *  -- If you have a filespec relative to some other path
 *  -- you need to setCurrentPath(newPath), use getFullFileSpec(...)
 *  -- then reset the current path with setCurrentPath(origPath)
 * std::string fullyqualified = Path::fileSpec(path, filename);
 *  -- This simply concatenates path with filename
 * std::string path = Path::getPath(fullyqualified);
 * std::string name = Path::getName(fullyqualified);
 * std::string extn = Path::getExt(fullyqualified);
 *
 * Directory d;
 * std::string dir = d.getCurrentDirectory();
 * d.setCurrentDirectory(dir);
 * std::vector<std::string> files = Directory::getFiles(path, pattern);
 * std::vector<std::string> dirs = Directory::getDirectories(path);
 * 
 * Required Files:
 * ===============
 * FileSystem.h, FileSystem.cpp
 *
 * Build Command:
 * ==============
 * cl /EHa /DTEST_FILESYSTEM FileSystem.cpp
 *
 * Maintenance History:
 * ====================
 * ver 2.6 : 04 Apr 15
 * - added File::getBuffer(...) and File::putBuffer(...) for use with
 *   Sockets package.
 * ver 2.5 : 02 Feb 15
 * - added test for empty file in File::readAll()
 * - fixed a debug assert that happens in std::isspace in File::ReadAll()
 *   on certain non-ascii characters that may occur in a binary file.
 *   The fix was to replace std::isspace(char) with an overload
 *   std::isspace(char, locale&) from the standard locale library.
 * - added some demo code in test stub to show how to handle attempting
 *   to read non-text file with File::ReadLine() and File::readAll();
 * ver 2.4 : 01 Feb 15
 * - added File::readAll()
 * - improved test stub testing
 * ver 2.3 : 31 Jan 15
 * - fixed bug in File::getLine() discovered by Huanming Fang
 *   This appears to be a change in behavior of istream::getline(...) with C++11
 * - added comments in Public Interface, above, to clarify way that Path::getFullFileSpec
 *   and Path::fileSpec work.
 * ver 2.2 : 23 Feb 13
 * - fixed bug in Path::getExt(...) discovered by Yang Zhou and Kevin Kong
 * ver 2.1 : 07 Jun 12
 * - moved FileSystemSearch to cpp file to make private
 * - added bool withExt parameter to Path::getName
 * - made Directory's getCurrentDirectory and setCurrentDirectory static
 * - added exists, create, and remove to Directory class
 * ver 2.0 : 04 Jun 12
 * - made a few small changes to provide same interface as 
 *   Linux version.
 * ver 1.0 : 31 Jan 09
 * - first release
 */

#ifndef WIN32_LEAN_AND_MEAN  // prevents duplicate includes of core parts of windows.h in winsock2.h
#define WIN32_LEAN_AND_MEAN
#endif

#include <fstream>
#include <string>
#include <vector>
#include <windows.h>

namespace FileSystem
{
  /////////////////////////////////////////////////////////
  // Block
  
  typedef char Byte;

  class Block
  {
  public:
    Block(size_t size=0) : bytes_(size) {}
    Block(Byte* beg, Byte* end);
    void push_back(Byte b);
    Byte& operator[](size_t i);
    Byte operator[](size_t i) const;
    bool operator==(const Block&) const;
    bool operator!=(const Block&) const;
    size_t size() const;
  private:
    std::vector<Byte> bytes_;
  };

  /////////////////////////////////////////////////////////
  // File
  
  class File
  {
  public:
    using byte = char;
    enum direction { in, out };
    enum type { text, binary };
    File(const std::string& filespec);
    bool open(direction dirn, type typ=File::text);
    ~File();
    std::string name();
    std::string getLine(bool keepNewLine = false);
    std::string readAll(bool KeepNewLines = false);
    void putLine(const std::string& line, bool wantReturn=true);
    Block getBlock(size_t size);
    void putBlock(const Block&);
    size_t getBuffer(size_t bufLen, byte* buffer);
    void putBuffer(size_t bufLen, byte* buffer);
    bool isGood();
    void clear();
    void flush();
    void close();
    static bool exists(const std::string& file);
    static bool copy(const std::string& src, const std::string& dst, bool failIfExists=false);
    static bool remove(const std::string& filespec);
  private:
    std::string name_;
    std::ifstream* pIStream;
    std::ofstream* pOStream;
    direction dirn_;
    type typ_;
    bool good_;
  };

  inline std::string File::name() { return name_; }

  /////////////////////////////////////////////////////////
  // FileInfo

  class FileInfo
  {
  public:
    enum dateFormat { fullformat, timeformat, dateformat };
    FileInfo(const std::string& fileSpec);
    bool good();
    std::string name() const;
    std::string date(dateFormat df=fullformat) const;
    size_t size() const;
    
    bool isArchive() const;
    bool isCompressed() const;
    bool isDirectory() const;
    bool isEncrypted() const;
    bool isHidden() const;
    bool isNormal() const;
    bool isOffLine() const;
    bool isReadOnly() const;
    bool isSystem() const;
    bool isTemporary() const;

    bool operator<(const FileInfo& fi) const;
    bool operator==(const FileInfo& fi) const;
    bool operator>(const FileInfo& fi) const;
    bool earlier(const FileInfo& fi) const;
    bool later(const FileInfo& fi) const;
    bool smaller(const FileInfo& fi) const;
    bool larger(const FileInfo& fi) const;
  private:
    bool good_;
    static std::string intToString(long i);
    WIN32_FIND_DATAA data;
  };

  /////////////////////////////////////////////////////////
  // Path

  class Path
  {
  public:
    static std::string getFullFileSpec(const std::string& fileSpec);
    static std::string getPath(const std::string& fileSpec);
    static std::string getName(const std::string& fileSpec, bool withExt=true);
    static std::string getExt(const std::string& fileSpec);
    static std::string fileSpec(const std::string& path, const std::string& name);
    static std::string toLower(const std::string& src);
    static std::string toUpper(const std::string& src);
  };
  
  /////////////////////////////////////////////////////////
  // Directory

  class Directory
  {
  public:
    static bool create(const std::string& path);
    static bool remove(const std::string& path);
    static bool exists(const std::string& path);
    static std::string getCurrentDirectory();
    static bool setCurrentDirectory(const std::string& path);
    static std::vector<std::string> getFiles(const std::string& path=".", const std::string& pattern="*.*");
    static std::vector<std::string> getDirectories(const std::string& path=".", const std::string& pattern="*.*");
  private:
    //static const int BufSize = 255;
    //char buffer[BufSize];
  };
}

#endif
