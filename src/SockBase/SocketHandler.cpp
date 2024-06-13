#include "SocketHandler.h"
#include "Socket.h"
#include "Reactor.h"
#include "util.h"



SocketHandler::SocketHandler(): 
	events_(0), 
	current_events_(0), 
	in_epoll_(false){}

SocketHandler::SocketHandler(std::unique_ptr<Socket>&& socket): 
	socket_(std::move(socket)), 
	events_(0), 
	current_events_(0), 
	in_epoll_(false) {}

SocketHandler::SocketHandler(std::shared_ptr<Reactor>& reactor):
	reactor_(reactor), 
	events_(0), 
	current_events_(0), 
	in_epoll_(false){}

SocketHandler::SocketHandler(std::shared_ptr<Reactor>& reactor, std::unique_ptr<Socket>&& socket):
	socket_(std::move(socket)), 
	reactor_(reactor), 
	events_(0),
	current_events_(0), 
	in_epoll_(false){}

SocketHandler::~SocketHandler(){
	//this->pop();	
}


void SocketHandler::setSocket(std::unique_ptr<Socket>&& socket){
	socket_ = std::move(socket);
}

void SocketHandler::setReactor(std::shared_ptr<Reactor>&& reactor){
	reactor_ = reactor;
}

void SocketHandler::setCallback(std::function<void()> cb){
	read_callback_ = cb;
}

void SocketHandler::setCurrentEvents(uint32_t current_events){
	current_events_ = current_events;
}

void SocketHandler::setInEpoll(){
	in_epoll_ = true;
}

void SocketHandler::removeFromEpoll(){
	in_epoll_ = false;
}

bool SocketHandler::isInEpoll() const{
	return in_epoll_;
}

void SocketHandler::push(){
	ErrorIf(reactor_ == nullptr, "reactor is nullptr");
	events_ |= EPOLLIN;//| EPOLLET;
	fd_ = socket_->getFd();
	int fd = getFd();
	ErrorIf(fd <= 0 || fd > 50, "sockethandler:socket fd error");
	reactor_->push(this);
}

void SocketHandler::pop(){
	reactor_->pop(this);
}

void SocketHandler::handleEvent() const{
	read_callback_();
}

std::unique_ptr<Socket> SocketHandler::accept() const{
	return socket_->accept();
}


void SocketHandler::updateEvent(uint32_t event){
	events_ = event;
}

uint32_t SocketHandler::getEvents() const{
	return events_;
}

uint32_t SocketHandler::getCurrentEvents() const{
	return current_events_;
}

int SocketHandler::getFd() const{
	return socket_->getFd();
}

const char* SocketHandler::getIp() const{
	return socket_->getIp();
}

uint16_t SocketHandler::getPort() const{
	return socket_->getPort();
}

void SocketHandler::useEdgeTriggered(){
	events_ = EPOLLIN|EPOLLET;
}

void SocketHandler::useLevelTriggered(){
	events_ = EPOLLIN;
}

//void SocketHandler::close() const{socket_->close();}

