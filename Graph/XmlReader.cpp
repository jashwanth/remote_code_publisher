///////////////////////////////////////////////////////////////////////
// XmlReader.cpp - Parse XML strings                                 //
// ver 2.2                                                           //
// Language:    Visual C++, Visual Studio 2010, SP1                  //
// platform:    Toshiba Portege R705, Windows 7, Home Premium, SP1   //
// Application: Summer 2011 Projects                                 //
// Author:      Jim Fawcett, Syracuse University, CST 4-187          //
//              www.lcs.syr.edu/faculty/fawcett, (315) 443-3948      //
///////////////////////////////////////////////////////////////////////

#include "XmlReader.h"
#include <ctype.h>
#include <stack>
#include <algorithm>

XmlReader::XmlReader(const std::string& xml)
	: _xml(xml), _position(0), localposition(0) {}

//----< helper identifies markup chars >-----------------------------

bool specialChar(char ch)
{
	bool test = isspace(ch) || ch == '/' || ch == '>' || ch == '<' || ch == '=';
	test = test || ch == '\'' || ch == '"';
	return test;
}
//----< helper finds identifiers >-----------------------------------

std::string XmlReader::extractIdentifier(size_t& pos)
{
	std::string ident;
	while (true)
	{
		if (pos == _xml.size())
			return ident;
		char ch = _xml[pos];
		if (specialChar(ch))
			++pos;
		else
			break;
	}
	while (true)
	{
		char ch = _xml[pos];
		if (specialChar(ch))
			break;
		ident += ch;
		++pos;
	}
	return ident;
}
//----< return current element string >------------------------------

std::string XmlReader::element()
{
	// find tag - assumes _position points to character after
	// opening "<" on entry
	localposition = _position;
	_tag = extractIdentifier(localposition);

	// is declaration?
	std::string decTag = "?xml";
	if (_tag.compare("?xml") == 0)
	{
		size_t locpos = _xml.find("?>");
		return _xml.substr(_position - 1, locpos - _position + 3);
	}

	// is comment?
	if (_tag.compare("!--") == 0)
	{
		size_t locpos = _xml.find("-->");
		return _xml.substr(_position - 1, locpos - _position + 4);
	}

	// find end of element </tag>
	size_t locpos1 = localposition;

	// note: tracks element scope with tagStack, so will correctly
	// return nested elements with the same tag name
	std::stack<std::string> tagStack;
	tagStack.push(_tag);
	while (true)
	{
		locpos1 = _xml.find(_tag, locpos1);
		if (locpos1 >= _xml.size())
			break;
		if (_xml[locpos1 - 1] == '/' && _xml[locpos1 + _tag.size()] == '>')  // modified 2/15/2012
			tagStack.pop();
		else if (_xml[locpos1 - 1] == '<')
			tagStack.push(_tag);
		if (tagStack.size() == 0)
			break;
		++locpos1;
	}

	// find end element of self-closing tag, e.g., <tag />
	size_t locpos2 = _xml.find(">", localposition);
	if (_xml[locpos2 - 1] != '/')
		locpos2 = _xml.size();

	// find end element </tag>
	localposition = std::min(locpos1, locpos2);
	if (localposition >= _xml.size())
		throw std::exception("malformed XML");
	if (localposition == locpos1)
	{
		localposition = _xml.find('>', localposition);
		return _xml.substr(_position - 1, localposition - _position + 2);
	}
	return _xml.substr(_position - 1, localposition - _position + 2);
}
//----< return body string >-----------------------------------------

std::string XmlReader::body()
{
	if (_tag.compare("?xml") == 0 || _tag.compare("!--") == 0)
	{
		return "";
	}
	std::string elem = element();
	size_t locpos1 = elem.find('>');
	if (locpos1 >= elem.size())
		throw std::exception("malformed XML");
	if (elem[locpos1 - 1] == '/')
		return "";
	std::string localtag = tag();
	size_t locpos2 = elem.find_last_of("</");
	if (locpos2 < elem.size())
		return elem.substr(locpos1 + 1, locpos2 - locpos1 - 2);
	return "";
}
//----< move to next XML tag >---------------------------------------

