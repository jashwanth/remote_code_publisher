#ifndef FILEMGR_H
#define FILEMGR_H
/////////////////////////////////////////////////////////////////////
// FileMgr.h - find files matching specified patterns              //
//             on a specified path                                 //
// ver 2.2                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016       //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides a class, FileMgr, and object factory function,
* Create().  It's mission is to search the directory tree, rooted at
* a specified path, looking for files that match a set of specified
* patterns.
*
* It provides virtual functions file(...), dir(...), and done(), which
* an application can override to provide application specific processing
* for the events: file found, directory found, and finished processing.
*
* The package also provides interface hooks that serve the same purpose
* but allow multiple receivers for those events.
*
* Required Files:
* ---------------
*   FileMgr.h, FileMgr.cpp, IFileMgr.h, 
*   FileSystem.h, FileSystem.cpp
*
* Build Process:
* --------------
*   devenv FileMgr.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 2.2 : 28 Aug 2016
* - added more prologue comments
* ver 2.1 : 31 Jul 2016
* - added virtual file, dir, and done functions
* - this lets application override instead of registering for event
* ver 2.0 : 12 Mar 2016
* - fixes bug in directory recursion in find(path)
* - reduces scope of for loop in find for efficiency
* ver 1.0 : 11 Mar 2016
* - first release
*/

#include "IFileMgr.h"
#include "../FileSystem/FileSystem.h"

namespace FileManager
{
  class FileMgr : public IFileMgr
  {
  public:
    using patterns = std::vector<std::string>;
    using File = std::string;
    using Dir = std::string;
    using fileSubscribers = std::vector<IFileEventHandler*>;
    using dirSubscribers = std::vector<IDirEventHandler*>;
    using doneSubscribers = std::vector<IDoneEventHandler*>;

    virtual ~FileMgr() {}

    //----< set default file pattern >-------------------------------

    FileMgr(const std::string& path) : path_(path)
    {
      patterns_.push_back("*.*");
      pInstance_ = this;
    }
    //----< return instance of FileMgr >-----------------------------

    static IFileMgr* getInstance()
    {
      return pInstance_;
    }
    //----< add file pattern, removing default on first call >-------

    void addPattern(const std::string& patt)
    {
      if (patterns_.size() == 1 && patterns_[0] == "*.*")
        patterns_.pop_back();
      patterns_.push_back(patt);
    }
    //----< applications can overload this or reg for fileEvt >------

    virtual void file(const File& f)
    {
      ++numFilesProcessed;
      for (auto pEvtHandler : fileSubscribers_)
      {
        pEvtHandler->execute(f);
      }
    }
    //----< applications can overload this or reg for dirEvt >-------

    virtual void dir(const Dir& fpath)
    {
      for (auto pEvtHandler : dirSubscribers_)
      {
        pEvtHandler->execute(fpath);
      }
    }
    //----< applications can overload this or reg for doneEvt >------

    virtual void done()
    {
      for (auto pEvtHandler : doneSubscribers_)
      {
        pEvtHandler->execute(numFilesProcessed);
      }
    }
    //----< start search on previously specified path >--------------

    void search()
    {
      find(path_);
      done();
    }
    //----< search current path including subdirectories >-----------

    void find(const std::string& path)
    {
      std::string fpath = FileSystem::Path::getFullFileSpec(path);
      dir(fpath);
      for (auto patt : patterns_)
      {
        std::vector<std::string> files = FileSystem::Directory::getFiles(fpath, patt);
        for (auto f : files)
        {
          file(f);
        }
      }
      std::vector<std::string> dirs = FileSystem::Directory::getDirectories(fpath);
      for (auto d : dirs)
      {
        if (d == "." || d == "..")
          continue;
        std::string dpath = fpath + "\\" + d;
        find(dpath);
      }
    }
    //----< applications use this to register for notification >-----

    void regForFiles(IFileEventHandler* pHandler)
    {
      fileSubscribers_.push_back(pHandler);
    }
    //----< applications use this to register for notification >-----

    void regForDirs(IDirEventHandler* pHandler)
    {
      dirSubscribers_.push_back(pHandler);
    }
    //----< applications use this to register for notification >-----

    void regForDone(IDoneEventHandler* pHandler)
    {
      doneSubscribers_.push_back(pHandler);
    }
  private:
    std::string path_;
    patterns patterns_;
    size_t numFilesProcessed = 0;
    fileSubscribers fileSubscribers_;
    dirSubscribers dirSubscribers_;
    doneSubscribers doneSubscribers_;
    static IFileMgr* pInstance_;
  };

  inline IFileMgr* FileMgrFactory::create(const std::string& path)
  {
    return new FileMgr(path);
  }
}
#endif
