/////////////////////////////////////////////////////////////////////////
// MsgClient.cpp - Demonstrates simple one-way HTTP messaging          //
//                                                                     //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016           //
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Dell XPS 8900, Windows 10 pro      //
/////////////////////////////////////////////////////////////////////////
/*
* This package implements a client that sends HTTP style messages and
* files to a server that simply displays messages and stores files.
*
* It's purpose is to provide a very simple illustration of how to use
* the Socket Package provided for Project #4.
*/
/*
* Required Files:
*   MsgClient.cpp, MsgServer.cpp
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp
*/
/*
* ToDo:
* - pull the sending parts into a new Sender class
* - You should create a Sender like this:
*     Sender sndr(endPoint);  // sender's EndPoint
*     sndr.PostMessage(msg);
*   HttpMessage msg has the sending adddress, e.g., localhost:8080.
*/
#include "MsgClient.h"
#include "../FileSystem/FileSystem.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <Windows.h>
#include <shellapi.h>


using Show = Logging::StaticLogger<1>;  // use for demonstrations of processing
using namespace Utilities;
using namespace Async;
using BQueue = BlockingQueue<std::string>;
using Utils = StringHelper;

class ClientHandler
{
public:
	using Path = std::string;
public:
	ClientHandler(BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);
	bool ProcessCommandLine();
	HttpMessage readMessageForPost(HttpMessage& msg, Socket& socket);
private:
	bool connectionClosed_;
	HttpMessage readMessage(Socket& socket);
	bool readFile(const std::string& filename, size_t fileSize, const std::string& getDir, Socket& socket);
	bool readFileFromServer(HttpMessage &msg, Socket& socket);
	bool readMessageBody(HttpMessage &msg, Socket& socket);
	BlockingQueue<HttpMessage>& msgQ_;
	Path path_;
};

/* This can be used only for normal message bodies except file stream etc.. */
bool ClientHandler::readMessageBody(HttpMessage &msg, Socket& socket) {
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
	return true;
}

bool ClientHandler::readFileFromServer(HttpMessage &msg, Socket& socket) {
	try {
		const size_t BlockSize = 2048;
		Socket::byte buffer[BlockSize];
		std::string recvFilePath_;
		if (msg.findValue("fileName").find(".htm") != std::string::npos)
			recvFilePath_ = "../../../clientStorage/FileStorage/" + msg.findValue("fileName");
		else
			recvFilePath_ = "../../../clientStorage/" + msg.findValue("fileName");
		 cout << "The recieve path at client end is " << FileSystem::Path::getFullFileSpec(recvFilePath_) << endl;
		if (FileSystem::File::exists(recvFilePath_)) {
			cout << "file already exist: " << recvFilePath_ << endl;
			FileSystem::File::remove(recvFilePath_); }
		FileSystem::File file(recvFilePath_);
		file.open(FileSystem::File::out, FileSystem::File::binary);
		if (file.isGood())
			cout << "File is good in client end" << endl;
		size_t bytesToRead;
		string getFileSize = msg.findValue("content-length");
		size_t fileSize = Converter<size_t>::toValue(getFileSize);
		while (true) {
			if (fileSize > BlockSize)
				bytesToRead = BlockSize;
			else
				bytesToRead = fileSize;
			socket.recv(bytesToRead, buffer);
			FileSystem::Block blk;
			for (size_t i = 0; i < bytesToRead; ++i) {
				blk.push_back(buffer[i]); }
			file.putBlock(blk);
			if (fileSize < BlockSize)
				break;
			fileSize -= BlockSize; }
		file.close();
		cout << "File : " + msg.findValue("fileName") + " was downloaded successfully on the client side successfully" << endl;
		/*if (msg.findValue("fileName").find(".htm") != std::string::npos) {
			std::string path1 = FileSystem::Path::getFullFileSpec(recvFilePath_);
			std::wstring sPath1(path1.begin(), path1.end());
			Utilities::StringHelper::Title("Trying to open the dependency Files on the IIS virtual Directory folder");
			ShellExecute(NULL, "open", sPath1.c_str(), NULL, NULL, SW_SHOWNORMAL);
		}*/
	}
	catch (std::exception& except) {
		cout << "Exception caught in readFileFromServer of client " <<  std::string(except.what()) <<  "\n\n";
	}
	return true;
}


HttpMessage ClientHandler::readMessageForPost(HttpMessage& msg, Socket& socket) {
	//// is this a file message?
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
		readMessageBody(msg, socket);
	}

	return msg;
}
//----< this defines processing to frame messages >------------------

