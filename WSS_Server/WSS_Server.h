/*
Allows for setting certificates and permission files for wss
*/

#pragma once
#include <WSServer.h>

class WSS_Server : public WSServer
{
public:
	WSS_Server(boost::asio::ip::tcp ipVersion);

	//The path of the server's ssl certificate
	void setCertPath(const std::string& certPath) {
			this->certPath = certPath;
	}

	//The path of the permission file
	void setPemPath(const std::string& pemPath) {
			this->pemPath = pemPath;
	}

	//We need to use our custom ServicePool, so we override this
	virtual void createManagers() override;

	//We need to use our custom TCPAcceptor, so we override this
	virtual void run(uint16_t port) override;

	virtual ~WSS_Server();

		std::string certPath;
		std::string pemPath;
};
