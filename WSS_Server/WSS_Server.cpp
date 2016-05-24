#include "WSS_Server.h"
#include "WSS_TCPAcceptor.h"

WSS_Server::WSS_Server()
	:WSServer()
{
}


void WSS_Server::asyncInitServer(uint16_t port)
{
	tcpAcceptor = boost::make_shared <WSS_TCPAcceptor>(this);
	tcpAcceptor->detach(port);
	ioServiceThread = new std::thread(&Server::asyncIOService, this);
	ioServiceThread->detach();
}

void WSS_Server::syncInitServer(uint16_t port)
{
	tcpAcceptor = boost::make_shared <WSS_TCPAcceptor>(this);
	tcpAcceptor->detach(port);
	ioService->run();
}


WSS_Server::~WSS_Server()
{
	
}