HttpMessage ClientHandler::readMessage(Socket& socket)
{
	HttpMessage msg;
	connectionClosed_ = false;
	try {
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
		{
			msg = readMessageForPost(msg, socket);
		}
		else if (msg.attributes()[0].first == "GET") {
			cout << "\n ClientHandler::readMessage::Inside Get \n";
			std::string getMessageType = msg.findValue("messageType");
			cout << "\n ClientHandler::readMessage::The Message Type is: " << getMessageType << "\n";
			Show::write("\n");
			if ((getMessageType.compare("getCategoriesReply") == 0) || 
				(getMessageType.compare("getFileNamesReply") == 0))
			{
				readMessageBody(msg, socket);
			}
			else if (getMessageType.compare("HtmFileContentReplyFromServer") == 0) {
				cout << "\n ClientHandler::readMessage::Client is calling the readFileFromServer method\n";
				readFileFromServer(msg, socket);
			}
		}
		cout << "\n The message received from the server is " << msg.toString() << "\n";
	}
	catch (std::exception& except)
	{ 
		cout << "Exception caught in readMessage of client " << std::string(except.what()) << "\n\n";
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
	std::string checkDirPath = path_ + "/" + getDir;
	if (FileSystem::Directory::exists(checkDirPath) == false)
	{
		FileSystem::Directory::create(checkDirPath);
	}
	std::string fqname = path_ + "/" + getDir + "/" + filename;
//	Show::write(" Trying to readFile: " + fqname + " \n");
	cout << "Trying to readFile: " << fqname << " \n";
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
		cout << "\n\n can't open file " << fqname << endl;
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
		//	Show::write("\n\n  clienthandler thread is terminating");
			cout << "\n\n clientHandler thread is terminationg " << endl;
			break;
		}
		msgQ_.enQ(msg);
	}
}


/////////////////////////////////////////////////////////////////////
// ClientCounter creates a sequential number for each client
//
class ClientCounter
{
public:
	ClientCounter() { ++clientCount; }
	size_t count() { return clientCount; }
private:
	static size_t clientCount;
};

size_t ClientCounter::clientCount = 0;




// --< start Http CLient to listen for connections >-----------
void MsgClient::startHttpClient(BQueue &sharedQueue)
{
	std::thread t2([&]() {
		try {
			SocketSystem ss;
			SocketListener sl(8081, Socket::IP6);
			BlockingQueue<HttpMessage> msgQ;
			ClientHandler cp(msgQ);
			cp.ProcessCommandLine();
			cout << "\n msgClient::startHttpClient::Starting startHttpClient socket and callable object client handler\n";
			sl.start(cp);
			while (true)
			{
				HttpMessage msg = msgQ.deQ();
				size_t numBytes = 0;
				size_t pos = msg.findAttribute("content-length");
				string getMessageType = msg.findValue("messageType");
				if ((pos < msg.attributes().size()) && (msg.findValue("messageType").compare("HtmFileContentReplyFromServer") != 0))
				{
					numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
					Socket::byte* buffer = new Socket::byte[numBytes + 1];
					msg.getBody(buffer, numBytes);
					buffer[numBytes] = '\0';
					cout << "\nMessage being enqueued to sender channel is " << msg.findValue("messageType") << "$" << buffer << endl;
					sharedQueue.enQ(msg.findValue("messageType")+ "$" + buffer);
					delete[] buffer;
				}
				Utilities::StringHelper::Title("Requirement 6 is met: Used Message passing communication to receive a message");
				cout << "\n\n  MsgClient::startHttpClient:: Client received new message with contents:\n" << msg.toString() << endl;
			}
		}
		catch (std::exception& exc) {
			std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
			cout << "\n  Exception caught: " << exMsg << endl;
		}
	});
	t2.detach();
}

