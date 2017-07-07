#pragma once
///////////////////////////////////////////////////////////////////////
// CLIShim.h - C++\CLI layer is a proxy for calls into MockChannel   //
//                                                                   //
// Note:                                                             //
//   - build as a dll so C# can load                                 //
//   - link to MockChannel static library                            //
//                                                                   //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017         //
///////////////////////////////////////////////////////////////////////

#include "../MockChannel/MockChannel.h"
#include <string>
using namespace System;

public ref class Shim
{
public:
  using Message = String;

  Shim();
  void PostMessage(Message^ msg);
  String^ GetMessage();
  String^ stdStrToSysStr(const std::string& str);
  std::string sysStrToStdStr(String^ str);
private:
  ISendr* pSendr;
  IRecvr* pRecvr;
  IMockChannel* pMockChan;
};


