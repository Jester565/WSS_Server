#include "WSS_TCPConnection.h"
#include <ClientManager.h>
#include <PacketManager.h>
#include <WSHeaderManager.h>
#include <Server.h>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>
#include <openssl/conf.h>
#include <iostream>

WSS_TCPConnection::WSS_TCPConnection(Server* server, ssl_socket* boundSocket)
	:TCPConnection(server, nullptr), sslSocket(boundSocket)
{

}

void WSS_TCPConnection::start()
{
	sslSocket->async_handshake(boost::asio::ssl::stream_base::server, boost::bind(&WSS_TCPConnection::asyncHandshakeHandler, this, boost::asio::placeholders::error));
}

void WSS_TCPConnection::send(boost::shared_ptr<OPacket> oPack)
{
	sendStorage = new std::vector<unsigned char>(*hm->encryptHeader(oPack));
	sslSocket->async_write_some(boost::asio::buffer(*sendStorage, sendStorage->size()), boost::bind(&TCPConnection::asyncSendHandler, this, boost::asio::placeholders::error));
}

void WSS_TCPConnection::send(boost::shared_ptr<std::vector<unsigned char>> sendData)
{
	sendStorage = new std::vector<unsigned char>(*sendData);
	sslSocket->async_write_some(boost::asio::buffer(*sendStorage), boost::bind(&TCPConnection::asyncSendHandler, this, boost::asio::placeholders::error));
}

void WSS_TCPConnection::read()
{
	if (alive)
	{
		if (receiveStorage != nullptr)
		{
			delete receiveStorage;
			receiveStorage = nullptr;
		}
		receiveStorage = new std::vector<unsigned char>();
		receiveStorage->resize(MAX_DATA_SIZE);
		sslSocket->async_read_some(boost::asio::buffer(*receiveStorage, MAX_DATA_SIZE), boost::bind(&WSS_TCPConnection::asyncReceiveHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
}

void WSS_TCPConnection::asyncHandshakeHandler(const boost::system::error_code& error)
{
	if (!error)
	{
		std::cout << "SSL Handshake Success!" << std::endl;
		read();
	}
	else
	{
		std::cout << "Error occured in SSL Handshake: " << error << " - " << error.message() << std::endl;
		std::string hrerr;
		hrerr += boost::lexical_cast<std::string>(ERR_GET_LIB(error.value()));
		hrerr += ", ";
		hrerr += boost::lexical_cast<std::string>(ERR_GET_FUNC(error.value()));
		hrerr += ", ";
		hrerr += boost::lexical_cast<std::string>(ERR_GET_REASON(error.value()));
		hrerr += ", ";
		char buf[128];
		ERR_error_string_n(error.value(), buf, 128);
		hrerr += buf;
		std::cout << "Human Readable Error Version: " << hrerr << std::endl;
	}
}

void WSS_TCPConnection::asyncReceiveHandler(const boost::system::error_code& error, unsigned int nBytes)
{
	std::cout << "SSL_AsyncReceive" << std::endl;
	if (error)
	{
		if (error == boost::asio::error::connection_reset)
		{
			std::cout << "Connection Closed" << std::endl;
			server->getClientManager()->removeClient(cID);
			return;
		}
		std::cerr << "Error occured in TCP Reading: " << error << " - " << error.message() << std::endl;
		std::cerr << "Error occured in TCP Reading: " << error << " - " << error.message() << std::endl;
		std::string hrerr;
		hrerr += boost::lexical_cast<std::string>(ERR_GET_LIB(error.value()));
		hrerr += ", ";
		hrerr += boost::lexical_cast<std::string>(ERR_GET_FUNC(error.value()));
		hrerr += ", ";
		hrerr += boost::lexical_cast<std::string>(ERR_GET_REASON(error.value()));
		hrerr += ", ";
		char buf[128];
		ERR_error_string_n(error.value(), buf, 128);
		hrerr += buf;
		std::cout << "Human Readable Error Version: " << hrerr << std::endl;

		switch (errorMode)
		{
		case THROW_ON_ERROR:
			throw error;
			break;
		case RETURN_ON_ERROR:
			return;
		case RECALL_ON_ERROR:
			read();
			return;
		};
	}
	boost::shared_ptr<IPacket> iPack = hm->decryptHeader(receiveStorage, nBytes, cID);
	if (iPack != nullptr)
	{
		server->getPacketManager()->process(iPack);
	}
	read();
}

WSS_TCPConnection::~WSS_TCPConnection()
{
	std::cout << "WSS DESTRUCTOR CALLED" << std::endl;
	boost::system::error_code ec;
	if (socket != nullptr)
	{
		socket->lowest_layer().cancel();
		socket->lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		if (ec)
		{
			std::cerr << "Error when closing TCPConnection: " << ec.message() << std::endl;
		}
		socket->lowest_layer().close();
		delete socket;
		socket = nullptr;
	}
}
