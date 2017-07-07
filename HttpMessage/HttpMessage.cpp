/////////////////////////////////////////////////////////////////////
// HttpMessage.cpp - Build and parse Http style messages           //
//                                                                 //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016       //
/////////////////////////////////////////////////////////////////////

#include "HttpMessage.h"
#include "../Utilities/Utilities.h"
#include <iostream>

using namespace Utilities;
using Attribute = HttpMessage::Attribute;
using Attributes = HttpMessage::Attributes;
using Name = HttpMessage::Name;
using Value = HttpMessage::Value;
using Body = HttpMessage::Body;
using byte = HttpMessage::byte;

class MockSocket
{
public:
	MockSocket(const HttpMessage& msg) : msgString_(msg.toString()) {};
	std::string recvString();
	bool recv(size_t bytes, byte* buffer);
	size_t& setContent_length() { return content_length_; }
private:
	std::string msgString_;
	size_t pos_ = 0;
	size_t content_length_ = 0;
};

std::string MockSocket::recvString()
{
	std::string line;
	char ch;
	while ((ch = msgString_[pos_++]) != '\n')
	{
		line += ch;
	}
	return line;
}

bool MockSocket::recv(size_t bytes, byte* buffer)
{
	for (size_t i = 0; i < bytes; ++i)
	{
		if (pos_ + i == msgString_.size())
		{
			std::cout << "\n---- attempt to index past end of msgString_ ----\n";
			return false;
		}
		buffer[i] = msgString_[pos_ + i];
	}
	return true;
}
//----< add attribute pair to attribute collection >-----------------

void HttpMessage::addAttribute(const Attribute& attrib)
{
	attributes_.push_back(attrib);
}
//----< find attribute by name >-------------------------------------

size_t HttpMessage::findAttribute(const Name& name)
{
	for (size_t i = 0; i < attributes_.size(); ++i)
		if (attributes_[i].first == name)
			return i;
	return attributes_.size();
}
//----< find value of attribute with specified name >----------------
/*
*  returns empty string if name not found
*/
Value HttpMessage::findValue(const Name& name)
{
	Value val = "";
	size_t pos = findAttribute(name);
	if (pos < attributes_.size())
	{
		val = attributes_[pos].second;
	}
	return val;
}
//----< remove attribute by name >-----------------------------------

bool HttpMessage::removeAttribute(const Name& name)
{
	size_t pos = findAttribute(name);
	if (pos == attributes_.size())
		return false;
	std::vector<Attribute>::iterator iter = attributes_.begin() + pos;
	attributes_.erase(iter);
	return true;
}
//----< retrieve attribute collection >------------------------------

Attributes& HttpMessage::attributes()
{
	return attributes_;
}
Attribute HttpMessage::parseAttribute(const std::string& src)
{
	Attribute attrib("", "");
	size_t pos = src.find(':');
	if (pos == std::string::npos)
		return attrib;
	std::string first = StringHelper::trim(src.substr(0, pos));
	std::string second = StringHelper::trim(src.substr(pos + 1, src.size() - 1));
	attrib.first = first;
	attrib.second = second;
	return attrib;
}

//----< fill body from buffer >--------------------------------------

void HttpMessage::setBody(byte buffer[], size_t Buflen)
{
	for (size_t i = 0; i < Buflen; ++i)
	{
		body_.push_back(buffer[i]);
	}
}
//----< fill buffer from body >--------------------------------------
/*
*  - returns length of buffer contents
*  - return value is zero if buffer is too small to hold body
*/
size_t HttpMessage::getBody(byte buffer[], size_t& Buflen)
{
	if (Buflen < body_.size())
		return 0;
	size_t length = body_.size();
	for (size_t i = 0; i < length; ++i)
	{
		buffer[i] = body_[i];
	}
	return length;
}
//----< replace body >-----------------------------------------------

void HttpMessage::addBody(const Body& body)
{
	body_ = body;
}
//----< replace body with contents of string >-----------------------

void HttpMessage::addBody(const std::string& body)
{
	body_.clear();
	for (auto ch : body)
		body_.push_back(ch);
}
//----< replace body from buffer contents >--------------------------

void HttpMessage::addBody(size_t numBytes, byte* pBuffer)
{
	for (size_t i = 0; i < numBytes; ++i)
		body_.push_back(*(pBuffer + i));
}

//-----< retrieve body >---------------------------------------------

Body& HttpMessage::body()
{
	return body_;
}
//----< return length of body in bytes >-----------------------------

size_t HttpMessage::bodyLength()
{
	return body_.size() * sizeof(byte);  // may change byte to int
}
//----< convert attribute pair to string >---------------------------

