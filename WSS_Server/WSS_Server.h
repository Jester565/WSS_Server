#pragma once
#include <WSServer.h>

class WSS_Server : public WSServer
{
public:
	WSS_Server();

	void setCertPath(const std::string& certPath) {
			this->certPath = certPath;
	}

	void setPemPath(const std::string& pemPath) {
			this->pemPath = pemPath;
	}

	virtual void createManagers() override;

	virtual void run(uint16_t port) override;

	virtual ~WSS_Server();

private:
		std::string certPath;
		std::string pemPath;
};
