#include "HttpHandler.h"
#include "RequestHandler.h"
#include "ResponseHandler.h"
#include <cerrno> 
#include <sys/epoll.h>
#include <iostream>









HttpHandler::HttpHandler(int fd): 
	fd_(fd), 
	state_(State::EPOLLIN), 
	request_handler_(new RequestHandler{}),
	response_handler_(new ResponseHandler{request_handler_}){}


HttpHandler::~HttpHandler(){
	delete request_handler_;
	delete response_handler_;
	request_handler_ = nullptr;
	response_handler_ = nullptr;
}



void HttpHandler::handleEvent(const uint32_t events){
	if (events & (::EPOLLIN|::EPOLLPRI)){
		handleRead();
	}
	if (events & ::EPOLLOUT){
		handleWrite();
	}
}

HttpHandler::State HttpHandler::getState() const{
	return state_;
}

HttpHandler::State HttpHandler::handleRead(){	
	size_t read_bytes = request_handler_->readRequest(fd_);
	int err = request_handler_->getErrno();
	if (err == EAGAIN || err == EWOULDBLOCK){
	//表示当前没有数据可读。在非阻塞 I/O 模式下，这不是一个真正的错误，而是一个提示，告诉你现在没有数据可读，稍后应该再试。
		if (read_bytes == 0){
			return state_ = State::EPOLLIN;
		}
		else{
			request_handler_->parseRequest();
			response_handler_->makeResponse();
			return state_ = State::EPOLLOUT;
		}
	}
	else{
		return state_ = State::CLOSE;//此时连接已经断开，准备断开连接
	}
	
}

//这里的逻辑在write函数中，直到写完了所有数据或者没写完但是errno!=EAGAIN时才终止
//后期可以更改一下，如果写缓冲区满了，可以先不卡在里面等缓冲区清空，可以先做别的事，等有写的空间后再重新调用
HttpHandler::State HttpHandler::handleWrite(){
	response_handler_->write(fd_);
	int err = response_handler_->getErrno();
	if (response_handler_->isWriteable() && (err == EAGAIN || err == EWOULDBLOCK)){
		std::cout << "写缓冲区满,等" << std::endl;
		return state_ = State::EPOLLOUT;
	}
	else if (err == EAGAIN || err == EWOULDBLOCK){
		return state_ = State::EPOLLIN;
	}
	else{
		return state_ = State::CLOSE;	
	}	
}




