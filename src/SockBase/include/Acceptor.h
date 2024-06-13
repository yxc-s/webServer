#pragma once
#include <memory>
#include <functional>


class Socket;
class SocketHandler;
class Connection;
class Reactor;

class Acceptor{
public:
	Acceptor();
	
	Acceptor(std::shared_ptr<Reactor>& reactor, const char* ip, uint16_t port);	
	
	~Acceptor();
	

	//作为handler的回调函数， 检测到新的连接请求时调用
	void newConnection() const;
	
	//acceptor被创建时在server中配置，cb是server中的函数，用于根据传递的Socket信息创建connection
	void setNewConnectionCallback(std::function<void(std::unique_ptr<Socket>)> cb);


private:
	std::shared_ptr<Reactor> reactor_;
	std::unique_ptr<SocketHandler> socket_handler_;
	std::function<void(std::unique_ptr<Socket>&&)> new_connection_callback_;
};