bool XmlReader::next()
{
	while (true)
	{
		_position = _xml.find('<', _position);
		if (_position >= _xml.size())
			return false;
		++_position;
		if (_xml[_position] != '/')
			break;
	}
	return true;
}
//----< return tag string >------------------------------------------

std::string XmlReader::tag()
{
	localposition = _position;
	return extractIdentifier(localposition);
}
//----< return vector of attributes >--------------------------------

XmlReader::attribElems XmlReader::attributes()
{
	_attributes.clear();
	if (_tag.compare("?xml") == 0 || _tag.compare("!--") == 0)
		return _attributes;
	localposition = _position;
	// move past tag
	extractIdentifier(localposition);

	// find attributes
	size_t locpos = _xml.find('>', localposition);
	while (true)
	{
		std::string name = extractIdentifier(localposition);
		if (locpos < localposition)
			return _attributes;
		std::string value = extractIdentifier(localposition);
		if (locpos < localposition)
			throw std::exception("malformed XML");
		std::pair<std::string, std::string> elem;
		elem.first = name;
		elem.second = value;
		_attributes.push_back(elem);
	}
	return _attributes;
}
//----< reposition string index >------------------------------------

bool XmlReader::position(size_t p)
{
	if (p < 0 || _xml.size() <= p)
		return false;
	_position = p;
	return true;
}
//----< test stub >--------------------------------------------------

#ifdef TEST_XMLREADER
#include <iostream>

int main()
{
	std::cout << "\n  Testing XmlReader";
	std::cout << "\n ===================\n";

	std::string Prologue = "<?xml version=\"1.0\"?><!-- top level comment -->";
	std::string test1 = Prologue;
	test1 += "<parent att1='val1' att2='val2'><child1 /><child2>child2 body<child1 /></child2></parent>";
	std::string test2 = Prologue;
	test2 += "<Dir><path>/TestFile/Dir1</path><File><name>text1.txt</name><date>02/11/2012</date></File></Dir>";
	XmlReader rdr(test1);

	std::cout << "\n  source = " << test1.c_str() << std::endl;

	// testing helper function

	std::cout << "\n  Testing extractIdentifier";
	std::cout << "\n ---------------------------";
	std::string ident;
	size_t position = 1;
	do
	{
		ident = rdr.extractIdentifier(position);
		std::cout << "\n  " << ident.c_str();
	} while (ident.size() > 0);

	// testing typical use, step through each element extracting tags, attributes,
	// bodies, and or entire element strings

	std::cout << "\n  testing next(), tag(), element(), body(), and attributes()";
	std::cout << "\n -----------------------------------------------------------";
	rdr.reset();
	while (rdr.next())
	{
		std::cout << "\n  tag:     " << rdr.tag().c_str();
		std::cout << "\n  element: " << rdr.element().c_str();
		std::cout << "\n  body:    " << rdr.body().c_str();
		XmlReader::attribElems attribs = rdr.attributes();
		for (size_t i = 0; i < attribs.size(); ++i)
			std::cout << "\n  attributes: " << attribs[i].first.c_str() << ", " << attribs[i].second.c_str();
	}
	std::cout << "\n\n";

	// testing typical use, step through each element extracting tags, attributes,
	// bodies, and or entire element strings

	rdr.xml() = test2;
	std::cout << "\n  source = " << rdr.xml().c_str() << std::endl;

	std::cout << "\n  testing next(), tag(), element(), body(), and attributes()";
	std::cout << "\n -----------------------------------------------------------";

	rdr.reset();
	while (rdr.next())
	{
		std::cout << "\n  tag:     " << rdr.tag().c_str();
		std::cout << "\n  element: " << rdr.element().c_str();
		std::cout << "\n  body:    " << rdr.body().c_str();
		XmlReader::attribElems attribs = rdr.attributes();
		for (size_t i = 0; i<attribs.size(); ++i)
			std::cout << "\n  attributes: " << attribs[i].first.c_str() << ", " << attribs[i].second.c_str();
	}
	std::cout << "\n\n";
	return 0;
}
#endif

