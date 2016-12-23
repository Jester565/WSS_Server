#include "WSS_ServicePool.h"
#include <boost/make_shared.hpp>

WSS_ServicePool::WSS_ServicePool(const std::string& chainFile, const std::string& keyFile, int usedCores)
		:ServicePool(usedCores)
{
		for (int i = 0; i < numCores; i++) {
				boost::shared_ptr<boost::asio::ssl::context> sslContext = boost::make_shared<boost::asio::ssl::context>(*services.at(i), boost::asio::ssl::context::sslv23);
			 sslContext->set_options(boost::asio::ssl::context::default_workarounds
						| boost::asio::ssl::context::no_sslv2
						| boost::asio::ssl::context::single_dh_use);
				try {
						sslContext->use_certificate_chain_file(chainFile);
						sslContext->use_private_key_file(keyFile, boost::asio::ssl::context::pem);
				}
				catch (std::exception& ex) {
					std::cerr << "ERROR WHEN CREATING SSL CONTEXT: " << ex.what() << std::endl;
				}
				sslContexts.push_back(sslContext);
		}
		sslContextIter = sslContexts.begin();
}

boost::asio::ssl::context & WSS_ServicePool::getNextSSLContext()
{
		if (sslContextIter == sslContexts.end())
		{
				sslContextIter = sslContexts.begin();
		}
		return **(sslContextIter++);
}

WSS_ServicePool::~WSS_ServicePool()
{
}
