#ifndef XMLWRITER_H
#define XMLWRITER_H
///////////////////////////////////////////////////////////////////////
// XmlWriter.h - Create XML Strings                                  //
// ver 4                                                             //
// Language:    Visual C++, Visual Studio 2010, SP1                  //
// platform:    Toshiba Portege R705, Windows 7, Home Premium, SP1   //
// Application: Summer 2011 Projects                                 //
// Author:      Jim Fawcett, Syracuse University, CST 4-187          //
//              www.lcs.syr.edu/faculty/fawcett, (315) 443-3948      //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* ===================
* This package contains a single class, XmlWriter.  It provides a
* simple interface to build XML strings.  The package does not
* support editing XML strings.  For that, you can write a graph
* editor - fairly easy to do with the support of graph - reading
* the original from an XML file with XmlReader, modifying with the
* editor, and saving using this XmlWriter.  You can also use the
* XmlDocument package.
*
* XmlWriter does not keep track of the current XML element scope.
* You have to do that.  Essentially when creating an element you
* must build its body before closing.  You can do that by creating
* a new instance of XmlWriter for the body part.  See the test
* stub for an illustration of how to do that.
*
* XmlReader and XmlWriter supercede both XmlTran and XmlParser.
*
* Required Files:
* ===============
* XmlWriter.h, XmlWriter.cpp
*
* Build Process:
* ==============
* cl /Eha XmlWriter.cpp
*
* Maintenance History:
* ====================
* ver 4 : 11 Feb 13
* - added tag stack so writer is tag scope aware
* - added indenting option
* ver 4 : 04 Jul 11
* - added composite operations by returning XmlWriter& to
*   support chaining methods.  Also added element(tag,body).
* ver 3 : 02 Jul 11
* - made cosmetic changes to the manual page information
* ver 2 : 27 Jun 11
* - added verbose output to illustrate how XmlWriter works
* ver 1 : 26 Jun 11
* - first release
*/
#include <string>
#include <stack>

class XmlWriter
{
public:
	typedef std::pair<std::string, std::string> attribElem;
	XmlWriter();
	XmlWriter& start(const std::string& tag);
	XmlWriter& addBody(const std::string& body);
	XmlWriter& addAttribute(const std::string& name, const std::string& value);
	XmlWriter& end();
	XmlWriter& element(const std::string& tag, const std::string& body);
	XmlWriter& addDeclaration();
	XmlWriter& addComment(const std::string& comment);
	std::string xml();
	XmlWriter& clear();
	XmlWriter& indent(bool ind = true);
private:
	void indentIt();
	bool _indented;
	size_t _level;
	std::string _tag;
	std::string _xml;
	size_t position;
	std::stack<std::string> tagStack;
};

inline XmlWriter::XmlWriter() : _indented(false), _level(0) {}

inline std::string XmlWriter::xml() { return _xml; }

inline XmlWriter& XmlWriter::clear()
{
	_xml = _tag = "";
	position = 0;
	return *this;
}

inline XmlWriter& XmlWriter::indent(bool ind) { _indented = ind; return *this; }

#endif
