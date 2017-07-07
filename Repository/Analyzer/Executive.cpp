/////////////////////////////////////////////////////////////////////
// Executive.cpp - Demonstrate Project2 Requirements               //
// ver 1.4                                                         //
//-----------------------------------------------------------------//
// Jim Fawcett (c) copyright 2016                                  //
// All rights granted provided this copyright notice is retained   //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Platform:    Dell XPS 8900, Windows 10                          //
// Application: Project #2, CSE687 - Object Oriented Design, S2015 //
// Author:      Jashwanth Reddy, Syracuse University, CST 4-187    //
//              jgangula@syr.edu                                   //
/////////////////////////////////////////////////////////////////////

#include "Executive.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <exception>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <windows.h>
#include <shellapi.h>
#include "../Parser/Parser.h"
#include "../FileSystem/FileSystem.h"
#include "../FileMgr/FileMgr.h"
#include "../Parser/ActionsAndRules.h"
#include "../Parser/ConfigureParser.h"
#include "../AbstractSyntaxTree/AbstrSynTree.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../DepthAnalysis/TypeAnalysis.h"
#include "../DependencyAnalysis/DependAnal.h"
#include "../Graph/Graph.h"
#include "../PublishCode/PublishCode.h"
using Rslt = Logging::StaticLogger<0>;  // use for application results
using Demo = Logging::StaticLogger<1>;  // use for demonstrations of processing
using Dbug = Logging::StaticLogger<2>;  // use for debug output
using namespace Utilities;
//using Rslt = Logging::StaticLogger<1>;
using namespace Async;
using namespace CodeAnalysis;
using namespace std;

/////////////////////////////////////////////////////////////////////
// AnalFileMgr class
// - Derives from FileMgr to make application specific file handler
//   by overriding FileMgr::file(), FileMgr::dir(), and FileMgr::done()

using Path = std::string;
using File = std::string;
using Files = std::vector<File>;
using Pattern = std::string;
using Ext = std::string;
using FileMap = std::unordered_map<Pattern, Files>;



class ClientHandler
{
public:
	using Path = std::string;
	using Pattern = std::string;
	using Patterns = std::vector<Pattern>;
	using File = std::string;
	using Files = std::vector<File>;
	using Ext = std::string;
	using Options = std::vector<char>;
	using FileMap = std::unordered_map<Pattern, Files>;
	using Slocs = size_t;
	using SlocMap = std::unordered_map<File, Slocs>;
public:
	ClientHandler(BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);
	bool ProcessCommandLine(int argc, char* argv[]);
	HttpMessage readMessageForPost(HttpMessage& msg, Socket& socket);
private:
	bool connectionClosed_;
	HttpMessage readMessage(Socket& socket);
	bool readFile(const std::string& filename, size_t fileSize, const std::string& getDir, Socket& socket);
	BlockingQueue<HttpMessage>& msgQ_;
	Path path_;
};

/* This method is only used by runCodeAnalyzerOnServer to reduce the line count to 50 */
void OpenBrowserPagesToDemonstarteReq11(int argc, char * argv[]) {
	Utilities::StringHelper::Title("Demonstarting Requirement 11 is met: ");
	std::string path = argv[6];
	path += "/dependencyTable.htm";
	path = FileSystem::Path::getFullFileSpec(path);
	std::wstring sPath(path.begin(), path.end());
	Utilities::StringHelper::Title("Trying to open the dependency Files in the local folder");
	ShellExecute(NULL, L"open", sPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
	std::string path1 = argv[8];
	path1 += "/dependencyTable.htm";
	std::wstring sPath1(path1.begin(), path1.end());
	Utilities::StringHelper::Title("Trying to open the dependency Files on the IIS virtula Directory folder");
	ShellExecute(NULL, L"open", sPath1.c_str(), NULL, NULL, SW_SHOWNORMAL);
	Utilities::StringHelper::Title("End of Requirement 11 Demonstation");
}

int runCodeAnalyzerOnServer(int argc, char* argv[]) {
	using namespace CodeAnalysis;
	CodeAnalysisExecutive exec;
	try {
		bool succeeded = exec.ProcessCommandLine(argc, argv);
		exec.setDisplayModes();
		exec.startLogger(std::cout);
		std::ostringstream tOut("CodeAnalysis");
		Utils::sTitle(tOut.str(), 3, 92, tOut, '=');
		Rslt::write(tOut.str());
		Rslt::write("\n     " + exec.getAnalysisPath());
		Rslt::write("\n     " + exec.systemTime());
		Rslt::flush();
		exec.showCommandLineArguments(argc, argv);
		Rslt::write("\n");
		exec.getSourceFiles();
		exec.processSourceCode(true);
		std::ostringstream out;
		out << "\n  " << std::setw(10) << "searched" << std::setw(6) << exec.numDirs() << " dirs";
		out << "\n  " << std::setw(10) << "processed" << std::setw(6) << exec.numFiles() << " files";
		Rslt::write(out.str());
		exec.stopLogger();
		putline();
		TypeAnal ta;
		ta.doTypeAnal();
		ta.displayTypeTable();
		DependAnal dan;
		dan.ProcessCommandLine(argc, argv);
		dan.getSourceFiles();
		dan.processSourceCode();
		CodeAnalysisExecutive::depmapforlazyloading = dan.getDependencyTable();
		Utilities::StringHelper::Title("Demonstarting Requirement 4 is met: Start of Code Publisher of Project 3 ");
		CodePublisher cp(dan.getDependencyTable());
		cp.ProcessCommandLine(argc, argv);
		cp.getSourceFiles();
		cp.displayDependencies();
		cp.demonstrateReq4();
		cp.processSourceCode();
		Utilities::StringHelper::Title("End of Requirement 4:  Code Publisher has published all the source code to HTML content and stylings included");
		OpenBrowserPagesToDemonstarteReq11(argc, argv);
	}
	catch (std::exception& except)
	{
		exec.flushLogger();
		std::cout << "\n\n  caught exception in Executive::main: " + std::string(except.what()) + "\n\n";
		exec.stopLogger();
		return 1;
	}
	return 0;
}

//----<this defines the processing of Get Requests and send the reply to the client >----------

bool sendFinalMessageToClient(HttpMessage &msg)
{
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8081))
		{
			Rslt::write("\n Server waiting to connect");
			::Sleep(100);
		}
	//	sendFile(filepath, si);
		std::string sendString = msg.toString();
		si.send(sendString.size(), (Socket::byte*)sendString.c_str());
		Rslt::write("\n Server has sent the get reply to the client.");
	} 
	catch (std::exception& exc)
	{
		Rslt::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Rslt::write(exMsg);
	}
	return true;
}


