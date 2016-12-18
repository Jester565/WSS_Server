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
	:TCPAcceptor(server), tempSSLSocket(nullptr)
{

}

void WSS_TCPAcceptor::runAccept()
{
		WSS_ServicePool* wssServicePool = (WSS_ServicePool*)server->getServicePool();
		tempSSLSocket = new ssl_socket(wssServicePool->getNextIOService(), wssServicePool->getNextSSLContext());
		acceptor->async_accept(tempSSLSocket->lowest_layer(), boost::bind(&WSS_TCPAcceptor::asyncAcceptHandler, shared_from_this(), boost::asio::placeholders::error));
}

void WSS_TCPAcceptor::asyncShutdownHandler(const boost::system::error_code error)
{
		std::cout << "SHUTDOWN IS A BEAR" << std::endl;
		tempSSLSocket->lowest_layer().close();
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
			runAccept();
			return;
		};
		return;
	}
	boost::shared_ptr <TCPConnection> tcpConnection = boost::make_shared<WSS_TCPConnection>(server, tempSSLSocket);
	tcpConnection->start();
	server->getClientManager()->addClient(tcpConnection);
	runAccept();
}

void WSS_TCPAcceptor::close()
{
		if (tempSSLSocket != nullptr) {
				tempSSLSocket->shutdown();
		}
}

WSS_TCPAcceptor::~WSS_TCPAcceptor()
{
		std::cout << "WSS_TCPACCEPTOR DESTRUCTOR CALLED" << std::endl;
}
