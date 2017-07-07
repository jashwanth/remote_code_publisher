/////////////////////////////////////////////////////////////////////////
// FileSystemTest.cpp - Unit test cases for the FileSystem namespace   //
// ver 1.1                                                             //
// ------------------------------------------------------------------- //
// copyright © Jim Fawcett, 2012                                       //
// All rights granted provided that this notice is retained            //
// ------------------------------------------------------------------- //
// Jim Fawcett, Summer Projects, 2012                                  //
/////////////////////////////////////////////////////////////////////////
/*
 * ver 1.1 : 23 Feb 13
 * - added tests in File::testFileExists()
 * - added tests in Path::getExt(...) for test cases 
 *   *.cpp, ../noExt ..\\someFile.ext
 */
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "../FileSystemDemo/FileSystem.h"
#include "UnitTest.h"

using namespace FileSystem;
using namespace Test;

class TestFile : public TestBase<TestFile>
{
public:
  TestFile() : testFilePath("..\\FileSystemDemo\\FileSystem.cpp")
  {
    std::cout << "\n\n  Testing File class";
  }
  bool testName()
  {
    File testFile(testFilePath);
    std::string name = Path::getName(testFile.name());
    std::string check = Path::getName(testFilePath);
    return checkResult(name == check, "testName");
  }

  bool testGetAndPutLine()
  {
    std::string testline = "a test line";
    File outTestFile("tempTestFile.txt");
    outTestFile.open(File::out);
    if(outTestFile.isGood())
    {
      outTestFile.putLine(testline);
      outTestFile.putLine(testline);
    }
    std::string line1, line2;
    File inTestFile("tempTestFile.txt");
    inTestFile.open(File::in);
    if(inTestFile.isGood())
    {
      line1 = inTestFile.getLine();
      line2 = inTestFile.getLine();
    }
    return checkResult(
      line1 == testline && line2 == testline, 
      "testGetAndPutLine"
    );
  }

  bool testGetAndPutBlock()
  {
    Byte blockArray[] = { 'a', 'b', 'c', 'd' };
    Block block(&blockArray[0], &blockArray[4]);
    File outTestFile("tempTestFile.bin");
    outTestFile.open(File::out, File::binary);
    if(outTestFile.isGood())
    {
      outTestFile.putBlock(block);
      outTestFile.putBlock(block);
      outTestFile.close();
    }
    Block block1, block2;
    File inTestFile("tempTestFile.bin");
    inTestFile.open(File::in, File::binary);
    if(inTestFile.isGood())
    {
      block1 = inTestFile.getBlock(block.size());
      block2 = inTestFile.getBlock(block.size());
    }
    return checkResult(
      block1 == block && block2 == block, 
      "testGetAndPutBlock"
    );
  }

  bool testExists()
  {
    bool t1, t2;
    std::string testFileSpec = "test.txt";
    t1 = File::exists(testFileSpec);
    testFileSpec = "foobar";
    t2 = !File::exists(testFileSpec);
    return checkResult(t1 & t2, "testExists");
  }

  bool testCopyAndRemove()
  {
    bool t1, t2, t3, t4;
    std::string testFileSpec = "test.txt";
    std::string dst = "..\\test.txt";
    if(File::exists(testFileSpec))
    {
      t1 = File::copy(testFileSpec,dst,false);
      t2 = File::exists(dst);
      t3 = File::remove(dst);
      t4 = !File::exists(dst);
      return checkResult(t1 && t2 && t3 && t4, "testCopyAndRemove");
    }
    else
    {
      std::cout << "\n  " << testFileSpec << " does not exist";
      return failed("testCopyAndRemove");
    }
  }

  bool test()
  {
    bool t1, t2, t3, t4, t5;
    t1 = doTest(&TestFile::testName);
    t2 = doTest(&TestFile::testGetAndPutLine);
    t3 = doTest(&TestFile::testGetAndPutBlock);
    t4 = doTest(&TestFile::testExists);
    t5 = doTest(&TestFile::testCopyAndRemove);
    return t1 && t2 && t3 && t4 && t5;
  }
private:
    const std::string testFilePath;
};

