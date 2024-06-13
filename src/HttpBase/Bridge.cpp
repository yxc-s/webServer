#include "Bridge.h"
#include "Server.h"
#include "HttpController.h"
#include "util.h"
#include <string>
#include <sys/epoll.h>

Bridge::Bridge(): server_(nullptr), http_controller_(nullptr){}

Bridge::~Bridge(){
	server_ = nullptr;
	http_controller_ = nullptr;
}


void Bridge::handleEvent(int fd, uint32_t event){
	http_controller_->handleEvent(fd, event);
}

void Bridge::closeHttpConnection(int fd){
	http_controller_->closeConnection(fd);
}

void Bridge::newHttpConnection(int fd){
	http_controller_->addConnection(fd);
}

void Bridge::closeSocketConnection(int fd){
	server_->deleteConnection(fd);
}


void Bridge::updateEvent(int fd, uint32_t event){
	server_->updateEvent(fd, event);
}

void Bridge::setSocketServer(Server* server){
	server_ = server;
}

void Bridge::setHttpController(HttpController* http_controller){
	http_controller_ = http_controller;
}

