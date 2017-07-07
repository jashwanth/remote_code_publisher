///////////////////////////////////////////////////////////////////////
// WindowsHelper.cpp - small helper functions for using Windows API  //
// ver 1.0                                                           //
//-------------------------------------------------------------------//
// Jim Fawcett (c) copyright 2015                                    //
// All rights granted provided this copyright notice is retained     //
//-------------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                              //
// Application: Several Projects, CSE687 - Object Oriented Design    //
// Author:      Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <string>
#include <winsock2.h>
#include "WindowsHelpers.h"

using namespace WindowsHelpers;

#pragma comment(lib, "Ws2_32.lib")

std::string WindowsHelpers::wstringToString(const std::wstring& wstr)
{
	std::string rtn;
	for (auto ch : wstr)
		rtn += static_cast<char>(ch);
	rtn += '\0';
	return rtn;
}

std::wstring WindowsHelpers::stringToWideString(const std::string& str)
{
	std::wstring rtn;
	for (auto ch : str)
		rtn += ch;
	rtn += static_cast<wchar_t>('\0');
	return rtn;
}

//----< get socket error message string >----------------------------

std::string WindowsHelpers::GetLastMsg(bool WantSocketMsg) {

	// ask system what type of error occurred

	DWORD errorCode;
	if (WantSocketMsg)
		errorCode = WSAGetLastError();
	else
		errorCode = GetLastError();
	if (errorCode == 0)
		return "no error";

	// map errorCode into a system defined error string

	DWORD dwFlags =
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER;
	LPCVOID lpSource = NULL;
	DWORD dwMessageID = errorCode;
	DWORD dwLanguageId = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
	LPSTR lpBuffer;
	DWORD nSize = 0;
	va_list *Arguments = NULL;

	FormatMessage(
		dwFlags, lpSource, dwMessageID, dwLanguageId,
		(LPTSTR)&lpBuffer, nSize, Arguments
	);

	std::string _msg(lpBuffer);
	LocalFree(lpBuffer);
	return _msg;
}

#ifdef TEST_WINDOWSHELPERS

int main()
{

}

#endif
