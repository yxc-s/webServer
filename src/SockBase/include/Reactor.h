#pragma once

#include <sys/epoll.h>
#include <cstring>
#include <vector>
#include <memory>
#include <functional>
#include <set>
#include <bitset>
//单个epoll事件表最多能监听的事件数
constexpr int EVENT_SIZE = 1024;

class Reactor;
class Connection;
class Socket;
class SocketHandler;
class HeapTimer;




class Epoll{
public:
	Epoll();
	 
	~Epoll();

	//加入事件到epoll表中
	void push(SocketHandler* handler);
	
	void update(SocketHandler* handler);
	
	//从epoll表中移除handler，由subreactor调用（sockethandler析构函数调用subreactor)
	void pop(SocketHandler* handler);
	
	//对于acceptor,timeout = -1，如果有堆timer，则在配置文件中配置timeout时间，超时自动断开
	std::vector<SocketHandler*> wait(int timeout = -1);


private:
	int epoll_fd_;
	struct epoll_event event_, events_[EVENT_SIZE];
};



class Reactor{
public:
	
	Reactor();
   	
	~Reactor();
   	
	//当sockethandler准备好后，一定由主线程调用!!
	void push(SocketHandler* handler);
   	
	//sockethandler析构函数调用，从epoll表中移除，并且从堆中移除（如果使用了堆）
	void pop(SocketHandler* handler);
	
	void update(SocketHandler* handler) const;
	
	//调用Epoll类进行监听
	void listen();
	
	//开始使用堆，由server在创建subreactor时调用
	void setHeapTimerCallback(std::function<bool(int)> cb);

	//开始使用堆，由server在创建subreactor时调用
	void useHeapTimer(int timeout);
	
	int id_ = -1;
private:
	std::unique_ptr<Epoll> epoll_;
	std::unique_ptr<HeapTimer> heap_timer_;
	std::function<bool(int)> heap_timer_callback_;
	std::set<SocketHandler*> has_deleted_;
	int timeout_ = -1;
	bool use_timer_ = false;
	
};







