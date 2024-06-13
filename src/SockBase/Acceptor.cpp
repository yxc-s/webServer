#include "Acceptor.h"
#include "Socket.h"
#include "Connection.h"
#include "Reactor.h"
#include "util.h"
#include "SocketHandler.h"

Acceptor::Acceptor():
	reactor_(nullptr), 
	socket_handler_(nullptr){}

Acceptor::Acceptor(std::shared_ptr<Reactor>& reactor, const char* ip, uint16_t port): 
	reactor_(reactor)
{
	std::unique_ptr<Socket> sock = std::make_unique<Socket>(ip, port);
	sock->bind();
	sock->setNonBlock();
	sock->listen();
	socket_handler_ = std::make_unique<SocketHandler>(reactor_);
	socket_handler_->setSocket(std::move(sock));
	socket_handler_->setCallback(std::bind(&Acceptor::newConnection, this));//第一个参数后面不用家括号
	socket_handler_->useLevelTriggered();
	socket_handler_->push();
}

Acceptor::~Acceptor(){}


void Acceptor::newConnection() const{
	new_connection_callback_(std::move(socket_handler_->accept()));
	//new_connection_callback_(socket_handler_->accept());
}

void Acceptor::setNewConnectionCallback(std::function<void(std::unique_ptr<Socket>)> cb){
	new_connection_callback_ = cb;
}








