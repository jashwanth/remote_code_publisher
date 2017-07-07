#pragma once
/////////////////////////////////////////////////////////////////////
// IFileMgr.h - interface for finding files matching specified     //
//              patterns on a specified path                       //
// ver 3.0                                                         //
// Jim Fawcett, MidtermCodeSupplement, Spring 2016                 //
/////////////////////////////////////////////////////////////////////

#include <string>

namespace FileManager
{
  struct IFileEventHandler
  {
    virtual void execute(const std::string& fileSpec) = 0;
  };

  struct IDirEventHandler
  {
    virtual void execute(const std::string& dirSpec) = 0;
  };

  struct IDoneEventHandler
  {
    virtual void execute(size_t numFilesProcessed) = 0;
  };

  struct IFileMgr
  {
    virtual ~IFileMgr() {}
    virtual void addPattern(const std::string& patt) = 0;
    virtual void search() = 0;
    virtual void find(const std::string& path) = 0;
    virtual void regForFiles(IFileEventHandler* pEventHandler) = 0;
    virtual void regForDirs(IDirEventHandler* pEventHandler) = 0;
    virtual void regForDone(IDoneEventHandler* pEventHandler) = 0;
    static IFileMgr* getInstance();
  };

  struct FileMgrFactory
  {
    static IFileMgr* create(const std::string& path);
  };
}