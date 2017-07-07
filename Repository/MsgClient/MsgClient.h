#ifndef MSG_CLIENT_H
#define MSG_CLIENT_H

#include <string>
#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
using Message = std::string;
using namespace Async;
using BQueue = BlockingQueue<Message>;


/////////////////////////////////////////////////////////////////////
// MsgClient class
// - was created as a class so more than one instance could be 
//   run on child thread
//
class MsgClient
{
public:
	using EndPoint = std::string;
	void execute(const size_t TimeBetweenMessages, const size_t NumMessages);
	void executeSingleMessage(std::string filepath);
	void executeGetCategoriesMessage();
	void executeGetAllFilesFromRepositoryBasedOnCategory(string getmsgtoretrievefilenames);
	void executeSingleFileDownloadMessage(string msg);
	void executeSingleFileDeleteRequest(string msg);
	void executeRunCodeAnalyzerOnServer(string msg);
	void startReceiver(int argc, char* argv[]);

	/* We will use this shared Blocking Queue for communication between MsgClient
	and Mock Channel. Clients enqeues the messages to the sharedQueue_ which is received
	by the receiver blocking queue to send up to shim layer and further to WPF GUI */
	void startHttpClient(BQueue &sharedQueue);
	/* We will use this method to trigger the running of code analysis on the server side */
	void sendCodeAnalysisToolRequest();
	/* start and stop Logger methods */

	void downloadJsAndCssFromServer();
private:
	HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string& fqname, Socket& socket);
};
#endif
