#include "WSS_TCPAcceptor.h"
#include "WSS_TCPConnection.h"
#include "WSS_ServicePool.h"
#include <Server.h>
#include <ClientManager.h>
#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>

WSS_TCPAcceptor::WSS_TCPAcceptor(Server* server)
	:TCPAcceptor(server)
{

}

void WSS_TCPAcceptor::runAccept()
{
		WSS_ServicePool* wssServicePool = (WSS_ServicePool*)server->getServicePool();
		tempSSLSocket = new ssl_socket(wssServicePool->getNextIOService(), wssServicePool->getNextSSLContext());
		acceptor->async_accept(tempSSLSocket->lowest_layer(), boost::bind(&WSS_TCPAcceptor::asyncAcceptHandler, this, boost::asio::placeholders::error));
}

void WSS_TCPAcceptor::asyncAcceptHandler(const boost::system::error_code& error)
{
	if (error)
	{
		std::cerr << "Error occured in TCPAcceptor: " << error.message() << std::endl;
		switch (errorMode)
		{
		case THROW_ON_ERROR:
			throw error;
			break;
		case RETURN_ON_ERROR:
			return;
			break;
		case RECALL_ON_ERROR:
			if (tempSSLSocket != nullptr)
			{
				delete tempSSLSocket;
				tempSSLSocket = nullptr;
			}
			runAccept();
			return;
		};
	}
	boost::shared_ptr <TCPConnection> tcpConnection = boost::make_shared<WSS_TCPConnection>(server, tempSSLSocket);
	tcpConnection->start();
	server->getClientManager()->addClient(tcpConnection);
	runAccept();
}

WSS_TCPAcceptor::~WSS_TCPAcceptor()
{
	std::cout << "WSS_TCPACCEPTOR DESTRUCTOR CALLED" << std::endl;
	if (tempSSLSocket != nullptr)
	{
		delete tempSSLSocket;
		tempSSLSocket = nullptr;
	}
}