bool ClientHandler::ProcessCommandLine()
{
	return true;
}
void MsgClient::downloadJsAndCssFromServer()
{
	ClientCounter counter;
	size_t myCount = counter.count();
	std::string myCountString = Utilities::Converter<size_t>::toString(myCount);
	cout << " \n MsgClient::downloadJsAndCssFromServer::A request for downloading js and css files has been recieved from WPF GUI to client \n";
	cout << "\nStarting HttpMessage client to download js and css files " << myCountString << "on thread " << Utilities::Converter<std::thread::id>::toString(std::this_thread::get_id());
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			cout << "\n MsgClient::downloadJsAndCssFromServer:: Client waiting to connect to the server \n";
			::Sleep(100);
		}
		HttpMessage msg = makeMessage(2, "downloadJsAndCssFromServer", "localhost::8080");
		msg.addAttribute(HttpMessage::Attribute("messageType", "downloadJsAndCssFromServer"));
		sendMessage(msg, si);
		cout << " \n HTTP Message request to download js and css files has been sent to the Repository server" << endl;
	}
	catch (std::exception& exc)
	{
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		cout << "\n  Exeception caught in the method downloadJsAndCssFromServer: \n" << exMsg;
	}
}
//----< factory for creating messages >------------------------------
/*
* This function only creates one type of message for this demo.
* - To do that the first argument is 1, e.g., index for the type of message to create.
* - The body may be an empty string.
* - EndPoints are strings of the form ip:port, e.g., localhost:8081. This argument
*   expects the receiver EndPoint for the toAddr attribute.
*/
HttpMessage MsgClient::makeMessage(size_t n, const std::string& body, const EndPoint& ep)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost::8081";  // ToDo: make this a member of the sender
											 // given to its constructor.
	switch (n)
	{
	case 1:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("POST", "Message"));
		msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
		msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));

		msg.addBody(body);
		if (body.size() > 0)
		{
			attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
			msg.addAttribute(attrib);

		}
		break;
	case 2:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("GET", "Message"));
		msg.addAttribute(HttpMessage::Attribute("mode", "Duplex"));
		msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));
		msg.addBody(body);
		if (body.size() > 0)
		{
			attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
			msg.addAttribute(attrib);
		}
		break;
	default:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("Error", "unknown message type"));
	}
	return msg;
}
//----< send message using socket >----------------------------------

void MsgClient::sendMessage(HttpMessage& msg, Socket& socket)
{
//	Show::write( " \n MsgClient::The message being sent from client end is :: " + msg.toString());
//	Show::write("\n");
	cout << "\n MsgClient::sendMessage : The message being sent from client end is: " << msg.toString() << "\n\n";
	Utilities::StringHelper::Title("\n Sending the message \n");
	std::string msgString = msg.toString();
	socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
}
//----< send file using socket >-------------------------------------
/*
* - Sends a message to tell receiver a file is coming.
* - Then sends a stream of bytes until the entire file
*   has been sent.
* - Sends in binary mode which works for either text or binary.
*/
bool MsgClient::sendFile(const std::string& fullFilePath, Socket& socket)
{
	cout << "\n Full file path to be sent is " << fullFilePath << "\n";
	std::string fqname = fullFilePath;
	FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
	{ 
		cout << "File is not in good state to be read \n";
		return false;
	}
	/* Get only filename from file path */
	size_t found = fullFilePath.find_last_of("\\");
	std::string filename = fullFilePath.substr(found + 1);
	std::string getPath = fullFilePath.substr(0, found);
	std::string getDirectory = getPath.substr(getPath.find_last_of("\\") + 1);
	HttpMessage msg = makeMessage(1, "", "localhost::8080");
	//Show::write("\n The file name is " + filename);
	//Show::write("\n");
	cout << "\n The file name is " << filename << "\n";
	msg.addAttribute(HttpMessage::Attribute("file", filename));
	//Show::write(" \n The Directory name is " + getDirectory);
	//Show::write("\n");
	cout << "\n The directory name is " << getDirectory << "\n";
	msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
	msg.addAttribute(HttpMessage::Attribute("directory", getDirectory));
	sendMessage(msg, socket);
	
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	while (true)
	{
		FileSystem::Block blk = file.getBlock(BlockSize);
		if (blk.size() == 0)
			break;
		for (size_t i = 0; i < blk.size(); ++i)
			buffer[i] = blk[i];
		socket.send(blk.size(), buffer);
		if (!file.isGood())
			break;
	}
	Utilities::StringHelper::Title("Requirement 8 is met: File " + filename + " was sent through stream of 2048 bytes each successfully from the client end");
	file.close();
	return true;
}