bool sendFileToClient(string path,string FileName, int argc, char* argv[]) {
	try {
		HttpMessage sendMsg;
		std::string SrvrEndPoint = "localhost::8080";
		std::string ClientEndPoint = "localhost::8081";
		sendMsg.addAttribute(HttpMessage::attribute("GET", "Message"));
		sendMsg.addAttribute(HttpMessage::Attribute("mode", "oneWay"));
		sendMsg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ClientEndPoint));
		sendMsg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + SrvrEndPoint));
		string FileHtmPath = path;
		FileHtmPath += '/' + FileName;
		Rslt::write("\n Html FilePath to be uploaded to client is :" + FileHtmPath);
		FileSystem::File file(FileHtmPath);
		file.open(FileSystem::File::in, FileSystem::File::binary);
		if (!file.isGood())
		{	Rslt::write("\n\n  can't open file " + FileHtmPath);
			return false;}
		FileSystem::FileInfo fi(FileHtmPath);
		size_t fileSize = fi.size();
		std::string sizeString = Converter<size_t>::toString(fileSize);
		SocketSystem ss;SocketConnecter si;
		while (!si.connect("localhost", 8081))
		{	Rslt::write("\n Server waiting to connect");
			::Sleep(100);}
		sendMsg.addAttribute(HttpMessage::Attribute("messageType", "HtmFileContentReplyFromServer"));
		sendMsg.addAttribute(HttpMessage::Attribute("fileName", FileName));
		sendMsg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
		std::string sendString = sendMsg.toString();
		si.send(sendString.size(), (Socket::byte*)sendString.c_str());
		/* Now send the file content */
		const size_t BlockSize = 2048;
		Socket::byte buffer[BlockSize];
		while (true)
		{	FileSystem::Block blk = file.getBlock(BlockSize);
			if (blk.size() == 0)
				break;
			for (size_t i = 0; i < blk.size(); ++i)
				buffer[i] = blk[i];
			si.send(blk.size(), buffer);
			if (!file.isGood())
				break;}
		file.close();
		cout << "Requirement 8 is met: Sent the file : " << FileHtmPath << " through stream of 2048 bytes each ";
		Rslt::write("\n\n File : " + FileHtmPath + " content was sent to the client successfully");
	}
	catch (std::exception& exc)
	{	Rslt::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Rslt::write(exMsg);}
	return true;}

bool SendDependencyFilesToClient(string FileName,int argc,char* argv[]) {
	Utilities::StringHelper::Title("Demonstrating Requirement :Lazy DownLoad Strategy");
	try {
		if (CodeAnalysisExecutive::depmapforlazyloading.size() <= 1) {
			runCodeAnalyzerOnServer(argc, argv);
		}
		unordered_map<std::string, std::set<std::string>>::iterator it = CodeAnalysisExecutive::depmapforlazyloading.begin();
		for (it; it != CodeAnalysisExecutive::depmapforlazyloading.end(); it++) {
			string fileStringFromItr = it->first;
			std::string fileName_ = fileStringFromItr.substr(fileStringFromItr.find_last_of("\\") + 1);
			if (fileName_.compare(FileName) == 0) {
				set<string> valueOfMap = it->second;

				for (std::string str : it->second)
				{
					cout << "File :" << FileName << "depends on " << str << endl;
					std::string dependFileNameOnly_ = str.substr(str.find_last_of("\\") + 1);
					cout << "Trying to send the dependency file " << dependFileNameOnly_ + ".htm" << "to the client " << endl;
					string FileHtmPath = argv[6];
					sendFileToClient(FileHtmPath,dependFileNameOnly_ + ".htm", argc, argv);
				}
			}
		}
		Utilities::StringHelper::Title("Requirement 10 is met: Sent the depedency files to the client for Lazy DownLoad Strategy");
	}
	catch (std::exception& exc)
	{
		Rslt::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Rslt::write(exMsg);
	}
	return true;
}

void createDefaultSendMessageWithAttributes(HttpMessage &msg) {

}
void processGetMessageforGetCategories(HttpMessage &msg, int argc, char* argv[]) {

	string sendResult = "";
	HttpMessage sndMsg;
	std::string SrvrEndPoint = "localhost::8080";
	std::string ClientEndPoint = "localhost::8081";
	sndMsg.addAttribute(HttpMessage::attribute("GET", "Message"));
	sndMsg.addAttribute(HttpMessage::Attribute("mode", "oneWay"));
	sndMsg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ClientEndPoint));
	sndMsg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + SrvrEndPoint));
	Rslt::write("\nServer needs to process the GetCategories message: \n");
	vector<std::string> getAllCategoriesList = FileSystem::Directory::getDirectories(argv[1]);
	//for (vector<std::string>::iterator it = getAllCategoriesList.begin(); it != getAllCategoriesList.end(); it++)
	for (int i = 0; i <getAllCategoriesList.size(); i++)
	{
		//	cout << "Directory found " << *it << " \n";
		if ((getAllCategoriesList[i] != ".") && (getAllCategoriesList[i] != ".."))
		{
			sendResult += getAllCategoriesList[i];
			if (i != getAllCategoriesList.size() - 1)
			{
				sendResult += ",";
			}
		}
	}
	sndMsg.addAttribute(HttpMessage::Attribute("messageType", "getCategoriesReply"));
	if (sendResult.size() > 0)
	{
		sndMsg.addAttribute(HttpMessage::attribute("content-length", Converter<size_t>::toString(sendResult.size())));
		sndMsg.addBody(sendResult); /* We add message body after all atributes are added for client
									to get the body based on content-length attribute */
	}
	getAllCategoriesList.clear();
	Utilities::StringHelper::Title("\n Requirement 8 is met: The HTTP response to be sent from the server end " + sndMsg.toString());
	Rslt::write("\nMessage to be sent is " + sndMsg.toString());
	sendFinalMessageToClient(sndMsg);
}


