#pragma once

#include <functional>
#include <memory>
#include <cstdint>

class Socket;
class Reactor;


class SocketHandler{
public:
	SocketHandler();
	
	//没有被使用
	explicit SocketHandler(std::unique_ptr<Socket>&& socket);
	
	//Acceptor使用，然后调用set函数设置socket
	explicit SocketHandler(std::shared_ptr<Reactor>& reactor);
	
	//connection采用这种创建方式，直接分配好reactor跟对应的socket
	SocketHandler(std::shared_ptr<Reactor>& reactor, std::unique_ptr<Socket>&& socket);
	
	~SocketHandler();
	
	//设置socket，acceptor使用一次
	void setSocket(std::unique_ptr<Socket>&& socket);
	
	//没被使用
	void setReactor(std::shared_ptr<Reactor>&& reactor);
	
	//设置回调函数，创建后被设置
	void setCallback(std::function<void()>);
	
	//Epoll监听到事件时设置，用于向外部函数传递监听到的事件类型
	void setCurrentEvents(uint32_t current_events);
	
	//被Epoll监听时调用，说明该handler已经被监听
	void setInEpoll();

	//被Epoll移除时调用，说明handler不在监听表中
	void removeFromEpoll();
	
	//是否在Epoll监听表
	bool isInEpoll() const;
	
	//将该handler加入到对应的subreactor中的epoll进行监听，事件是EPOLLIN|EPOLLET
	void push();
	
	//停止监听该handler，从subreactor的timer跟epoll中移除，目前只在析构函数中调用
	void pop();
	
	//调用回调函数，监听到事件时调用。目前绑定的是connection或在acceptor中的函数。
	void handleEvent() const;
	
	//接受，返回一个unique_ptr,存储的是新的连接client的信息
	std::unique_ptr<Socket> accept() const;	
	
	//外部函数通过bridge->server->connection->sockethandler->subreactor->epoll(MOD)
	void updateEvent(uint32_t event);

	//获取事件
	uint32_t getEvents() const;
	
	//获取本次监听到的事件
	uint32_t getCurrentEvents() const;
	
	int getFd() constm,;

	const char* getIp() const;
	
	uint16_t getPort() const;

	void useEdgeTriggered();

	void useLevelTriggered();

	int fd_;
//	void close() const;


private:
	std::unique_ptr<Socket> socket_;
	std::shared_ptr<Reactor> reactor_;
	std::function<void()> read_callback_;
	uint32_t events_;
	uint32_t current_events_;
	bool in_epoll_;
};                                                                                                                                                                                                                                                                                  

