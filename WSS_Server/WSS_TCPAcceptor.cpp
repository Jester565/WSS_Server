#include "WSS_TCPAcceptor.h"
#include "WSS_TCPConnection.h"
#include <Server.h>
#include <ClientManager.h>
#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>

WSS_TCPAcceptor::WSS_TCPAcceptor(Server* server)
	:TCPAcceptor(server), sslContext(*server->getIOService(), boost::asio::ssl::context::sslv23)
{
	sslContext.set_options(boost::asio::ssl::context::default_workarounds
		| boost::asio::ssl::context::no_sslv2
		| boost::asio::ssl::context::single_dh_use);
	sslContext.use_certificate_chain_file("local.crt");
	sslContext.use_private_key_file("local.key", boost::asio::ssl::context::pem);
	sslContext.use_tmp_dh_file("dh1024.pem");
}

void WSS_TCPAcceptor::runAccept()
{
	tempSSLSocket = new ssl_socket(*server->getIOService(), sslContext);
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
