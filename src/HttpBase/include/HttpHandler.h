#pragma once
#include <cstdint>




class RequestHandler;
class ResponseHandler;

class HttpHandler{
public:
	enum State{
		CLOSE = 0,
		EPOLLOUT,
		EPOLLIN,
		
	};
	
	HttpHandler(int fd);

	~HttpHandler();
		

	//打算在这里设置一个返回值，如果事件处理失败返回false则直接删除链接
	void handleEvent(const uint32_t events);
	
	HttpHandler::State getState() const;
	
	
private:
	int fd_;
	
	State state_;
	RequestHandler* request_handler_;
	ResponseHandler* response_handler_;

	State handleRead();

	State handleWrite();
};	







