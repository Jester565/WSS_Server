/*
Use ssl socket instead of normal tcp socket
*/

#pragma once
#include <TCPAcceptor.h>
#include <boost/asio/ssl.hpp>

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

class WSS_TCPAcceptor : public TCPAcceptor
{
public:
		WSS_TCPAcceptor(Server* server);

		boost::shared_ptr <WSS_TCPAcceptor> shared_from_this()
		{
				return boost::static_pointer_cast<WSS_TCPAcceptor>(TCPAcceptor::shared_from_this());
		}

		void close();

		~WSS_TCPAcceptor();

protected:
		void runAccept() override;
		void asyncShutdownHandler(const boost::system::error_code error);
		void asyncAcceptHandler(const boost::system::error_code& error) override;
		ssl_socket* tempSSLSocket;
};
