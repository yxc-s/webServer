#pragma once
#include <cstdint>


class Server;
class HttpController;

class Bridge{
public:
	Bridge();
	
	~Bridge();
	
	
	void handleEvent(int fd, uint32_t event);
	
	void closeHttpConnection(int fd);
	
	void newHttpConnection(int fd);
	
	void closeSocketConnection(int fd);
	
	void updateEvent(int fd, uint32_t event);

	void setSocketServer(Server* server);

	void setHttpController(HttpController* http_controller);

	
private:
	Server* server_;
	HttpController* http_controller_;
};


