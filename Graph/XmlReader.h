#ifndef XMLREADER_H
#define XMLREADER_H
///////////////////////////////////////////////////////////////////////
// XmlReader.h - Parse XML strings                                   //
// ver 2.2                                                           //
// Language:    Visual C++, Visual Studio 2010, SP1                  //
// platform:    Toshiba Portege R705, Windows 7, Home Premium, SP1   //
// Application: Summer 2011 Projects                                 //
// Author:      Jim Fawcett, Syracuse University, CST 4-187          //
//              www.lcs.syr.edu/faculty/fawcett, (315) 443-3948      //
///////////////////////////////////////////////////////////////////////
/*
* Note: XmlReader and XmlWriter supercede both XmlTran and XmlParser.
*
* Package Operations:
* ===================
* This package contains a single class, XmlReader.  It provides a
* simple interface to step from XML element to element and extract
* constituent parts, e.g., tag, attributes, and body.  The package
* does not support editing XML strings.  For that, you should use
* the XmlDocument package.
*
* Parsing honors element scope, e.g., if there are two elements with
* the same tag names at different levels of the document scope, the
* XmlReader will not get confused and prematurely close an element
* because it found a matching closing tag for a contained element.
*
* Exceptions will be thrown by element(), body(), and attributes()
* if malformed XML is detected.
*
* Required Files:
* ===============
* XmlReader.h, XmlReader.cpp
*
* Build Process:
* ==============
* cl /Eha XmlReader.cpp
*
* Maintenance History:
* ====================
* ver 2.2 : 15 Feb 12
* - fixed bug found by Yateen Nikharge in element(). Search for
*   element end was not selective enough.
* - previously added member functions position() and position(size_t p)
*   to enable measuring and changing internal string index - used in
*   creating Visual Studio project.
* ver 2.1 : 25 Jul 11
* - added comments to manual page, above, and implementations in
*   XmlReader.cpp
* ver 2.0 : 27 Jun 11
* - moved #include<stack> to XmlReader.cpp
* - moved #include<iostream> to test stub in XmlReader.cpp
* ver 1.0 : 26 Jun 11
* - first release
*/
#include <vector>

class XmlReader
{
public:
	typedef std::vector<std::pair<std::string, std::string>> attribElems;
	XmlReader(const std::string& xml);
	bool next();
	std::string tag();
	attribElems attributes();
	std::string body();
	std::string element();
	std::string extractIdentifier(size_t& pos);
	void reset();
	size_t position();
	bool position(size_t p);
	std::string& xml();
private:
	// source string
	std::string _xml;
	size_t _position;
	// current element parts
	std::string _tag;
	attribElems _attributes;
	size_t localposition;
};

inline void XmlReader::reset() { _position = 0; }

inline size_t XmlReader::position() { return _position; }

inline std::string& XmlReader::xml() { return _xml; }

#endif