void processGetMessageForGetFileNamesFromServer(HttpMessage &msg, int argc, char* argv[]) {
	string sendResult = "";
	HttpMessage sndMsg;
	std::string SrvrEndPoint = "localhost::8080";
	std::string ClientEndPoint = "localhost::8081";
	sndMsg.addAttribute(HttpMessage::attribute("GET", "Message"));
	sndMsg.addAttribute(HttpMessage::Attribute("mode", "oneWay"));
	sndMsg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ClientEndPoint));
	sndMsg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + SrvrEndPoint));
	Rslt::write("\nServer needs to process the GetFiles message: \n");
	string searchCategoryPath = argv[1];
	searchCategoryPath += "/" + msg.findValue("FileCategory");
	std::cout << "\n The argv[1] is: " << argv[1] << endl;
	std::cout << "\n The searchCategoryPath is: " << searchCategoryPath << endl;
	vector<std::string> getAllFilesList = FileSystem::Directory::getFiles(searchCategoryPath, "*.*");
	//	for (vector<std::string>::iterator it = getAllFilesList.begin();it != getAllFilesList.end(); it++)
	for (int i = 0; i < getAllFilesList.size(); i++)
	{
		cout << "File found " << getAllFilesList[i] << " \n";
		/*if ((*it != ".") && (*it != ".."))
		{*/
		sendResult += getAllFilesList[i];
		if (i != getAllFilesList.size() - 1)
		{
			sendResult += ",";
		}
		//	}
	}
	sndMsg.addAttribute(HttpMessage::Attribute("messageType", "getFileNamesReply"));
	if (sendResult.size() > 0)
	{
		sndMsg.addAttribute(HttpMessage::attribute("content-length", Converter<size_t>::toString(sendResult.size())));
		sndMsg.addBody(sendResult); /* We add message body after all atributes are added for client
									to get the body based on content-length attribute */
	}
	getAllFilesList.clear();
	Rslt::write("\nMessage to be sent is " + sndMsg.toString());
	sendFinalMessageToClient(sndMsg);
}

void processGetMessageFordownloadHtmFileFromServer(HttpMessage &msg, int argc, char* argv[]) {

	string sendResult = "";
	HttpMessage sndMsg;
	std::string SrvrEndPoint = "localhost::8080";
	std::string ClientEndPoint = "localhost::8081";
	sndMsg.addAttribute(HttpMessage::attribute("GET", "Message"));
	sndMsg.addAttribute(HttpMessage::Attribute("mode", "oneWay"));
	sndMsg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ClientEndPoint));
	sndMsg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + SrvrEndPoint));
	size_t contentSize;
	try {
		std::string sizeString = msg.findValue("content-length");
		if (sizeString != "")
			contentSize = Converter<size_t>::toValue(sizeString);
		if (contentSize > 0) {
			Socket::byte* buffer = new Socket::byte[contentSize + 1];
			msg.getBody(buffer, contentSize);
			buffer[contentSize] = '\0';
			string getMsgBody = buffer;
			delete[] buffer;
			if (getMsgBody.find('$') != std::string::npos) {
				string FirstDollarSplit = getMsgBody.substr(getMsgBody.find('$') + 1);
				if (FirstDollarSplit.find('$') != std::string::npos) {
					string CategoryName = FirstDollarSplit.substr(0, FirstDollarSplit.find('$'));
					string FileName = FirstDollarSplit.substr(FirstDollarSplit.find('$') + 1);
					Rslt::write("\nServer needs to get File HTML content = " + FileName + " Category = " + CategoryName +
						" message request from the client: \n");
					//javascriptPath_ += "../javascript.js";
					//sendFileToClient(javascriptPath_, sndMsg, argc, argv);
					string FileHtmPath = argv[6];
					sendFileToClient(FileHtmPath, FileName + ".htm", argc, argv);
					SendDependencyFilesToClient(FileName, argc, argv);
				}
			}
		}
	}


	catch (std::exception& exc)
	{
		Rslt::write("\n  Exeception caught in the method deleteSingleFileOnServer: \n");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Rslt::write(exMsg);
	}
}


bool processGetMessages(HttpMessage &msg, int argc, char* argv[])
{
	string sendResult = "";
	HttpMessage sndMsg;
	std::string SrvrEndPoint = "localhost::8080";
	std::string ClientEndPoint = "localhost::8081";
	sndMsg.addAttribute(HttpMessage::attribute("GET", "Message"));
	sndMsg.addAttribute(HttpMessage::Attribute("mode", "oneWay"));
	sndMsg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ClientEndPoint));
	sndMsg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + SrvrEndPoint));
	if (msg.findValue("messageType") != "")
	{
		if (msg.findValue("messageType").compare("getCategories") == 0)
		{
			Utilities::StringHelper::Title("Demonstrating Requirement5: Sending the File Categories to the client");
			processGetMessageforGetCategories(msg, argc, argv);
		}
		else if (msg.findValue("messageType").compare("getFileNamesFromServer") == 0)
		{
			Utilities::StringHelper::Title("Demonstrating Requirement5: Sending the File Names with in category to the client");
			processGetMessageForGetFileNamesFromServer(msg,argc,argv);
		}
		else if (msg.findValue("messageType").compare("downloadHtmFileFromServer") == 0) {
			Utilities::StringHelper::Title("Demonstrating Requirement5: Sending the Html and dependent files to the client");
			processGetMessageFordownloadHtmFileFromServer(msg, argc, argv);
		}
		else if (msg.findValue("messageType").compare("downloadJsAndCssFromServer") == 0) {
			cout << "\n Repository::processGetMessages::Request for download js and css files came from Client" << endl;
			string FileHtmPath="../";
			string JsFileName="javascript.js";
			string CSSFileName = "stylesheet.css";
			Utilities::StringHelper::Title("Demonstrating Requirement5: Sending the JSS and CSS files to Client");
			sendFileToClient(FileHtmPath, JsFileName, argc, argv);
			sendFileToClient(FileHtmPath, CSSFileName, argc, argv);
			Utilities::StringHelper::Title("Requirement 5 is met: Sent the js and css files to the client");
		}
	}
	return true;
}



void deleteSingleFileOnServer(HttpMessage &msg, int argc, char* argv[]) {
	Rslt::write("\nServer needs to process the delete message request from the client: \n");
	size_t contentSize = 0;
	HttpMessage replyMsg;
	std::string SrvrEndPoint = "localhost::8080";
	std::string ClientEndPoint = "localhost::8081";
	replyMsg.addAttribute(HttpMessage::attribute("GET", "Message"));
	replyMsg.addAttribute(HttpMessage::Attribute("mode", "oneWay"));
	replyMsg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ClientEndPoint));
	replyMsg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + SrvrEndPoint));
	try {
		std::string sizeString = msg.findValue("content-length");
		if (sizeString != "")
			contentSize = Converter<size_t>::toValue(sizeString);
		if (contentSize > 0) {
			Socket::byte* buffer = new Socket::byte[contentSize + 1];
			msg.getBody(buffer, contentSize);
			buffer[contentSize] = '\0';
			string getMsgBody = buffer;
			delete[] buffer;
			if (getMsgBody.find('$') != std::string::npos) {
				string FirstDollarSplit = getMsgBody.substr(getMsgBody.find('$') + 1);
				if (FirstDollarSplit.find('$') != std::string::npos) {
					string CategoryName = FirstDollarSplit.substr(0, FirstDollarSplit.find('$'));
					string FileName = FirstDollarSplit.substr(FirstDollarSplit.find('$') + 1);
					Rslt::write("\nServer needs to delete File = " + FileName + " Category = " + CategoryName +
						" message request from the client: \n");
					string FileDeletePath = argv[1];
					FileDeletePath += '/' + CategoryName + '/' + FileName;
					Rslt::write("\n FilePath to be deleted is :" + FileDeletePath);
					if (FileSystem::File::exists(FileDeletePath)) {
						FileSystem::File::remove(FileDeletePath);
					}
				}
			}
		}
	}
	catch (std::exception& exc)
	{
		Rslt::write("\n  Exeception caught in the method deleteSingleFileOnServer: \n");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Rslt::write(exMsg);
	}
}

