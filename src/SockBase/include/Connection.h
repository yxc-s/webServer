#pragma once
#include <memory>
#include <functional>

class Reactor;
class Socket;
class SocketHandler;


class Connection{
public:
	//在检测到新的连接时，在server中的函数中构造
	Connection(std::shared_ptr<Reactor> reactor, std::unique_ptr<Socket>&& socket);

	~Connection();

	
	//这个函数目前已经不被任何接口调用，删除connection的api由timer（超时删除），外部调用（socket处理事件时检测到socket断开）
	void deleteConnection();
	
	//调用绑定的外部回调处理函数
	void handleEvent();

	//被创建时调用，回调函数搜索server中的删除连接函数
	void setDeleteConnectionCallback(std::function<bool(int)> cb);
	
	//server通过调用外部接口设置的回调函数，用于处理socket上的监听到的事件
	void setHandleEventCallback(std::function<void(int, uint32_t)> cb);
	
	//更新该连接中监听的事件类型，将事件替换为event
	void updateEvent(uint32_t event);

	//由server调用
	void push() const;
	
	//目前也不被任何api调用
	int getState() const;
	
	int getFd() const;
	
	uint16_t getPort() const;

	const char* getIp() const;
	

private:
	std::unique_ptr<SocketHandler> socket_handler_;
	std::shared_ptr<Reactor> reactor_;
	//std::function<bool(int)> delete_connection_callback_;	//连接断开时的回调函数
	std::function<void(int, uint32_t)> handle_event_callback_;
	
	enum State {
		Invalid = 0,
		Connected,	
		Closed,
	};
	State state_;
};