class TestFileInfo : public TestBase<TestFileInfo>
{
public:
  TestFileInfo() : testFilePath("..\\FileSystemDemo\\FileSystem.cpp")
  {
    std::cout << "\n\n  Testing FileInfo class";
  }
  bool testConstruction()
  {
    FileInfo fi(testFilePath);
    return checkResult(fi.good(), "testConstruction");
  }
  bool testName()
  {
    FileInfo fi(testFilePath);
    std::string name = fi.name();
    std::string check = Path::getName(testFilePath);

    //throw std::exception("testFileInfoName");
    
    return checkResult(name == check, "testName");
  }
  bool testDate()
  {
    bool t1, t2, t3;
    FileInfo fi(testFilePath);
    std::string date = fi.date(FileInfo::fullformat);
    t1 = date.find("/") < date.size() && date.find(":") < date.size();
    date = fi.date(FileInfo::dateformat);
    t2 = date.find("/") < date.size() && date.find(":") >= date.size();
    date = fi.date(FileInfo::timeformat);
    t3 = date.find("/") >= date.size() && date.find(":") < date.size();
    return checkResult(t1 && t2 && t3, "testDate");
  }
  bool testSize()
  {
    bool t1, t2;
    File testSize("testsize");
    testSize.open(File::out, File::binary);
    FileInfo fi("testsize");
    size_t size = fi.size();
    t1 = size == 0;
    Byte bytes[] = { 'a', 'b', 'c', 'd' };
    Block blk(&bytes[0], &bytes[4]);
    testSize.putBlock(blk);
    testSize.close();
    FileInfo fi2("testsize");
    size = fi2.size();
    t2 = size == 4;
    return checkResult(t1 && t2, "testSize");
  }

  bool testNameCompare()
  {
    bool t1, t2, t3, t4, t5, t6, t7, t8;
    t1 = t2 = t3 = t4 = t5 = t6 = t7 = t8 = false;
    File testA("testA");
    testA.open(File::out);
    ::Sleep(50);
    if(testA.isGood())
      testA.putLine("a line of text");
    testA.close();
    File testB("testB");
    testB.open(File::out);
    ::Sleep(50);
    if(testB.isGood())
      testB.putLine("text");
    testB.close();
    FileInfo fiA("testA");
    FileInfo fiB("testB");
    t1 = fiA < fiB;
    t2 = !(fiA == fiB);
    t3 = !(fiA > fiB);
    t4 = fiA.earlier(fiB);
    t5 = !fiA.later(fiB);
    size_t sa = fiA.size();
    size_t sb = fiB.size();
    t6 = fiA.larger(fiB);
    t7 = !fiA.smaller(fiB);
    return checkResult(
      t1 && t2 && t3 && t4 && t5 && t6 && t7, 
      "testNameCompare"
    );
  }
  bool test()
  {
    bool t1, t2, t3, t4, t5;
    t1 = doTest(&TestFileInfo::testConstruction);
    t2 = doTest(&TestFileInfo::testName);
    t3 = doTest(&TestFileInfo::testDate);
    t4 = doTest(&TestFileInfo::testSize);
    t5 = doTest(&TestFileInfo::testNameCompare);
    return t1 && t2 && t3 && t4 && t5;
  }
private:
  const std::string testFilePath;
};

class TestPath : public TestBase<TestPath>
{
public:
  TestPath() : testFilePath("..\\FileSystemDemo\\FileSystem.cpp")
  {
    std::cout << "\n\n  Testing Path class";
  }
  bool TestGetFullFileSpec()
  {
    std::string currPath = Directory::getCurrentDirectory();
    std::string fileSpec = Path::getFullFileSpec(".");
    return checkResult(currPath == fileSpec, "TestGetFullFileSpec");
  }
  bool TestGetPath()
  {
    bool t1, t2;
    std::string path = Path::getPath("..\\foobar\\someFile.txt");
    std::string expected = "..\\foobar\\";
    t1 = path == expected;
    path = Path::getPath("../foobar/someFile.txt");
    expected = "../foobar/";
    t2 = path == expected;
    return checkResult(t1 && t2, "TestGetPath");
  }
  bool TestGetName()
  {
    bool t1, t2, t3;
    std::string name = Path::getName("..\\foobar\\someFile.txt");
    std::string expected = "someFile.txt";
    t1 = name == expected;
    name = Path::getName("../foobar/someFile.txt", false);
    expected = "someFile";
    t2 = name == expected;
    name = Path::getName("..\\foobar\\someFile");
    expected = "someFile";
    t3 = name == expected;
    return checkResult(t1 && t2 && t3, "TestGetName");
  }
  bool TestGetExt()
  {
    bool t1, t2, t3, t4, t5;
    std::string ext = Path::getExt("../foobar/someFile.txt");
    std::string expected = "txt";
    t1 = ext == expected;
    ext = Path::getExt("../foobar/someFile.txt");
    expected = "txt";
    t2 = ext == expected;
    ext = Path::getExt("../foobar/someFile");
    expected = "";
    t3 = ext == expected;
    ext = Path::getExt("../someDir/someFile.cpp");
    expected = "cpp";
    t4 = ext == expected;
    ext = Path::getExt("*.cpp");
    expected = "cpp";
    t5 = ext == expected;
    return checkResult(t1 && t2 && t3 && t4 && t5, "TestGetExt");
  }
  bool TestFileSpec()
  {
    bool t1, t2, t3;
    std::string path = "./foo";
    std::string name = "bar.txt";
    std::string expected = "./foo/bar.txt";
    std::string fileSpec = Path::fileSpec(path,name);
    t1 = fileSpec == expected;
    path = "..\\foo";
    name = "bar";
    expected = "..\\foo\\bar";
    fileSpec = Path::fileSpec(path,name);
    t2 = fileSpec == expected;
    path = "C:\\foo";
    name = "bar.cpp";
    expected = "C:\\foo\\bar.cpp";
    fileSpec = Path::fileSpec(path,name);
    t3 = fileSpec == expected;
    return checkResult(t1 && t2 && t3, "TestFileSpec");
  }
  bool test()
  {
    bool t1, t2, t3, t4, t5;
    t1 = doTest(&TestPath::TestGetFullFileSpec);
    t2 = doTest(&TestPath::TestGetPath);
    t3 = doTest(&TestPath::TestGetName);
    t4 = doTest(&TestPath::TestGetExt);
    t5 = doTest(&TestPath::TestFileSpec);
    return t1 && t2 && t3 && t4 && t5;
  }
private:
  const std::string testFilePath;
};