bool processPostMessages(HttpMessage &msg, int argc, char* argv[]) {
	cout<<"\nServer needs to process the Post message request from the client: \n"<<endl;
	if (msg.findValue("messageType") != "")
	{
		if (msg.findValue("messageType").compare("runCodeAnalyzer") == 0) {
			cout << "\nServer needs to process the runCodeAnalyzer message request from the client: \n";
			runCodeAnalyzerOnServer(argc, argv);
		}
		else if (msg.findValue("messageType").compare("deleteFileFromServer") == 0) {
			deleteSingleFileOnServer(msg, argc, argv);
		}
	}
	return true;
}


HttpMessage ClientHandler::readMessageForPost(HttpMessage& msg, Socket& socket) {

	std::string filename = msg.findValue("file");
	if (filename != "")
	{
		size_t contentSize;
		std::string sizeString = msg.findValue("content-length");
		if (sizeString != "")
			contentSize = Converter<size_t>::toValue(sizeString);
		else
			return msg;

		std::string getDir = msg.findValue("directory");

		readFile(filename, contentSize, getDir, socket);
	}

	if (filename != "")
	{
		msg.removeAttribute("content-length");
		std::string bodyString = "<file>" + filename + "</file>";
		std::string sizeString = Converter<size_t>::toString(bodyString.size());
		msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
		msg.addBody(bodyString);
	}
	else
	{
		size_t numBytes = 0;
		size_t pos = msg.findAttribute("content-length");
		if (pos < msg.attributes().size())
		{
			numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
			Socket::byte* buffer = new Socket::byte[numBytes + 1];
			socket.recv(numBytes, buffer);
			buffer[numBytes] = '\0';
			std::string msgBody(buffer);
			msg.addBody(msgBody);
			delete[] buffer;
		}
	}

	return msg;
}

//----< this defines processing to frame messages >------------------

HttpMessage ClientHandler::readMessage(Socket& socket)
{  connectionClosed_ = false;
	HttpMessage msg;
	while (true)
	{
		std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1)
		{
			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);
		}
		else
			break;
	}
	if (msg.attributes().size() == 0)
	{
		connectionClosed_ = true;
		return msg;
	}
	if (msg.attributes()[0].first == "POST")
		msg = readMessageForPost(msg, socket);
	else if (msg.attributes()[0].first == "GET") {
		std::string getMessageType = msg.findValue("messageType");
			size_t numBytes = 0;
			size_t pos = msg.findAttribute("content-length");
			if (pos < msg.attributes().size())
			{
				numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
				Socket::byte* buffer = new Socket::byte[numBytes + 1];
				socket.recv(numBytes, buffer);
				buffer[numBytes] = '\0';
				std::string msgBody(buffer);
				msg.addBody(msgBody);
				delete[] buffer;
			}
	}
	return msg;
}
//----< read a binary file from socket and save >--------------------
/*
* This function expects the sender to have already send a file message,
* and when this function is running, continuosly send bytes until
* fileSize bytes have been sent.
*/
bool ClientHandler::readFile(const std::string& filename, size_t fileSize, 
	                         const std::string& getDir, Socket& socket)
{
	//std::string fqname = "../TestFiles_Client_Server/" + filename + ".snt";
  	std::string checkDirPath = path_+"/"+getDir; 
	if (FileSystem::Directory::exists(checkDirPath) == false)
	{
		FileSystem::Directory::create(checkDirPath);
	}
	std::string fqname = path_ + "/" + getDir + "/" + filename;
	Rslt::write(" Trying to readFile: " + fqname + " \n");
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
		Rslt::write("\n\n  can't open file " + fqname);
		return false;
	}

	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	size_t bytesToRead;
	while (true)
	{
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;

		socket.recv(bytesToRead, buffer);

		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);

		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;
	}
	file.close();
	return true;
}
//----< receiver functionality is defined by this function >---------

void ClientHandler::operator()(Socket socket)
{
	
	while (true)
	{
		HttpMessage msg = readMessage(socket);
	
		if (connectionClosed_ || msg.bodyString() == "quit")
		{
			Rslt::write("\n\n  clienthandler thread is terminating \n");
			break;
		}
		//if (msg.toString().size() > 5) {
		msgQ_.enQ(msg);
		//}
	}
}

bool ClientHandler::ProcessCommandLine(int argc, char * argv[])
{
	
	try {
		path_ = FileSystem::Path::getFullFileSpec(argv[1]);
		if (!FileSystem::Directory::exists(path_))
		{
			std::cout << "\n\n  path \"" << path_ << "\" does not exist\n\n";
			return false;
		}

	}
	catch (std::exception& ex)
	{
		Utils::Title("\ncommand line argument parsing error:");
		std::cout << "\n  " << ex.what() << "\n\n";
		return false;
	}
	Rslt::write("\nInside ProcessCommandLine of ClientHandler");
	return true;
}

//----< initialize application specific FileMgr >--------------------
/*
 * - Accepts CodeAnalysisExecutive's path and fileMap by reference
 */
AnalFileMgr::AnalFileMgr(const Path& path, FileMap& fileMap)
  : FileMgr(path), fileMap_(fileMap), numFiles_(0), numDirs_(0) {}

//----< override of FileMgr::file(...) to store found files >------

void AnalFileMgr::file(const File& f)
{
  File fqf = d_ + "\\" + f;
  Ext ext = FileSystem::Path::getExt(fqf);
  Pattern p = "*." + ext;
  fileMap_[p].push_back(fqf);
  ++numFiles_;
}
//----< override of FileMgr::dir(...) to save current dir >----------

void AnalFileMgr::dir(const Dir& d)
{
  d_ = d;
  ++numDirs_;
}
//----< override of FileMgr::done(), not currently used >------------

void AnalFileMgr::done()
{
}
//----< returns number of matched files from search >----------------

size_t AnalFileMgr::numFiles()
{
  return numFiles_;
}

//----< returns number of dirs searched >----------------------------

size_t AnalFileMgr::numDirs()
{
  return numDirs_;
}

