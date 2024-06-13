#pragma once
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <memory>



class Address{
public:
	Address();

	//Socket构造Acceptor的时候会调用这个
	Address(const char* ip, uint16_t port);

	//Acceptor接受新Socket的时候会这样构造
	explicit Address(struct sockaddr_in addr);
	
	~Address();
	
	
	const char* getIp() const;

	uint16_t getPort () const;
	
	const sockaddr* getAddr() const;
	
	socklen_t getAddrSize() const;
	

private:
	sockaddr_in addr_;
	std::string ip_;
	uint16_t port_;
	socklen_t addr_len_;
};




class Socket{
public:
	Socket();
	
	//Acceptor目前采用这种初始化方式，在socket内部自动创建Address成员
	Socket (const char* ip, uint16_t port);
	
	//Socket接受新连接请求的时候创建好client的fd跟addr，直接构造
	Socket (int fd, std::unique_ptr<Address> addr);
	
	//会检查fd是不是-1，如果不是-1就直接关闭了，在上层逻辑中不需要手动关闭
	~Socket();

	
	void bind() const;

	void listen() const;

	void connect() const;
	
	std::unique_ptr<Socket> accept() const;
	
	//目前没有需求对这个api进行调用，都是断开连接时析构函数自动关闭
	void close() const;

	void setFd(int fd);
	
	void setAddr(std::unique_ptr<Address>&& addr);

	void setNonBlock();

	int getFd() const;
	
	uint16_t getPort () const;

	const char* getIp() const;
	

private:
	int fd_;
	std::unique_ptr<Address> addr_;
};


