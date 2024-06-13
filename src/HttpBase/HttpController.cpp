#include "HttpController.h"
#include "HttpHandler.h"
#include "Bridge.h"
#include "util.h"
#include <sys/epoll.h>
#include <iostream>



HttpController::HttpController(){}

HttpController::~HttpController(){
	for (auto it : http_handlers_){
		delete it.second;
		it.second = nullptr;
	}
	http_handlers_.clear(); 
}


void HttpController::addConnection(int fd){
	ErrorIf(http_handlers_.count(fd), "Try to push a fd which has been exist in httpController");
	writeLog("尝试插入http连接，Fd:" + std::to_string(fd) + "\n");
	try {
		HttpHandler* handler = new HttpHandler{fd};
		//http_handlers_[fd] = handler;
		auto ok = http_handlers_.insert(std::pair<int, HttpHandler*>(fd, handler));
		if (ok.second == true){
			writeLog("http插入成功, Fd:" + std::to_string(fd));
		}
		else{
			std::cout << "http插入失败, Fd:" << fd<< std::endl;	
		}
	} 	
	catch (const std::bad_alloc& e) {
		std::cerr << "Memory allocation for HttpHandler failed: " << e.what() << std::endl;
	}
	ErrorIf(http_handlers_.count(fd) == 0,"htppcontroller error, can't find fd justt inserted");
	writeLog(std::to_string(fd) + "已建立http连接\n");
}

void HttpController::closeConnection(int fd){
	//del_mutex_.lock();
	auto it = http_handlers_.find(fd);
	if (it != http_handlers_.end()){
		HttpHandler* handler = it->second;
		http_handlers_.erase(it);
		delete handler;
		writeLog(std::to_string(fd) + "已删除http连接\n");
	}
	else{
		std::cout << "尝试删除httpfd，但是没有找到" << "Fd:" << fd << std::endl;
	}
	//del_mutex_.unlock();
}


//handle完以后看是不要要增加epollout监听，还是关闭.0是关闭，1是update
void HttpController::handleEvent(int fd, const uint32_t event){
	auto it = http_handlers_.find(fd);
	if (it == http_handlers_.end()){
		std::cout << "http contoller: handle event with invalid fd" << fd << std::endl;
	}
	ErrorIf(it == http_handlers_.end(), "http contoller: handle event with invalid fd");	
	it->second->handleEvent(event);
	int state = it->second->getState();
	if (state == 0){
		writeLog("Httpcontroller尝试关闭, fd:" + std::to_string(fd) + "\n");
		bridge_->closeSocketConnection(fd);
	}
	else if (state == 1){
		writeLog("Httpcontroller尝试将状态更新为EPOLLOUT, fd:" + std::to_string(fd) + "\n");
		bridge_->updateEvent(fd, EPOLLOUT);// | EPOLLET);
	}
	else{
		writeLog("Httpcontroller尝试将状态更新为EPOLLIN, fd:" + std::to_string(fd) + "\n");
		bridge_->updateEvent(fd, EPOLLIN);// | EPOLLET);
	}
}


void HttpController::setBridge(std::shared_ptr<Bridge> bridge){
	bridge_ = bridge;
}