/////////////////////////////////////////////////////////////////////
// CodeAnalysisExecutive class
// - 
using Path = std::string;
using Pattern = std::string;
using Patterns = std::vector<Pattern>;
using File = std::string;
using Files = std::vector<File>;
using Ext = std::string;
using FileMap = std::unordered_map<Pattern, Files>;
using ASTNodes = std::vector<ASTNode*>;
using FileToNodeCollection = std::vector<std::pair<File, ASTNode*>>;

std::unordered_map<std::string, std::set<std::string>> CodeAnalysisExecutive::depmapforlazyloading = { {} };
//----< initialize parser, get access to repository >----------------

CodeAnalysisExecutive::CodeAnalysisExecutive()
{
  pParser_ = configure_.Build();
  if (pParser_ == nullptr)
  {
    throw std::exception("couldn't create parser");
  }
  pRepo_ = Repository::getInstance();
}

//----< cleanup >----------------------------------------------------

CodeAnalysisExecutive::~CodeAnalysisExecutive()
{
  /*
   *  Nothing to do:
   *  - pParser_ and pRepo_ point to objects that the configure_
   *    destructor will delete.
   *  - The AbstractSynTree object will cleanup its contents when
   *    it goes out of scope by deleting the root of the AST node
   *    tree.  
   *  - Each node deletes its children, so, again, nothing more 
   *    to do.
   *  - This is here to present these comments and to make this
   *    base destructor virtual.
   */
}
//----< usage message >----------------------------------------------

void showUsage()
{
  std::ostringstream out;
  out << "\n  Usage:";
  out << "\n  Command Line Arguments are:";
  out << "\n  - 1st: path to subdirectory containing files to analyze";
  out << "\n  - remaining non-option arguments are file patterns, e.g., *.h and/or *.cpp, etc.";
  out << "\n  - must have at least one file pattern to specify what to process";
  out << "\n  - option arguments have the format \"\"/x\" , where x is one of the options:";
  out << "\n    - m : display function metrics";
  out << "\n    - s : display file sizes";
  out << "\n    - a : display Abstract Syntax Tree";
  out << "\n    - r : set logger to display results";
  out << "\n    - d : set logger to display demo outputs";
  out << "\n    - b : set logger to display debug outputs";
  out << "\n    - f : write all logs to logfile.txt";
  out << "\n  A metrics summary is always shown, independent of any options used or not used";
  out << "\n\n";
  std::cout << out.str();
  //Rslt::write(out.str());
  //Rslt::flush();
}
//----< show command line arguments >--------------------------------

void CodeAnalysisExecutive::showCommandLineArguments(int argc, char* argv[])
{
  std::ostringstream out;
  out << "\n     Path: \"" << FileSystem::Path::getFullFileSpec(argv[1]) << "\"\n     Args: ";
  for (int i = 2; i < argc - 1; ++i)
    out << argv[i] << ", ";
  out << argv[argc - 1];
  Rslt::write(out.str());
  Rslt::flush();
}
//----< handle command line arguments >------------------------------
/*
* Arguments are:
* - path: possibly relative path to folder containing all analyzed code,
*   e.g., may be anywhere in the directory tree rooted at that path
* - patterns: one or more file patterns of the form *.h, *.cpp, and *.cs
* - options: /m (show metrics), /s (show file sizes), and /a (show AST)
*/
bool CodeAnalysisExecutive::ProcessCommandLine(int argc, char* argv[])
{
  if (argc < 2)
  {
    showUsage();
    return false;
  }
  try {
    path_ = FileSystem::Path::getFullFileSpec(argv[1]);
    if (!FileSystem::Directory::exists(path_))
    {
      std::cout << "\n\n  path \"" << path_ << "\" does not exist\n\n";
      return false;
    }
    for (int i = 2; i < argc; ++i)
    {
      if (argv[i][0] == '/')
        options_.push_back(argv[i][1]);
      else
        patterns_.push_back(argv[i]);
    }
    if (patterns_.size() == 0)
    {
      showUsage();
      return false;
    }
  }
  catch (std::exception& ex)
  {
	Utils::Title("\ncommand line argument parsing error:");
    std::cout << "\n  " << ex.what() << "\n\n";
    return false;
  }
  return true;
}
//----< returns path entered on command line >-------------------

std::string CodeAnalysisExecutive::getAnalysisPath()
{
  return path_;
}
//----< returns reference to FileMap >---------------------------
/*
 * Supports quickly finding all the files found with a give pattern
 */
FileMap& CodeAnalysisExecutive::getFileMap()
{
  return fileMap_;
}
//----< searches path for files matching specified patterns >----
/*
 * - Searches entire diretory tree rooted at path_, evaluated 
 *   from a command line argument.
 * - Saves found files in FileMap.
 */
void CodeAnalysisExecutive::getSourceFiles()
{
  AnalFileMgr fm(path_, fileMap_);
  for (auto patt : patterns_)
    fm.addPattern(patt);
  fm.search();
  numFiles_ = fm.numFiles();
  numDirs_ = fm.numDirs();
}

std::string CodeAnalysisExecutive::systemTime()
{
	time_t sysTime = time(&sysTime);
	char buffer[27];
	ctime_s(buffer, 27, &sysTime);
	buffer[24] = '\0';
	std::string temp(buffer);
	return temp;
}

//----< helper: is text a substring of str? >--------------------

bool contains(const std::string& str, const std::string& text)
{
  if (str.find(text) < str.length())
    return true;
  return false;
}
//----< retrieve from fileMap all files matching *.h >-----------

std::vector<File>& CodeAnalysisExecutive::cppHeaderFiles()
{
  cppHeaderFiles_.clear();
  for (auto item : fileMap_)
  {
    if (contains(item.first, "*.h"))
    {
      for (auto file : item.second)
        cppHeaderFiles_.push_back(file);
    }
  }
  return cppHeaderFiles_;
}

//----< retrieve from fileMap all files matching *.cpp >---------

std::vector<File>& CodeAnalysisExecutive::cppImplemFiles()
{
  cppImplemFiles_.clear();
  for (auto item : fileMap_)
  {
    if (contains(item.first, "*.cpp"))
    {
      for (auto file : item.second)
        cppImplemFiles_.push_back(file);
    }
  }
  return cppImplemFiles_;
}
//----< retrieve from fileMap all files matching *.cs >----------

std::vector<File>& CodeAnalysisExecutive::csharpFiles()
{
  csharpFiles_.clear();
  for (auto item : fileMap_)
  {
    if (contains(item.first, "*.cs"))
    {
      for (auto file : item.second)
        csharpFiles_.push_back(file);
    }
  }
  return csharpFiles_;
}
//----< report number of Source Lines Of Code (SLOCs) >----------

