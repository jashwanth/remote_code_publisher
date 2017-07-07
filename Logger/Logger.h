#ifndef LOGGER_H
#define LOGGER_H
/////////////////////////////////////////////////////////////////////
// Logger.h - log text messages to std::ostream                    //
// ver 1.2                                                         //
//-----------------------------------------------------------------//
// Jim Fawcett (c) copyright 2015                                  //
// All rights granted provided this copyright notice is retained   //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Application: Several Projects, CSE687 - Object Oriented Design  //
// Author:      Jim Fawcett, Syracuse University, CST 4-187        //
//              jfawcett@twcny.rr.com                              //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package supports logging for multiple concurrent clients to a
* single std::ostream.  It does this be enqueuing messages in a
* blocking queue and dequeuing with a single thread that writes to
* the std::ostream.
*
* It provides two logging classes, a non-template Logger class with 
* instance methods, and a template class StaticLogger<int> with static 
* methods.
*
* In most of my code I use the following definitions:
* 
*   using Rslt = StaticLogger<0>;    // show program results
*   using Demo = StaticLogger<1>;    // show demonstration outputs
*   using Dbug = StaticLogger<2>;    // show debugging outputs
*
* That allows a user to determine the kinds of outputs the program
* presents based on which of these loggers are started, e.g., by
* changing only a couple of lines of code.
*
* Build Process:
* --------------
* Required Files: Logger.h, Logger.cpp, Utilities.h, Utilities.cpp
*
* Build Command: devenv logger.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.2 : 27 Aug 2016
* - added flushing of streams in Logger::flush()
* - call thread join on stop instead of spin locking
* ver 1.1 : 06 Aug 2016
* - added member function flush() to wait for output to complete
* ver 1.0 : 22 Feb 2016
* - first release
*
* Planned Additions and Changes:
* ------------------------------
* - fix pause locking
*/

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"

namespace Logging
{
  class Logger
  {
  public:
    Logger() {}
    void attach(std::ostream* pOut);
    void start();
    bool running();
    void pause(bool doPause);
    bool paused();
    void flush();
    void stop(const std::string& msg = "");
    void wait();
    void write(const std::string& msg);
    void title(const std::string& msg, char underline = '-');
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
  private:
    std::thread* _pThr;
    std::vector<std::ostream*> streams_;
    Async::BlockingQueue<std::string> _queue;
    bool _ThreadRunning = false;
    bool _Paused = false;
  };

  template<int i>
  class StaticLogger
  {
  public:
    static void attach(std::ostream* pOut) { _logger.attach(pOut); }
    static void start() { _logger.start(); }
    static bool running() { return _logger.running(); }
    static void pause(bool doPause) { _logger.pause(doPause); }
    static void paused() { return _logger.paused(); }
    static void flush() { _logger.flush(); }
    static void stop(const std::string& msg = "") { _logger.stop(msg); }
    static void wait() { _logger.wait(); }
    static void write(const std::string& msg) { _logger.write(msg); }
    static void title(const std::string& msg, char underline = '-') { _logger.title(msg, underline); }
    static Logger& instance() { return _logger; }
    StaticLogger(const StaticLogger&) = delete;
    StaticLogger& operator=(const StaticLogger&) = delete;
  private:
    static Logger _logger;
  };

  template<int i>
  Logger StaticLogger<i>::_logger;

  using Rslt = Logging::StaticLogger<0>;
  using Demo = Logging::StaticLogger<1>;
  using Dbug = Logging::StaticLogger<2>;
}
#endif
