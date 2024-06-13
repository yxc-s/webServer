#include "Reactor.h"
#include "util.h"
#include "Socket.h"
#include "Connection.h"
#include "SocketHandler.h"
#include "HeapTimer.h"
#include <iostream>
#include <sstream>





Epoll::Epoll(){
	epoll_fd_ = epoll_create1(0);
	ErrorIf(epoll_fd_ == -1, "epoll create error");
	memset(&event_, 0, sizeof(event_));
	memset(events_, 0, sizeof(events_));
}

Epoll::~Epoll(){}


void Epoll::push(SocketHandler* handler){
	memset(&event_, 0, sizeof(event_));
	event_.data.ptr = handler;
	event_.events = handler->getEvents();
	if (handler->isInEpoll() == false){
		ErrorIf(epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, handler->getFd(), &event_) == -1, "epoll push error");
		handler->setInEpoll();
		ErrorIf(handler->isInEpoll() == false, "epoll push error");
		writeLog("主线程插入, fd" + std::to_string(handler->getFd()) + "地址:" + getHexAddr(handler) + "\n");
	}   
	else{
		logError("update a invalid fd, fd:", std::to_string(handler->getFd()));
	}
}

void Epoll::update(SocketHandler* handler){
	memset(&event_, 0, sizeof(event_));
	event_.data.ptr = handler;
	event_.events = handler->getEvents();
	ErrorIf(epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, handler->getFd(), &event_) == -1, "epoll update error");
	writeLog("子线程更新, fd" + std::to_string(handler->getFd()) + "地址:" + getHexAddr(handler)+ "\n");
}

void Epoll::pop(SocketHandler* handler){
	if (handler->isInEpoll()){
		ErrorIf(epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, handler->getFd(), nullptr) == -1, 
			"try to delete a invalid socket handler");
		handler->removeFromEpoll();
		writeLog("子线程删除, fd" + std::to_string(handler->getFd()) + "地址:" +  getHexAddr(handler) + "\n");
		ErrorIf(epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, handler->getFd(), nullptr) != -1, "epoll表仍然检测到fd");
		writeLog("确保fd已被删除" + std::to_string(handler->getFd()) + "\n");
	} 
	else{
		std::cout << "尝试从epoll中移除Fd" << handler->getFd() << ",但是handler的inepoll是false";
	}
}

std::vector<SocketHandler*> Epoll::wait(int timeout){
	memset(events_, 0, sizeof(events_));
	std::vector<SocketHandler*> activity;
	int nfds = epoll_wait(epoll_fd_, events_, EVENT_SIZE, timeout);
	ErrorIf(nfds == -1, "epoll wait error");
	for (int i = 0; i < nfds; ++i){
		activity.emplace_back(reinterpret_cast<SocketHandler*> (events_[i].data.ptr));
		activity.back()->setCurrentEvents(events_[i].events);
		 SocketHandler* handler = static_cast<SocketHandler*>(events_[i].data.ptr);
	}
	return activity;
}























Reactor::Reactor(): 
	epoll_(std::make_unique<Epoll>()), 
	heap_timer_(std::make_unique<HeapTimer>()){}

Reactor::~Reactor(){}


void Reactor::push(SocketHandler* handler){
	if (use_timer_){
		heap_timer_->pushToQueue(handler->getFd());
	}
	epoll_->push(handler);
}

void Reactor::update(SocketHandler* handler) const{
	epoll_->update(handler);
}

void Reactor::pop(SocketHandler* handler){
	epoll_->pop(handler);
	if (use_timer_){
		heap_timer_->erase(handler->getFd());
	}
	has_deleted_.insert(handler);
}

void Reactor::setHeapTimerCallback(std::function<bool(int)> cb){
	heap_timer_callback_ = cb;
}

void Reactor::useHeapTimer(int timeout){
	use_timer_ = true;
	timeout_ = timeout;
	heap_timer_->setTimeout(timeout_);
	heap_timer_->setDeleteCallback(heap_timer_callback_);
};

void handleQUeue(){
	
}

void Reactor::listen(){
	int cur_timeout = -1;
	while (true){
		if (use_timer_ == true){
			cur_timeout = heap_timer_->getNextTick();
		}
		has_deleted_.clear();
		auto activity = epoll_->wait(cur_timeout);
		heap_timer_->handleFdQueue();
		for (const auto& cur_activity : activity){
			if (use_timer_){
				heap_timer_->push(cur_activity->getFd(), timeout_);
			}
			if (has_deleted_.count(cur_activity)){
				logError("尝试运行已删除的地址:", getHexAddr(cur_activity));
				continue;
			}
			int fd = cur_activity->getFd();
			writeLog("线程:", std::to_string(id_), "处理Fd", std::to_string(fd), "地址:", getHexAddr(cur_activity));
			cur_activity->handleEvent();
		}
	}
}