CodeAnalysisExecutive::Slocs CodeAnalysisExecutive::fileSLOCs(const File& file) 
{ 
  return slocMap_[file];
}
//----< report number of files processed >-----------------------

size_t CodeAnalysisExecutive::numFiles()
{
  return numFiles_;
}
//----< report number of directories searched >------------------

size_t CodeAnalysisExecutive::numDirs()
{
  return numDirs_;
}
//----< show processing activity >-------------------------------

void CodeAnalysisExecutive::showActivity(const File& file)
{
  std::function<std::string(std::string, size_t)> trunc = [](std::string in, size_t count)
  {
    return in.substr(0, count);
  };

  if (Rslt::running())
  {
    std::cout << std::left << "\r     Processing file: " << std::setw(80) << trunc(file, 80);
  }
}

void CodeAnalysisExecutive::clearActivity()
{
  if (Rslt::running())
  {
    std::cout << std::left << "\r                      " << std::setw(80) << std::string(80,' ');
  }
}
//----< parses code and saves results in AbstrSynTree >--------------
/*
* - Processes C++ header files first to build AST with nodes for
*   all public classes and structs.
* - Then processes C++ implementation files.  Each member function
*   is relocated to its class scope node, not the local scope.
* - Therefore, this ordering is important.
* - C# code has all member functions inline, so we don't need to
*   do any relocation of nodes in the AST.  Therefore, that analysis
*   can be done at any time.
* - If you bore down into the analysis code in ActionsAndRules.h you
*   will find some gymnastics to handle template syntax.  That can
*   get somewhat complicated, so there may be some latent bugs there.
*   I don't know of any at this time.
*/
void CodeAnalysisExecutive::setLanguage(const File& file)
{
  std::string ext = FileSystem::Path::getExt(file);
  if (ext == "h" || ext == "cpp")
    pRepo_->language() = Language::Cpp;
  else if (ext == "cs")
    pRepo_->language() = Language::CSharp;
}

void CodeAnalysisExecutive::processSourceCode(bool showProc)
{
  for (auto file : cppHeaderFiles())
  {
    if (showProc)
      showActivity(file);
    pRepo_->package() = FileSystem::Path::getName(file);

    if (!configure_.Attach(file))
    {
      std::ostringstream out;
      out << "\n  could not open file " << file << "\n";
      Rslt::write(out.str());
      Rslt::flush();
      continue;
    }
    // parse file
    Rslt::flush();
    Demo::flush();
    Dbug::flush();

    if(!Rslt::running())
      Demo::write("\n\n  opening file \"" + pRepo_->package() + "\"");
    if(!Demo::running() && !Rslt::running())
      Dbug::write("\n\n  opening file \"" + pRepo_->package() + "\"");
    pRepo_->language() = Language::Cpp;
    pRepo_->currentPath() = file;
    while (pParser_->next())
    {
      pParser_->parse();
    }

    Slocs slocs = pRepo_->Toker()->currentLineCount();
    slocMap_[pRepo_->package()] = slocs;
  }
  for (auto file : cppImplemFiles())
  {
    if (showProc)
      showActivity(file);
    pRepo_->package() = FileSystem::Path::getName(file);

    if (!configure_.Attach(file))
    {
      std::ostringstream out;
      out << "\n  could not open file " << file << "\n";
      Rslt::write(out.str());
      Rslt::flush();
      continue;
    }
    // parse file

    if (!Rslt::running())
      Demo::write("\n\n  opening file \"" + pRepo_->package() + "\"");
    if (!Demo::running() && !Rslt::running())
      Dbug::write("\n\n  opening file \"" + pRepo_->package() + "\"");
    pRepo_->language() = Language::Cpp;
    pRepo_->currentPath() = file;
    while (pParser_->next())
      pParser_->parse();

    Slocs slocs = pRepo_->Toker()->currentLineCount();
    slocMap_[pRepo_->package()] = slocs;
  }
  for (auto file : csharpFiles())
  {
    if (showProc)
      showActivity(file);
    pRepo_->package() = FileSystem::Path::getName(file);

    if (!configure_.Attach(file))
    {
      std::ostringstream out;
      out << "\n  could not open file " << file << "\n";
      Rslt::write(out.str());
      continue;
    }
    // parse file

    if (!Rslt::running())
      Demo::write("\n\n  opening file \"" + pRepo_->package() + "\"");
    if (!Demo::running() && !Rslt::running())
      Dbug::write("\n\n  opening file \"" + pRepo_->package() + "\"");
    pRepo_->language() = Language::CSharp;
    pRepo_->currentPath() = file;
    while (pParser_->next())
      pParser_->parse();

    Slocs slocs = pRepo_->Toker()->currentLineCount();
    slocMap_[pRepo_->package()] = slocs;
  }
  if (showProc)
    clearActivity();
  std::ostringstream out;
  out << std::left << "\r  " << std::setw(77) << " ";
  Rslt::write(out.str());
}
//----< evaluate complexities of each AST node >---------------------

void CodeAnalysisExecutive::complexityAnalysis()
{
  ASTNode* pGlobalScope = pRepo_->getGlobalScope();
  CodeAnalysis::complexityEval(pGlobalScope);
}
//----< comparison functor for sorting FileToNodeCollection >----
/*
* - supports stable sort on extension values
* - displayMetrics(...) uses to organize metrics display
*/
struct CompExts
{
  bool operator()(const std::pair<File, ASTNode*>& first, const std::pair<File, ASTNode*>& second)
  {
    return FileSystem::Path::getExt(first.first) > FileSystem::Path::getExt(second.first);
  }
};
//----< comparison functor for sorting FileToNodeCollection >----
/*
* - supports stable sort on name values
* - displayMetrics(...) uses these functions to organize metrics display
*/
static void removeExt(std::string& name)
{
  size_t extStartIndex = name.find_last_of('.');
  name = name.substr(0, extStartIndex);
}

struct CompNames
{
  bool operator()(const std::pair<File, ASTNode*>& first, const std::pair<File, ASTNode*>& second)
  {
    std::string fnm = FileSystem::Path::getName(first.first);
    removeExt(fnm);
    std::string snm = FileSystem::Path::getName(second.first);
    removeExt(snm);
    return fnm < snm;
  }
};
//----< display header line for displayMmetrics() >------------------

