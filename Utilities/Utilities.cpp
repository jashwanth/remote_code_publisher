///////////////////////////////////////////////////////////////////////
// Utilities.cpp - small, generally usefule, helper classes          //
// ver 1.4                                                           //
// Language:    C++, Visual Studio 2015                              //
// Platform:    Dell XPS 8900, Windows 10                            //
// Application: Most Projects, CSE687 - Object Oriented Design       //
// Author:      Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////

//#include <cctype>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <locale>
#include <clocale>
#include "Utilities.h"

using namespace Utilities;

/*
*  Note: 
*  The std::string src argument must be pass by value so that we can pass
*  std::ostringstream.out() to the first argument and std::ostringstream out
*  to the last argument, e.g., the same std::ostringstream object for both
*  arguments.  Otherwise we attempt to insert out's string into itself.
*/
void StringHelper::Title(std::string src, std::ostream& out)
{
  out << "\n  " << src;
  out << "\n " << std::string(src.size() + 2, '=');
}

void StringHelper::Title(std::string src, std::ostringstream& out)
{
  out.str("");
  out << "\n  " << src;
  out << "\n " << std::string(src.size() + 2, '=');
}

void StringHelper::title(std::string src, std::ostream& out)
{
  out << "\n  " << src;
  out << "\n " << std::string(src.size() + 2, '-');
}

void StringHelper::title(std::string src, std::ostringstream& out)
{
  out.str("");
  out << "\n  " << src;
  out << "\n " << std::string(src.size() + 2, '-');
}

void StringHelper::sTitle(std::string src, size_t offset, size_t width, std::ostream& out, char underline)
{
  if (width < src.size())
    width = src.size();
  size_t lwidth = (width - src.size()) / 2;
  out << "\n  " << std::setw(offset) << "" << std::setw(lwidth) << " " << src;
  out << "\n " << std::setw(offset) << "" << std::string(width + 2, underline);
}

void StringHelper::sTitle(std::string src, size_t offset, size_t width, std::ostringstream& out, char underline)
{
  out.str("");
  if (width < src.size())
    width = src.size();
  size_t lwidth = (width - src.size()) / 2;
  out << "\n  " << std::setw(offset) << "" << std::setw(lwidth) << " " << src;
  out << "\n " << std::setw(offset) << "" << std::string(width + 2, underline);
}

std::vector<std::string> StringHelper::split(const std::string& src)
{
  std::vector<std::string> accum;
  std::string temp;
  std::locale loc;
  size_t index = 0;
  do
  {
    while ((isspace(src[index], loc) || src[index] == ',') && src[index] != '\n')
    {
      ++index;
      if (temp.size() > 0)
      {
        accum.push_back(temp);
        temp.clear();
      }
    }
    if(src[index] != '\0')
      temp += src[index];
  } while (index++ < src.size());
  if(temp.size() > 0)
    accum.push_back(temp);
  return accum;
}

//----< remove leading and trailing whitespace >---------------------

std::string StringHelper::trim(const std::string& src)
{
	std::locale loc;
	std::string trimmed = src;
	size_t first = 0;
	while (true)
	{
		if (std::isspace(trimmed[first], loc))
			++first;
		else
			break;
	}
	size_t last = trimmed.size() - 1;
	while (true)
	{
		if (std::isspace(trimmed[last], loc) && last > 0)
			--last;
		else
			break;

	}
	return trimmed.substr(first, last - first + 1);
}

std::string StringHelper::addHeaderAndFooterLines(const std::string& src)
{
	std::string line = "------------------------------";
	return line + "\n" + src + "\n" + line + "\n";
}

//----< takes any pointer type and displays as a dec string >--------

std::string Utilities::ToDecAddressString(size_t address)
{
	std::ostringstream oss;
	oss << std::uppercase << std::dec << address;
	return oss.str();
}
//----< takes any pointer type and displays as a hex string >--------

std::string Utilities::ToHexAddressString(size_t address)
{
	std::ostringstream oss;
	oss << std::uppercase << " (0x" << std::hex << address << ")";
	return oss.str();
}

//----< write newline to console >-----------------------------------
void Utilities::putline()
{
  std::cout << "\n";
}


#ifdef TEST_UTILITIES

using Utils = Utilities::StringHelper;

int main()
{
  Utils::Title("Testing Utilities Package");
  putline();

  Utils::title("test StringHelper::split(std::string)");

  std::string test = "a, \n, bc, de, efg, i, j k lm nopq rst";
  std::cout << "\n  test string = " << test;
  
  std::vector<std::string> result = StringHelper::split(test);
  
  std::cout << "\n";
  for (auto item : result)
  {
    if (item == "\n")
      std::cout << "\n  " << "newline";
    else
      std::cout << "\n  " << item;
  }
  std::cout << "\n";

  Utils::title("test std::string Converter<T>::toString(T)");

  std::string conv1 = Converter<double>::toString(3.1415927);
  std::string conv2 = Converter<int>::toString(73);
  std::string conv3 = Converter<std::string>::toString("a_test_string plus more");

  std::cout << "\n  Converting from values to strings: ";
  std::cout << conv1 << ", " << conv2 << ", " << conv3;
  putline();

  Utils::title("test T Converter<T>::toValue(std::string)");

  std::cout << "\n  Converting from strings to values: ";
  std::cout << Converter<double>::toValue(conv1) << ", ";
  std::cout << Converter<int>::toValue(conv2) << ", ";
  std::cout << Converter<std::string>::toValue(conv3);

  std::cout << "\n\n";
  return 0;
}
#endif
