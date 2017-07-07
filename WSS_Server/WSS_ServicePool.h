/*
Each thread should have its own sslContext because it is not thread safe
*/

#pragma once
#include <ServicePool.h>
#include <boost/asio/ssl/context.hpp>

class WSS_ServicePool : public ServicePool
{
public:
		WSS_ServicePool(const std::string& chainFile, const std::string& keyFile, int usedCores = 1);

		boost::asio::ssl::context& getNextSSLContext();

		~WSS_ServicePool();

protected:
		typedef boost::shared_ptr <boost::asio::ssl::context> ssl_context_ptr;
		std::vector <ssl_context_ptr> sslContexts;
		std::vector <ssl_context_ptr>::iterator sslContextIter;
};

