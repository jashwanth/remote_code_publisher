///////////////////////////////////////////////////////////////////////////////////////
// MockChannel.cpp - Demo for CSE687 Project #4, Spring 2015                         //
// - build as static library showing how C++\CLI client can use native code channel  //
// - MockChannel reads from sendQ and writes to recvQ                                //
//                                                                                   //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015                         //
///////////////////////////////////////////////////////////////////////////////////////

#define IN_DLL
#include "MockChannel.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../MsgClient/MsgClient.h"
#include <string>
#include <thread>
#include <iostream>
using namespace Async;
using BQueue = BlockingQueue<Message>;

/////////////////////////////////////////////////////////////////////////////
// Sendr class
// - accepts messages from client for consumption by MockChannel
//
class Sendr : public ISendr
{
public:
	void postMessage(const Message& msg);
	BQueue& queue();
private:
	BQueue sendQ_;
};

void Sendr::postMessage(const Message& msg)
{
	sendQ_.enQ(msg);
}

BQueue& Sendr::queue() { return sendQ_; }

/////////////////////////////////////////////////////////////////////////////
// Recvr class
// - accepts messages from MockChanel for consumption by client
//
class Recvr : public IRecvr
{
public:
	Message getMessage();
	BQueue& queue();
private:
	BQueue recvQ_;
};

Message Recvr::getMessage()
{
	return recvQ_.deQ();
}

BQueue& Recvr::queue()
{
	return recvQ_;
}
/////////////////////////////////////////////////////////////////////////////
// MockChannel class
// - reads messages from Sendr and writes messages to Recvr
//
class MockChannel : public IMockChannel
{
public:
	MockChannel(ISendr* pSendr, IRecvr* pRecvr);
	void start();
	void stop();
	void receiverFromClientToMockChannel();
private:
	std::thread thread_;
	//std::thread threadmock_;
	ISendr* pISendr_;
	IRecvr* pIRecvr_;
	bool stop_ = false;
};

//----< pass pointers to Sender and Receiver >-------------------------------

MockChannel::MockChannel(ISendr* pSendr, IRecvr* pRecvr) : pISendr_(pSendr), pIRecvr_(pRecvr) {}

//----< creates thread to read from sendQ and echo back to the recvQ >-------


void MockChannel::start()
{ std::cout << "\n  MockChannel starting up";
	receiverFromClientToMockChannel();
	thread_ = std::thread(
		[&] {
		Sendr* pSendr = dynamic_cast<Sendr*>(pISendr_);
		Recvr* pRecvr = dynamic_cast<Recvr*>(pIRecvr_);
		BQueue& sendQ = pSendr->queue();
		BQueue& recvQ = pRecvr->queue();
		if (pSendr == nullptr || pRecvr == nullptr)
		{	std::cout << "\n  failed to start Mock Channel\n\n";
			return;}
		MsgClient c1;
		while (!stop_)
		{
			Message msg = sendQ.deQ();  // will block here so send quit message when stopping
			std::cout << "\n  MockChannel::start:: dequeud message from the wpf/message client is : \n" << endl<<msg;
			if (msg.find("uploadFile") != std::string::npos) {
				int pos = msg.find("uploadFile");
				Message mshbeingsenttoclient = msg.substr(10);
				std::cout << "\n MockChannel::start:: before sending the upload request from the client. Stripping the upload tag"  << std::endl<< mshbeingsenttoclient;
				c1.executeSingleMessage(mshbeingsenttoclient);
			}
			else if (msg.find("getcategories") != std::string::npos)
			{
				std::cout << "\n Mockchannel::start:: before sending the getcategories request from the client." << std::endl<<msg;
				c1.executeGetCategoriesMessage();
			} else if (msg.find("getFileNamesBasedOnCategory") != std::string::npos) {
				std::cout << "\n Mockchannel::start:: before sending the getFilesFromRepo request from the client." << std::endl<<msg;
				c1.executeGetAllFilesFromRepositoryBasedOnCategory(msg);
			} else if (msg.find("downloadFileHtmlContent") != std::string::npos) {
				std::cout << "\n Mockchannel::start:: before sending the getFilesFromRepo request from the client." << std::endl<<msg;
				c1.executeSingleFileDownloadMessage(msg);
			} else if (msg.find("deleteFileFromServer") != std::string::npos) {
				std::cout << "\n MockChannel::start:: before sending the File Deletion request from the client." << std::endl<<msg;
				c1.executeSingleFileDeleteRequest(msg); } 
			else if(msg.find("RunCodeAnalyzerOnServer") != std::string::npos) {
				std::cout << "\n MockChannel::start:: before sending the codeAnalyzer request from the client." << std::endl<<msg;
				c1.executeRunCodeAnalyzerOnServer(msg); } 
			else {
				std::cout << " Mockchannel::start::Enqueuing the message to the receiver queue of Mock channel. This message is from the client " << endl;
				recvQ.enQ(msg); }
		}
		std::cout << "\n  Mock channel stopping\n\n";});}
//----< signal server thread to stop >---------------------------------------

void MockChannel::stop() { stop_ = true; }

/* This method is called only once from Mock Channel start to ensure that
client is opening a listening socket to get the reply from the Test Executive */
void MockChannel::receiverFromClientToMockChannel()
{
	std::thread t3([&]() {
		try {
			Sendr* pSendr = dynamic_cast<Sendr*>(pISendr_);
			if (pSendr != nullptr)
			{
				BQueue& sharedQueue = pSendr->queue();
				MsgClient c1;
				c1.startHttpClient(sharedQueue);
				c1.downloadJsAndCssFromServer();
			}
		}
		catch (std::exception& ex) {
			cout << "Exception caught in MockChannel::receiverFromClientToMockChannel thread" << endl;
			cout << ex.what() << endl;
		}
	});
	t3.detach();
}

//----< factory functions >--------------------------------------------------

ISendr* ObjectFactory::createSendr() { return new Sendr; }

IRecvr* ObjectFactory::createRecvr() { return new Recvr; }

IMockChannel* ObjectFactory::createMockChannel(ISendr* pISendr, IRecvr* pIRecvr)
{
	return new MockChannel(pISendr, pIRecvr);
}

#ifdef TEST_MOCKCHANNEL

//----< test stub >----------------------------------------------------------

int main()
{
	ObjectFactory objFact;
	ISendr* pSendr = objFact.createSendr();
	IRecvr* pRecvr = objFact.createRecvr();
	IMockChannel* pMockChannel = objFact.createMockChannel(pSendr, pRecvr);
	pMockChannel->start();
	pSendr->postMessage("Hello World");
	pSendr->postMessage("CSE687 - Object Oriented Design");
	Message msg = pRecvr->getMessage();
	std::cout << "\n  received message = \"" << msg << "\"";
	msg = pRecvr->getMessage();
	std::cout << "\n  received message = \"" << msg << "\"";
	pSendr->postMessage("stopping");
	msg = pRecvr->getMessage();
	std::cout << "\n  received message = \"" << msg << "\"";
	pMockChannel->stop();
	pSendr->postMessage("quit");
	std::cin.get();
}
#endif
