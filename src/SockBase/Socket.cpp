#include <unistd.h>
#include <fcntl.h>
#include "Socket.h"
#include "util.h"

#include <iostream>



Address::Address() : port_(0), addr_len_(sizeof(addr_)){
	memset(&addr_, 0, sizeof(addr_));
}

Address::Address(const char* ip, uint16_t port): ip_(ip), port_(port), addr_len_(sizeof(addr_)){
	memset(&addr_, 0, sizeof(addr_));
	addr_.sin_family = AF_INET;
	addr_.sin_addr.s_addr = inet_addr(ip_.c_str());
	addr_.sin_port = htons(port_);
}

Address::Address(struct sockaddr_in addr): addr_(addr), ip_(std::string(inet_ntoa(addr_.sin_addr))), port_(ntohs(addr_.sin_port)), addr_len_(sizeof(addr_)){}

Address::~Address(){}


const char* Address::getIp() const{
	return ip_.c_str();
}

uint16_t Address::getPort() const{
	return port_;
}

const sockaddr* Address::getAddr() const{
	return reinterpret_cast<const sockaddr*>(&addr_);
}

socklen_t Address::getAddrSize() const{
	return addr_len_;
}















Socket::Socket(){
	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	ErrorIf(fd_ == -1, "socket create error");
}

Socket::Socket(const char* ip, uint16_t port){
	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	ErrorIf(fd_ == -1, "socket create error");
	addr_ = std::make_unique<Address> (ip, port);
}

Socket::Socket(int fd, std::unique_ptr<Address> addr): 
	fd_(fd), 
	addr_(std::move(addr))
{
	ErrorIf(fd_ == -1, "invalid value of socket while creaing Socket class for client");
}

Socket::~Socket(){
	if (fd_ != -1){
		::close(fd_);
		writeLog(std::to_string(fd_) + "文件描述符已关闭\n");
		fd_ = -1;
	}
}


void Socket::bind() const{
	ErrorIf(::bind(fd_, addr_->getAddr(), addr_->getAddrSize()) == -1, "socket bind error");
}

void Socket::listen() const{
	ErrorIf(::listen(fd_, SOMAXCONN) == -1, "socket listen error");
}

std::unique_ptr<Socket> Socket::accept() const{
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	int client_fd = ::accept(fd_, reinterpret_cast<sockaddr*>(&addr), &addr_len);
	ErrorIf(client_fd == -1, "invalid fd for a new client connection");
	writeLog("Socket：接受到客户端Fd:" + std::to_string(client_fd) + "\n");
	return std::make_unique<Socket> (client_fd, std::move(std::make_unique<Address>(addr)));
}

void Socket::connect() const{
	ErrorIf(::connect(fd_, addr_->getAddr(), addr_->getAddrSize()) == -1, "socket connect error");
}

void Socket::close() const{
	ErrorIf(fd_ == -1, "socket close error, try to close fd with value of -1");
	::close(fd_);
}


void Socket::setFd(int fd){
	if (fd != -1){
		::close(fd_);
	}
	fd_ = fd;
}

void Socket::setAddr(std::unique_ptr<Address>&& addr){
	addr_ = std::move(addr);
}

void Socket::setNonBlock(){
	fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK);
}

int Socket::getFd() const{
	return fd_;
}

uint16_t Socket::getPort() const{
	return addr_->getPort();
}

const char* Socket::getIp() const{
	return addr_->getIp();
}














