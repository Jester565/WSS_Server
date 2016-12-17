#pragma once
#include <TCPAcceptor.h>
#include <boost/asio/ssl.hpp>

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

class WSS_TCPAcceptor : public TCPAcceptor
{
public:
	WSS_TCPAcceptor(Server* server);

	~WSS_TCPAcceptor();

protected:
	void runAccept() override;
	void asyncAcceptHandler(const boost::system::error_code& error) override;
	ssl_socket* tempSSLSocket;
};