void MsgClient::sendCodeAnalysisToolRequest() {
	ClientCounter counter;
	size_t myCount = counter.count();
	std::string myCountString = Utilities::Converter<size_t>::toString(myCount);
	cout << " \n MsgClient::executeSingleMessage::A request for codeAnalyzer has been recieved from wpf by client \n";
	cout << "\n  Starting HttpMessage client " << myCountString << "on thread " << Utilities::Converter<std::thread::id>::toString(std::this_thread::get_id());
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
		//	Show::write("\n MsgClient::sendCodeAnalysisToolRequest:: Client waiting to connect to the server \n");
			cout << "\n MsgClient::sendCodeAnalysisToolRequest:: Client waiting to connect to the server \n";
			::Sleep(100);
		}
		HttpMessage msg = makeMessage(1, "RunCodeAnalysis", "localhost::8080");
		msg.addAttribute(HttpMessage::Attribute("messageType", "runCodeAnalysisOnRepository"));
		sendMessage(msg, si);
		cout << " \n MsgClient::sendCodeAnalysisToolRequest::Message has been sent to the Repository server from the Client" << endl;
	}
	catch (std::exception& exc)
	{
		//Show::write("\n  Exeception caught in the method sendCodeAnalysisToolRequest: \n");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		//Show::write(exMsg);
		cout << "\n  Exeception caught in the method sendCodeAnalysisToolRequest: \n" << exMsg;
	}
}

void MsgClient::executeSingleFileDeleteRequest(string splitMsg) {
	ClientCounter counter;
	size_t myCount = counter.count();
	std::string myCountString = Utilities::Converter<size_t>::toString(myCount);
	cout << " \n MsgClient::executeSingleFileDeleteRequest:: A request for deletFile has been recieved from wpf by client \n";
	cout << "\n Starting HttpMessage client " << myCountString << "on thread " << Utilities::Converter<std::thread::id>::toString(std::this_thread::get_id());
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			cout << "\n MsgClient::executeSingleFileDeleteRequest:: Client waiting to connect to the server \n";
			::Sleep(100);
		}
		HttpMessage msg = makeMessage(1, splitMsg, "localhost::8080");
		msg.addAttribute(HttpMessage::Attribute("messageType", "deleteFileFromServer"));
		sendMessage(msg, si);
	}
	catch (std::exception& exc)
	{
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		cout << "\n  Exeception caught in the method executeSingleMessage: \n" << exMsg;
	}
}

void MsgClient::executeSingleFileDownloadMessage(string splitMsg) {

	ClientCounter counter;
	size_t myCount = counter.count();
	std::string myCountString = Utilities::Converter<size_t>::toString(myCount);
	cout << " \n MsgClient::executeSingleFileDownloadMessage:: A request for downloadfile has been recieved from wpf by client" << endl;
	cout << "\n Starting HttpMessage client" + myCountString + " on thread " + Utilities::Converter<std::thread::id>::toString(std::this_thread::get_id()) << endl;
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			cout<<"\n MsgClient::executeSingleMessage:: Client waiting to connect to the server \n"<<endl;
			::Sleep(100);
		}
		HttpMessage msg = makeMessage(2, splitMsg, "localhost::8080");
		msg.addAttribute(HttpMessage::Attribute("messageType", "downloadHtmFileFromServer"));
		cout << endl;
		sendMessage(msg, si);
		cout << " \n MsgClient::executeGetAllFilesFromRepositoryBasedOnCategory::Message has been sent to the Repository server from the Client" << endl;
	}
	catch (std::exception& exc)
	{
		cout<<"\n  Exeception caught in the method executeSingleMessage: \n";
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		cout << exMsg << endl;
	}

}

void MsgClient::executeRunCodeAnalyzerOnServer(string requestMsg)
{
	ClientCounter counter;
	size_t myCount = counter.count();
	std::string myCountString = Utilities::Converter<size_t>::toString(myCount);
    cout << " \n MsgClient::executeSingleFileDownloadMessage:: A request for downloadfile has been recieved from wpf by client \n";
	cout << "\n Starting HttpMessage client" + myCountString + " on thread " + Utilities::Converter<std::thread::id>::toString(std::this_thread::get_id()) << endl;
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			cout << "\n MsgClient::executeRunCodeAnalyzerOnServer:: Client waiting to connect to the server \n" << endl;
			::Sleep(100);
		}
		HttpMessage msg = makeMessage(1, requestMsg, "localhost::8080");
		msg.addAttribute(HttpMessage::Attribute("messageType", "runCodeAnalyzer"));
		cout << endl;
		sendMessage(msg, si);
		cout << " \n Requirement 7 is met: HTTP Message for code Analysis has been sent to the Repository" << endl;
	}
	catch (std::exception& exc)
	{
		cout << "\n  Exeception caught in the method executeRunCodeAnalyzerOnServer: \n";
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		cout << exMsg << endl;
	}
}

