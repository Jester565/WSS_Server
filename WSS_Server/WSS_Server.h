#pragma once
#include <WSServer.h>

class WSS_Server : public WSServer
{
public:
	WSS_Server();

	virtual void asyncInitServer(uint16_t port) override;

	virtual void syncInitServer(uint16_t port) override;

	virtual ~WSS_Server();
};
//commit please
