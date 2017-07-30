#include "WSS_TCPConnection.h"
#include <Client.h>
#include <WSOPacket.h>
#include <WSIPacket.h>
#include <ClientManager.h>
#include <PacketManager.h>
#include <WSHeaderManager.h>
#include <Server.h>
#include <Logger.h>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>
#include <openssl/conf.h>

WSS_TCPConnection::WSS_TCPConnection(Server* server, ssl_socket* boundSocket)
	:TCPConnection(server, nullptr), sslSocket(boundSocket)
{
}

void WSS_TCPConnection::start()
{
	sslSocket->async_handshake(boost::asio::ssl::stream_base::server, boost::bind(&WSS_TCPConnection::asyncHandshakeHandler, boost::static_pointer_cast<WSS_TCPConnection>(shared_from_this()), boost::asio::placeholders::error));
}

void WSS_TCPConnection::send(boost::shared_ptr<OPacket> oPack)
{
	LOG_PRINTF(LOG_LEVEL::DebugLow, "Sending pack %s to id %d", oPack->getLocKey(), sender->getID());
	boost::shared_ptr<std::vector <unsigned char>> sendData = hm->encryptHeader(oPack);
	sendingMutex.lock();
	if (!sending)
	{
		sending = true;
		sendingMutex.unlock();
		boost::asio::async_write(*sslSocket, boost::asio::buffer(*sendData, sendData->size()), boost::bind(&TCPConnection::asyncSendHandler, shared_from_this(), boost::asio::placeholders::error, sendData));
	}
	else
	{
		queueSendDataMutex.lock();
		sendingMutex.unlock();
		queueSendData.push(sendData);
		queueSendDataMutex.unlock();
	}
}

void WSS_TCPConnection::send(boost::shared_ptr<std::vector<unsigned char>> sendData)
{
	sendingMutex.lock();
	if (!sending)
	{
		sending = true;
		sendingMutex.unlock();
		boost::asio::async_write(*sslSocket, boost::asio::buffer(*sendData, sendData->size()), boost::bind(&TCPConnection::asyncSendHandler, shared_from_this(), boost::asio::placeholders::error, sendData));
	}
	else
	{
		queueSendDataMutex.lock();
		sendingMutex.unlock();
		queueSendData.push(sendData);
		queueSendDataMutex.unlock();
	}
}

void WSS_TCPConnection::read()
{
	if (alive)
	{
		if (receiveStorage == nullptr)
		{
			receiveStorage = new std::vector<unsigned char>();
			receiveStorage->resize(MAX_DATA_SIZE);
		}
		sslSocket->async_read_some(boost::asio::buffer(*receiveStorage, MAX_DATA_SIZE), boost::bind(&WSS_TCPConnection::wssAsyncReceiveHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
}

void WSS_TCPConnection::close()
{
	TCPConnection::close();
	if (socket != nullptr)
	{
		sslSocket->shutdown();
		socket->lowest_layer().close();
	}
}

void WSS_TCPConnection::asyncHandshakeHandler(const boost::system::error_code& error)
{
	if (!error)
	{
		Logger::Log(LOG_LEVEL::DebugLow, "SSL Handshake Success!");
		read();
	}
	else
	{
		LOG_PRINTF(LOG_LEVEL::Error, "Error occured in SSL Handshake: %s", error.message());
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
		LOG_PRINTF(LOG_LEVEL::Error, "Human Readable Error Version: %s", hrerr);
	}
}

void WSS_TCPConnection::wssAsyncReceiveHandler(const boost::system::error_code& error, unsigned int nBytes)
{
	if (error)
	{
		if (error == boost::asio::error::connection_reset | error.category() == boost::asio::error::ssl_category)
		{
			server->getClientManager()->removeClient(sender->getID());
			return;
		}
		LOG_PRINTF(LOG_LEVEL::Error, "Error occured in SSL Read: %s", error.message());
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
		LOG_PRINTF(LOG_LEVEL::Error, "Human Readable Error Version: %s", hrerr);
	}
	boost::shared_ptr<IPacket> iPack = hm->decryptHeader(receiveStorage->data(), nBytes, sender);
	if (iPack != nullptr)
	{
		LOG_PRINTF(LOG_LEVEL::DebugLow, "Received pack %s from id %d", iPack->getLocKey(), sender->getID());
		sender->getPacketManager()->process(iPack);
	}
	read();
}

WSS_TCPConnection::~WSS_TCPConnection()
{
	if (sslSocket != nullptr)
	{
		delete sslSocket;
		sslSocket = nullptr;
	}
}