class TestDirectory : public TestBase<TestDirectory>
{
public:
  TestDirectory() : testPath_("c:\\temp")
  {
    std::cout << "\n\n  Testing Directory class";
  }
  bool TestCreateRemoveExists()
  {
    bool t1, t2, t3;
    std::string path = testPath_ + "\\TestDir";
    t1 = Directory::create(path) || Directory::exists(path);
    ::Sleep(100);
    t2 = Directory::exists(path);
    ::Sleep(100);
    t3 = Directory::remove(path) || !Directory::exists(path);
    return checkResult(t1 && t2 && t3, "TestCreateRemoveExists");
  }
  bool TestGetAndSetCurrentDirectory()
  {
    bool t1, t2, t3;
    std::string currPath = Directory::getCurrentDirectory();
    t1 = Path::getFullFileSpec(".") == currPath;
    Directory::setCurrentDirectory(testPath_);
    std::string newPath = Directory::getCurrentDirectory();
    t2 = newPath == Path::getFullFileSpec(testPath_);
    Directory::setCurrentDirectory(currPath);
    t3 = Directory::getCurrentDirectory() == currPath;
    return checkResult(t1 && t2 && t3, "TestGetAndSetCurrentDirectory");
  }
  bool TestGetFilesAndDirectories()
  {
    bool t1, t2, t3, t4, t5;

    // test finding files by copying know files then getting them
    std::string srcPath = "..\\FileSystemDemo";
    std::string dstPath = testPath_ + "\\testDir1";

    // create place to look
    Directory::create(dstPath);
    // store files there
    std::string fileSpec1 = Path::fileSpec(srcPath,"FileSystem.h");
    std::string fileSpec2 = Path::fileSpec(srcPath,"FileSystem.cpp");
    std::string dstFile1 = Path::fileSpec(dstPath,"FileSystem.h"); 
    File::copy(fileSpec1, dstFile1);
    std::string dstFile2 = Path::fileSpec(dstPath,"FileSystem.cpp");
    File::copy(fileSpec2, dstFile2);
    t1 = File::exists(dstFile1) && File::exists(dstFile2);
    // now test getFiles
    std::vector<std::string> files = Directory::getFiles(dstPath);
    std::vector<std::string>::iterator iter 
      = std::find(files.begin(), files.end(), "FileSystem.h");
    t2 = iter != files.end();
    iter = std::find(files.begin(), files.end(), "FileSystem.cpp");
    t3 = iter != files.end();

    // test finding directories by creating them and then finding them
    Directory::create(testPath_ + "\\testDir2");
    Directory::create(testPath_ + "\\testDir3");
    std::vector<std::string> dirs = Directory::getDirectories(testPath_);
    iter = std::find(dirs.begin(), dirs.end(), "testDir2");
    t4 = iter != dirs.end();
    iter = std::find(dirs.begin(), dirs.end(), "testDir3");
    t5 = iter != dirs.end();

    // clean up
    Directory::remove(testPath_ + "\\testDir1");  // fails because it has contents
    Directory::remove(testPath_ + "\\testDir2");  
    Directory::remove(testPath_ + "\\testDir3");

    return checkResult(t1 && t2 && t3 && t4 &&t5, "TestGetFilesAndDirectories");
  }
  bool test()  // overriding inherited test method
  {
    bool t1, t2, t3;
    t1 = doTest(&TestDirectory::TestCreateRemoveExists);
    t2 = doTest(&TestDirectory::TestGetAndSetCurrentDirectory);
    t3 = doTest(&TestDirectory::TestGetFilesAndDirectories);
    return t1 && t2 && t3;
  }
private:
  std::string testPath_;
};

bool test()
{
  bool t1, t2, t3, t4;
  TestFile tf;
  t1 = tf.test();
  TestFileInfo tfi;
  t2 = tfi.test();
  TestPath tp;
  t3 = tp.test();
  TestDirectory td;
  t4 = td.test();
  return t1 && t2 && t3;
}
int main(int argc, char* argv[])
{
  Title("Testing FileSystem namespace classes");

  if(test())
    std::cout << "\n\n  all tests passed\n\n";
  else
    std::cout << "\n\n  at least one test failed\n\n";
}