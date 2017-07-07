/////////////////////////////////////////////////////////////////////
// FileMgr.cpp - find files matching specified patterns            //
//               on a specified path                               //
// ver 2.2                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016       //
/////////////////////////////////////////////////////////////////////

#include "FileMgr.h"
#include <iostream>

using namespace FileManager;

IFileMgr* FileMgr::pInstance_;

IFileMgr* IFileMgr::getInstance()
{
  return FileMgr::getInstance();
}

#ifdef TEST_FILEMGR

using namespace FileManager;

struct FileHandler : IFileEventHandler
{
  void execute(const std::string& fileSpec)
  {
    std::cout << "\n  --   " << fileSpec;
  }
};

struct DirHandler : IDirEventHandler
{
  void execute(const std::string& dirSpec)
  {
    std::cout << "\n  ++ " << dirSpec;
  }
};

struct DoneHandler : IDoneEventHandler
{
  void execute(size_t numFilesProcessed)
  {
    std::cout << "\n\n  Processed " << numFilesProcessed << " files";
  }
};
int main()
{
  std::cout << "\n  Testing FileMgr";
  std::cout << "\n =================";

  std::string path = FileSystem::Path::getFullFileSpec("..");
  IFileMgr* pFmgr = FileMgrFactory::create(path);

  FileHandler fh;
  DirHandler dh;
  DoneHandler dnh;

  pFmgr->regForFiles(&fh);
  pFmgr->regForDirs(&dh);
  pFmgr->regForDone(&dnh);

  pFmgr->addPattern("*.h");
  pFmgr->addPattern("*.cpp");
  //pFmgr->addPattern("*.log");

  pFmgr->search();

  std::cout << "\n\n";
  return 0;
}
#endif