std::string HttpMessage::attribString(const Attribute& attrib)
{
	return attrib.first + ":" + attrib.second + '\n';
}
//----< build attribute from name and value >------------------------

Attribute HttpMessage::attribute(const Name& name, const Value& value)
{
	return std::pair<Name, Value>(name, value);
}
//----< convert message header to string >---------------------------

std::string HttpMessage::headerString() const
{
	std::string header;
	for (auto attrib : attributes_)
	{
		header += attribString(attrib);
	}
	header += term_;
	return header;
}
//----< convert message header to indented string >------------------

std::string HttpMessage::toIndentedHeaderString() const
{
	std::string header = "  ";
	for (auto attrib : attributes_)
	{
		header += attribString(attrib) + "  ";
	}
	header += term_;
	return header;
}
//----< convert body to string >-------------------------------------

std::string HttpMessage::bodyString() const
{
	std::string body;
	for (char ch : body_)
		body += ch;
	return body;
}
//----< convert body to indented string >----------------------------

std::string HttpMessage::toIndentedBodyString() const
{
	std::string body = "  ";
	for (char ch : body_)
		body += ch;
	return body;
}
//----< convert message to string >----------------------------------

std::string HttpMessage::toString() const
{
	return headerString() + bodyString();
}
//----< convert message to indented string >-------------------------

std::string HttpMessage::toIndentedString() const
{
	return toIndentedHeaderString() + toIndentedBodyString();
}
//----< remove all message contents >--------------------------------

void HttpMessage::clear()
{
	attributes_.clear();
	body_.clear();
}
//----< fill buffer with char >--------------------------------------

void HttpMessage::fillBuffer(byte buffer[], size_t BufSize, byte fill)
{
	for (size_t i = 0; i < BufSize; ++i)
		buffer[i] = fill;
}

using Utils = StringHelper;

#ifdef TEST_HTTPMESSAGE

int main()
{
	Utils::Title("Testing HttpMessage class", '=');

	HttpMessage msg;
	msg.addAttribute(HttpMessage::attribute("Command", "GetFiles"));
	msg.addAttribute(HttpMessage::attribute("ToAddr", "127.0.0.1:8080"));
	msg.addAttribute(HttpMessage::attribute("FromAddr", "127.0.0.1:8081"));
	msg.addAttribute(HttpMessage::attribute("Mode", "OneWay"));
	msg.addAttribute(HttpMessage::attribute("content_length", "10"));

	msg.addBody(std::vector<HttpMessage::byte> { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });

	std::cout << "\n" << Utils::addHeaderAndFooterLines(msg.toString());

	Utils::title("testing headerString(), bodyString(), and toString()");
	std::cout << "\nheader string:";
	std::cout << "\n" << msg.headerString();
	std::cout << "\nbody string:";
	std::cout << "\n" << msg.bodyString();
	std::cout << "\n\nmessage string:";
	std::cout << "\n" << StringHelper::addHeaderAndFooterLines(msg.toString());

	Utils::title("Testing removeAttribute");
	putline();
	msg.removeAttribute("content_length");
	std::cout << msg.toString();
	putline();

	Utils::title("Testing addBody(const std::string&)");
	std::string msgBody = "<msg>this is a message</msg>";
	msg.addAttribute(HttpMessage::Attribute("content_length", Converter<size_t>::toString(msgBody.size())));
	msg.addBody(msgBody);
	std::cout << "\n" << StringHelper::addHeaderAndFooterLines(msg.toString());

	Utils::title("Testing parseAttribute(const std::string&)");
	std::string test2 = "name:value";
	std::cout << "\n  input = \"" << test2 << "\"";
	Attribute attrib2 = HttpMessage::parseAttribute(test2);
	std::cout << "\n  result is: " << HttpMessage::attribString(attrib2);

	test2 = " name : value ";
	std::cout << "\n  input = \"" << test2 << "\"";
	attrib2 = HttpMessage::parseAttribute(test2);
	std::cout << "\n  result is: " << HttpMessage::attribString(attrib2);

	Utils::title("Testing Message parsing");
	MockSocket sock(msg);
	HttpMessage msg2;
	while (true)
	{
		std::string line = sock.recvString();
		if (line.size() == 0)
			break;
		Attribute attrib = HttpMessage::parseAttribute(line);
		msg2.addAttribute(attrib);
	}
	Value val = msg2.findValue("content_length");
	if (val.size() > 0)
	{
		size_t numBytes = Converter<size_t>::toValue(val);
		byte* pBuffer = new byte[numBytes];
		sock.recv(numBytes, pBuffer);
		msg2.addBody(numBytes, pBuffer);
	}
	std::cout << "\n" << Utils::addHeaderAndFooterLines(msg2.toString());
	std::cout << "\n\n";
}
#endif
