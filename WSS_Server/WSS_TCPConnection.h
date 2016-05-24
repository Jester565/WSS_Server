#pragma once
#include <TCPConnection.h>
#include <boost/asio/ssl.hpp>

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

class WSS_TCPConnection : public TCPConnection
{
public:
	WSS_TCPConnection(Server* server, ssl_socket* boundSocket);

	void start() override;

	void send(boost::shared_ptr<OPacket> oPack) override;

	void send(boost::shared_ptr<std::vector<unsigned char>> sendData) override;

	void read() override;

	~WSS_TCPConnection();

protected:
	void asyncHandshakeHandler(const boost::system::error_code& error);

	void asyncReceiveHandler(const boost::system::error_code& error, unsigned int nBytes) override;

	ssl_socket* sslSocket;
};