void CodeAnalysisExecutive::displayHeader()
{
  std::ostringstream out;
  out << std::right;
  out << "\n ";
  out << std::setw(25) << "file name";
  out << std::setw(12) << "type";
  out << std::setw(35) << "name";
  out << std::setw(8) << "line";
  out << std::setw(8) << "size";
  out << std::setw(8) << "cplx";
  out << std::right;
  out << "\n  ";
  out << std::setw(25) << "-----------------------";
  out << std::setw(12) << "----------";
  out << std::setw(35) << "---------------------------------";
  out << std::setw(8) << "------";
  out << std::setw(8) << "------";
  out << std::setw(8) << "------";
  Rslt::write(out.str());
}
//----< display single line for displayMetrics() >-------------------

void CodeAnalysisExecutive::displayMetricsLine(const File& file, ASTNode* pNode)
{
  std::function<std::string(std::string, size_t)> trunc = [](std::string in, size_t count)
  {
    return in.substr(0, count);
  };
  std::ostringstream out;
  out << std::right;
  out << "\n ";
  out << std::setw(25) << trunc(file, 23);
  out << std::setw(12) << pNode->type_;
  out << std::setw(35) << trunc(pNode->name_, 33);
  out << std::setw(8) << pNode->startLineCount_;
  out << std::setw(8) << pNode->endLineCount_ - pNode->startLineCount_ + 1;
  size_t debug1 = pNode->startLineCount_;
  size_t debug2 = pNode->endLineCount_;
  out << std::setw(8) << pNode->complexity_;
  Rslt::write(out.str());
}
//----< display lines containing public data declaration >-----------

std::string CodeAnalysisExecutive::showData(const Scanner::ITokCollection* pTc)
{
  std::string semiExpStr;
  for (size_t i=0; i<pTc->length(); ++i)
    semiExpStr += (*pTc)[i] + " ";
  return semiExpStr;
}

void CodeAnalysisExecutive::displayDataLines(ASTNode* pNode, bool isSummary)
{
  for (auto datum : pNode->decl_)
  {
    if (pNode->parentType_ == "namespace" || pNode->parentType_ == "class" || pNode->parentType_ == "struct")
    {
      if (pNode->type_ == "function" || pNode->parentType_ == "function")
        continue;
      if (datum.access_ == Access::publ && datum.declType_ == DeclType::dataDecl)
      {
        std::ostringstream out;
        out << std::right;
        out << "\n ";
        out << std::setw(25) << "public data:" << " ";
        if (isSummary)
        {
          out << datum.package_ << " : " << datum.line_ << " - "
            << pNode->type_ << " " << pNode->name_ << "\n " << std::setw(15) << " ";
        }
        out << showData(datum.pTc);
        Rslt::write(out.str());
      }
    }
  }
}
//----<  helper for displayMetrics() >-------------------------------
/*
* - Breaking this out as a separate function allows application to
*   display metrics for a subset of the Abstract Syntax Tree
*/
void CodeAnalysisExecutive::displayMetrics(ASTNode* root)
{
  flushLogger();
  std::ostringstream out;
  out << "Code Metrics - Start Line, Size (lines/code), and Complexity (number of scopes)";
  Utils::sTitle(out.str(), 3, 92, out, '=');
  out << "\n";
  Rslt::write(out.str());

  std::function<void(ASTNode* pNode)> co = [&](ASTNode* pNode) {
    if (
      pNode->type_ == "namespace" ||
      pNode->type_ == "function" ||
      pNode->type_ == "class" ||
      pNode->type_ == "interface" ||
      pNode->type_ == "struct" ||
      pNode->type_ == "lambda"
      )
      fileNodes_.push_back(std::pair<File, ASTNode*>(pNode->package_, pNode));
  };
  ASTWalkNoIndent(root, co);
  std::stable_sort(fileNodes_.begin(), fileNodes_.end(), CompExts());
  std::stable_sort(fileNodes_.begin(), fileNodes_.end(), CompNames());

  displayHeader();

  std::string prevFile;
  for (auto item : fileNodes_)
  {
    if (item.first != prevFile)
    {
      Rslt::write("\n");
      displayHeader();
    }
    displayMetricsLine(item.first, item.second);
    displayDataLines(item.second);
    prevFile = item.first;
  }
  Rslt::write("\n");
}
//----< display metrics results of code analysis >---------------

void CodeAnalysisExecutive::displayMetrics()
{
  ASTNode* pGlobalScope = pRepo_->getGlobalScope();
  displayMetrics(pGlobalScope);
}
//----< walk tree of element nodes >---------------------------------

template<typename element>
void TreeWalk(element* pItem, bool details = false)
{
  static std::string path;
  if (path != pItem->path_ && details == true)
  {
    path = pItem->path_;
    Rslt::write("\n" + path);
  }
  static size_t indentLevel = 0;
  std::ostringstream out;
  out << "\n  " << std::string(2 * indentLevel, ' ') << pItem->show();
  Rslt::write(out.str());
  auto iter = pItem->children_.begin();
  ++indentLevel;
  while (iter != pItem->children_.end())
  {
    TreeWalk(*iter);
    ++iter;
  }
  --indentLevel;
}
//----< display the AbstrSynTree build in processSourceCode() >------

void CodeAnalysisExecutive::displayAST()
{
  flushLogger();
  ASTNode* pGlobalScope = pRepo_->getGlobalScope();
  Utils::title("Abstract Syntax Tree");
  TreeWalk(pGlobalScope);
  Rslt::write("\n");
}
//----< show functions with metrics exceeding specified limits >-----

void CodeAnalysisExecutive::displayMetricSummary(size_t sMax, size_t cMax)
{
  flushLogger();
  std::ostringstream out;
  Utils::sTitle("Functions Exceeding Metric Limits and Public Data", 3, 92, out, '=');
  Rslt::write(out.str());
  displayHeader();

  if (fileNodes_.size() == 0)  // only build fileNodes_ if displayMetrics hasn't been called
  {
    std::function<void(ASTNode* pNode)> co = [&](ASTNode* pNode) {
      fileNodes_.push_back(std::pair<File, ASTNode*>(pNode->package_, pNode));
    };
    ASTNode* pGlobalNamespace = pRepo_->getGlobalScope();
    ASTWalkNoIndent(pGlobalNamespace, co);
    std::stable_sort(fileNodes_.begin(), fileNodes_.end(), CompExts());
    std::stable_sort(fileNodes_.begin(), fileNodes_.end(), CompNames());
  }
  for (auto item : fileNodes_)
  {
    if (item.second->type_ == "function")
    {
      size_t size = item.second->endLineCount_ - item.second->startLineCount_ + 1;
      size_t cmpl = item.second->complexity_;
      if (size > sMax || cmpl > cMax)
        displayMetricsLine(item.first, item.second);
    }
  }
  Rslt::write("\n");
  for (auto item : fileNodes_)
  {
    displayDataLines(item.second, true);
  }
  Rslt::write("\n");
}
//----< comparison functor for sorting SLOC display >----------------

