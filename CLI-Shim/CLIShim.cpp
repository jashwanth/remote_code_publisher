/////////////////////////////////////////////////////////////////////////
// CLIShim.cpp - C++\CLI layer is a proxy for calls into MockChannel   //
//                                                                     //
// Note:                                                               //
//   - build as a dll so C# can load                                   //
//   - link to MockChannel static library                              //
//                                                                     //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017           //
/////////////////////////////////////////////////////////////////////////

#include "CLIShim.h"
#include <iostream>

//----< convert std::string to System.String >---------------------------

String^ Shim::stdStrToSysStr(const std::string& str)
{
  return gcnew String(str.c_str());
}
//----< convert System.String to std::string >---------------------------

std::string Shim::sysStrToStdStr(String^ str)
{
  std::string temp;
  for (int i = 0; i < str->Length; ++i)
    temp += char(str[i]);
  return temp;
}
//----< Constructor sets up sender and receiver >------------------------

Shim::Shim()
{
  ObjectFactory factory;
  pSendr = factory.createSendr();
  pRecvr = factory.createRecvr();
  pMockChan = factory.createMockChannel(pSendr, pRecvr);
  pMockChan->start();
}
//----< put message into channel >---------------------------------------

void Shim::PostMessage(Message^ msg)
{
  pSendr->postMessage(sysStrToStdStr(msg));
}
//----< retrieve message from channel >----------------------------------

String^ Shim::GetMessage()
{
  std::string msg = pRecvr->getMessage();
  return stdStrToSysStr(msg);
}

#ifdef TEST_CLISHIM_LAYER

int main()
{
  String^ msg = "Hello from sender";
  Shim^ pShim = gcnew Shim();
  std::cout << "\n  sending message \"" << pShim->sysStrToStdStr(msg) << "\"";
  pShim->PostMessage(msg);
  String^ pReply = pShim->GetMessage();
  std::cout << "\n  received message \"" << pShim->sysStrToStdStr(pReply) << "\"";
  std::cout << "\n\n";
}
#endif
