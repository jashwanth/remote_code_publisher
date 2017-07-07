#ifndef WINDOWSHELPERS_H
#define WINDOWSHELPERS_H
/////////////////////////////////////////////////////////////////////
// WindowsHelper.h - small helper functions for using Windows API  //
// ver 1.0                                                         //
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
* This package supports programming to the Windows API.  It currently
* contains three global functions that convert strings to and from
* UTF-16, and retrieving error messages.
*
* Build Process:
* --------------
* Required Files: WindowsHelper.h, WindowwsHelper.cpp
*
* Build Command: devenv WindowsHelper.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 22 Feb 2016
* - first release
*
* Planned Additions and Changes:
* ------------------------------
* - none yet
*/

#include <string>
#include <vector>

namespace WindowsHelpers
{
	std::string wstringToString(const std::wstring& wstr);
	std::wstring stringToWideString(const std::string& str);
	std::string GetLastMsg(bool wantSocketMessage);
}

#endif