void MsgClient::executeSingleMessage(string filepath)
{
	ClientCounter counter;
	size_t myCount = counter.count();
	std::string myCountString = Utilities::Converter<size_t>::toString(myCount);
    cout << " \n MsgClient::executeSingleMessage:: A request for uploadfile has been recieved from wpf by client \n"<<endl;
	cout << "\n Starting HttpMessage client" + myCountString + " on thread " + Utilities::Converter<std::thread::id>::toString(std::this_thread::get_id()) << endl;

	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			cout << "\n MsgClient::executeSingleMessage:: Client waiting to connect to the server \n" << endl;
			::Sleep(100);
		}
		sendFile(filepath, si);
	}
	catch (std::exception& exc)
	{
		cout << "\n  Exeception caught in the method executeSingleMessage: \n";
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		cout << exMsg << endl;
	}
}

void MsgClient::executeGetAllFilesFromRepositoryBasedOnCategory(string getmsgtoretrievefilenames)
{

	std::string filecategory = getmsgtoretrievefilenames.substr(getmsgtoretrievefilenames.find('$') + 1);
	cout << "\n MsgClient::executeGetAllFilesFromRepositoryBasedOnCategory::filecategory selected by the user is: " << filecategory << endl;
    cout <<"\n MsgClient::executeGetAllFilesFromRepositoryBasedOnCategory::A request for getFiles has been recieved from wpf by client"<<endl;
	cout << endl;
	ClientCounter counter;
	size_t myCount = counter.count();
	std::string myCountString = Utilities::Converter<size_t>::toString(myCount);
	cout << "\n Starting HttpMessage client" + myCountString + " on thread " + Utilities::Converter<std::thread::id>::toString(std::this_thread::get_id()) << endl;
	cout << "\nA request for getfiles has been recieved from WPF GUI to client"<<endl;
	cout << endl;
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			cout << "\n MsgClient::executeGetAllFilesFromRepositoryBasedOnCategory:: Client waiting to connect to the server \n" << endl;
			::Sleep(100);
		}
		HttpMessage msg = makeMessage(2, "getFilesRequest", "localhost::8080");
		msg.addAttribute(HttpMessage::Attribute("messageType", "getFileNamesFromServer"));
		msg.addAttribute(HttpMessage::Attribute("FileCategory", filecategory));
		//sending message
		cout << endl;
		sendMessage(msg, si);
		cout << " \n Requirement 7 is met: HTTP Message request for Files based on category has been sent to the Repository server" << endl;
	}
	catch (std::exception& exc)
	{
		cout << "\n  Exeception caught in the method executeGetAllFilesFromRepositoryBasedOnCategory: \n";
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		cout << exMsg << endl;
	}
}

void MsgClient::executeGetCategoriesMessage()
{
	try {
		cout << endl;
	    cout<<" MsgClient::executeGetCategoriesMessage::A request for getcategories has been recieved from wpf by client"<<endl;
	    cout << endl;
	    ClientCounter counter;
	    size_t myCount = counter.count();
	    std::string myCountString = Utilities::Converter<size_t>::toString(myCount);
	    cout << "\n Starting HttpMessage client" + myCountString + " on thread " + Utilities::Converter<std::thread::id>::toString(std::this_thread::get_id()) << endl;
	    cout << endl;
        cout << "\nA request for getcategories has been recieved from WPF GUI to client"<<endl;
	    cout << endl;
	
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			cout << "\n MsgClient::executeGetCategoriesMessage:: Client waiting to connect to the server \n" << endl;
			::Sleep(100);
		}
		HttpMessage msg = makeMessage(2, "getcategoriesrequest", "localhost::8080");
		msg.addAttribute(HttpMessage::Attribute("messageType", "getCategories"));
		//sending message
		cout << endl;
		sendMessage(msg, si);
		cout << " \n Requirement 7 is met: HTTP Message Request for categories has been sent to the Repository " << endl;
	}
	catch (std::exception& exc)
	{
		cout << "\n  Exeception caught in the method executeGetCategoriesMessage: \n";
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		cout << exMsg << endl;
	}
}


#ifdef  TEST_MESSAGE_CLIENT
int main(int argc, char* argv[])
{
	::SetConsoleTitle("Clients Running on Threads");

	Show::title("Demonstrating two HttpMessage Clients each running on a child thread");

	MsgClient c1;
	std::thread t1(
		[&]() { c1.execute(100, 20); } // 20 messages 100 millisec apart
	);

	std::thread t2([&]() { c1.startReceiver(argc, argv); });

	//MsgClient c2;
	//std::thread t2(
	//	[&]() { c2.execute(120, 20); } // 20 messages 120 millisec apart
	//);
	t1.join();
	t2.join();
	//t2.join();
	getchar();
}
#endif //  TEST_MESSAGE_CLIENT