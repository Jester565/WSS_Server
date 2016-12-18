#include "WSS_Server.h"
#include "WSS_TCPAcceptor.h"
#include "WSS_ServicePool.h"
#include <PacketManager.h>
#include <ClientManager.h>

WSS_Server::WSS_Server(boost::asio::ip::tcp ipVersion)
	:WSServer(ipVersion)
{
}

void WSS_Server::createManagers()
{
		servicePool = new WSS_ServicePool(certPath, pemPath);
		pm = new PacketManager(this);
		cm = new ClientManager(this);
}


void WSS_Server::run(uint16_t port)
{
		tcpAcceptor = boost::make_shared <WSS_TCPAcceptor>(this);
		tcpAcceptor->detach(port);
		servicePool->run();
}

WSS_Server::~WSS_Server()
{
	
}
