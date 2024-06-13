#include "Server.h"
#include "HttpController.h"
#include "Bridge.h"
#include <memory>




int main(){
	Server server;
	HttpController http_controller;
	std::shared_ptr<Bridge> bridge = std::make_shared<Bridge> ();
	
	server.setBridge(bridge);

	http_controller.setBridge(bridge);
	bridge->setSocketServer(&server);
	bridge->setHttpController(&http_controller);
	
	server.listen();

	return 0;
}