struct compFiles
{
private:
  std::string ChangeFirstCharOfExt(const std::string& fileName) const
  {
    std::string temp = fileName;
    size_t pos = temp.find_last_of('.');
    if (pos < temp.size() - 1)
      if (temp[pos + 1] == 'h')
        temp[pos + 1] = 'a';
    return temp;
  }
public:
  bool operator()(const std::string& fileName1, const std::string& fileName2) const
  {
    return ChangeFirstCharOfExt(fileName1) < ChangeFirstCharOfExt(fileName2);
  }
};
//----< show sizes of all the files processed >----------------------

void CodeAnalysisExecutive::displaySlocs()
{
  flushLogger();
  Utils::sTitle("File Size - Source Lines of Code", 3, 92);
  size_t slocCount = 0;
  std::map<std::string, size_t, compFiles> fileColl;
  for (auto item : fileMap_)
  {
    for (auto file : item.second)
    {
      File fileName = FileSystem::Path::getName(file);
      fileColl[file] = slocMap_[fileName];
    }
  }
  for (auto fitem : fileColl)
  {
    std::ostringstream out;
    out << "\n  " << std::setw(8) << fitem.second << " : " << fitem.first;
    Rslt::write(out.str());
    slocCount += fitem.second;
  }
  std::ostringstream out;
  out << "\n\n      Total line count = " << slocCount << "\n";
  Rslt::write(out.str());
  Rslt::write("\n");
}
//----< display analysis info based on command line options >--------

void CodeAnalysisExecutive::dispatchOptionalDisplays()
{
  for (auto opt : options_)
  {
    switch (opt)
    {
    case 'm':
      displayMetrics();
      Rslt::start();
      break;
    case 'a':
      displayAST();
      Rslt::start();
      break;
    case 's':
      displaySlocs();
      Rslt::start();
      break;
    default:
      break;
    }
  }
}
//----< display analysis info based on command line options >--------

void CodeAnalysisExecutive::setDisplayModes()
{
  for (auto opt : options_)
  {
    switch (opt)
    {
    case 'r':
      Rslt::start();
      break;
    case 'd':
      Demo::start();
      break;
    case 'b':
      Dbug::start();
      break;
    case 'f':
      setLogFile("logFile.txt");
      break;
    default:
      if (opt != 'a' && opt != 'b' && opt != 'd' && opt != 'f' && opt != 'm' && opt != 'r' && opt != 's')
      {
        std::cout << "\n\n  unknown option " << opt << "\n\n";
      }
    }
  }
}
//----< helper functions for managing application's logging >--------

void CodeAnalysisExecutive::startLogger(std::ostream& out)
{
  Rslt::attach(&out);
  Demo::attach(&out);
  Dbug::attach(&out);

  // will start Demo and Dbug if  have options /d and /b

  setDisplayModes();
}

void CodeAnalysisExecutive::flushLogger()
{
  Rslt::flush();
  Demo::flush();
  Dbug::flush();
}

void CodeAnalysisExecutive::stopLogger()
{
  Rslt::flush();
  Demo::flush();
  Dbug::flush();
  Rslt::stop();
  Demo::stop();
  Dbug::stop();
}
//----< open file stream for logging >-------------------------------
/*
*  - must come after CodeAnalysisExecutive::processCommandLine()
*  - must come before starting any of the loggers
*/
void CodeAnalysisExecutive::setLogFile(const File& file)
{
  std::string path = getAnalysisPath();
  path += "\\" + file;
  pLogStrm_ = new std::ofstream(path);
  if (pLogStrm_->good())
  {
    Rslt::attach(pLogStrm_);
    Demo::attach(pLogStrm_);
    Dbug::attach(pLogStrm_);
  }
  else
    Rslt::write("\n  couldn't open logFile.txt for writing");
}

#include <fstream>

void startHttpServer(int argc, char* argv[]) {
  Rslt::attach(&std::cout);
  Rslt::start();
  Rslt::title("\n  HttpMessage Server started");
  BlockingQueue<HttpMessage> msgQ;
  try
  {
	SocketSystem ss;
	SocketListener sl(8080, Socket::IP6);
	ClientHandler cp(msgQ);
	cp.ProcessCommandLine(argc, argv);
	sl.start(cp);
	 while (true)
	 {
		HttpMessage msg = msgQ.deQ();
		// if message type == "upload_files triggercodeAnalyzer.enq("upload success");
		Rslt::write("\n\n  server recvd message contents:\n" + msg.toString());
		if (msg.attributes().size() > 0)
		{
			if (msg.attributes()[0].first == "GET")
			{
				processGetMessages(msg, argc, argv);
			}
			else if (msg.attributes()[0].first == "POST")
			{
				processPostMessages(msg, argc, argv);
			}
		}
		Rslt::flush();
	 }
   }
   catch (std::exception& exc)
   {
	 Rslt::write("\n  Exeception caught: ");
	 std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
	 Rslt::write(exMsg);
   }
}

void receiver(int argc, char* argv[]) {
	try {
		startHttpServer(argc, argv);
	}
	catch (std::exception& ex) {
		cout << "Exception caught in receiver thread" << endl;
		cout << ex.what() << endl;
	}
}
void displayCommandLineArgumentsHelp(int argc, char* argv[])
{
	Utilities::StringHelper::Title("Display Arguments meaning : \n");
	for (int i = 0; i < argc; i++) {
		if (i == 1) {
			cout << "\n Argument 1:" << "To run CodeAnalyzer on the Repository folder : \n";
			cout << "        " << argv[i] << endl;
		}
		if (i == 6) {
			cout << "\n Argument 6:" << "To store the HTML files published by CodePublisher : \n";
			cout << "        " << argv[i] << endl;
		}
		if (i == 7) {
			cout << "\n Argument 7:" << "The physical path mapped to virtual Directory which also stores the HTML files in addition to local folder : \n";
			cout << "        " << argv[i] << endl;
		}
		if (i == 8) {
			cout << "\n Argument 8" << "The virtual Directory hosted by IIS is :  " << argv[8] << endl; 
			cout << " and maps to physical path : Argument 7 = " << argv[7] << endl;
		}
	}
}

int main(int argc, char* argv[])
{	
  //startHttpServer(argc, argv);
	Utilities::StringHelper::Title("Requirement 2 is met: \n");
	Utilities::StringHelper::Title("Used C++ standard library's streams for all console I/O and new and delete for all heap-based memory management.");
	Utilities::StringHelper::Title("Requirement 9 is met: Starting the Test Executive to demonstarte all the project 4 requirments");
	displayCommandLineArgumentsHelp(argc, argv);
	std::thread receieverthread(receiver, argc, argv); 
  receieverthread.join();
  return 0;
}