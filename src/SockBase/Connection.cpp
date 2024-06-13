#include <iostream>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include "Connection.h"
#include "Reactor.h"
#include "Socket.h"
#include "SocketHandler.h"
#include "util.h"
#include <sys/epoll.h>



Connection::Connection(std::shared_ptr<Reactor> reactor, std::unique_ptr<Socket>&& socket):
	reactor_(reactor)
{
	socket->setNonBlock();
	state_ = State::Connected;
	socket_handler_ = std::make_unique<SocketHandler> (reactor_, std::move(socket));
	socket_handler_->setCallback(std::bind(&Connection::handleEvent, this));	
}

Connection::~Connection(){
	SocketHandler* handler = socket_handler_.release();
	reactor_->pop(handler);
	delete handler;
	handler = nullptr;
}

void Connection::deleteConnection(){
	state_ = State::Closed;
	//delete_connection_callback_(socket_handler_->getFd());
}

void Connection::handleEvent(){
	writeLog("Connection:开始处理事件，fd:" + std::to_string(socket_handler_->getFd()) + (socket_handler_->getCurrentEvents() & EPOLLIN ? "EPOLLIN" : "EPOLLOUT") + "\n");
	handle_event_callback_(socket_handler_->getFd(), socket_handler_->getCurrentEvents());
}

void Connection::push() const{
	socket_handler_->push();
	writeLog(std::to_string(socket_handler_->getFd()) + "已被监听读事件\n");
}

//void Connection::setDeleteConnectionCallback(std::function<bool(int)> cb){
//	delete_connection_callback_ = cb;
//}

void Connection::setHandleEventCallback(std::function<void(int, uint32_t)> cb){
	handle_event_callback_ = cb;	
}

void Connection::updateEvent(uint32_t event){
	writeLog("Connection update handler:" + getHexAddr(socket_handler_.get()) + "\n");
	socket_handler_->updateEvent(event);
	reactor_->update(socket_handler_.get());
}

int Connection::getState() const{
	return state_;
}

int Connection::getFd() const{
	return socket_handler_->getFd();
}

uint16_t Connection::getPort() const{
	return socket_handler_->getPort();
}

const char* Connection::getIp() const{
	return socket_handler_->getIp();
}



