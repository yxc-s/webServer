#include "Server.h"
#include "Reactor.h"
#include "SocketHandler.h"
#include "Socket.h"
#include "Connection.h"
#include "Acceptor.h"
#include "util.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include "ThreadPool.h"
#include "Bridge.h"
#include<cerrno>


Server::Server(): 
	main_reactor_(nullptr), 
	acceptor_(nullptr),
	port_(static_cast<uint16_t>(-1)),
	use_timer_(false),
	timeout_(-1)
{	writeLog("服务器已启动，开始配置");
	FILE* fp = fopen(socket_config_path, "r");
	ErrorIf(fp == nullptr, "socket config path invalid");
	while (!feof(fp)){
		char line_cstr[1024];
		memset(line_cstr, 0, sizeof(line_cstr));
		fgets(line_cstr, 1024, fp);
		std::string line(line_cstr);
		if (line.empty()){
			break;				//会把最后一个空行也读进来，直接break
		}
		size_t p = line.find(':');
		ErrorIf(p == line.npos, "socket config data invalid");
		std::string key = line.substr(0, p);
		std::string value = line.substr(p + 1);
		if (key == "ip"){
			ip_ = value;
		}
		else if (key == "port"){
			port_ = static_cast<uint16_t>(std::stoi(value));
		}
		else if (key == "usetimer"){
			use_timer_ = (value[0] == '1');
		}
		else if (key == "timeout"){
			timeout_ = std::stoi(value);
		}
	}
	ErrorIf(ip_.empty() || port_ == static_cast<uint16_t> (-1), "socket config data invalid, can't load ip or port");
	fclose(fp);			//这里没有纠错机制，人工确保每个参数名都输入正确
	writeLog("ip:");writeLog(ip_ + "\n");
	writeLog("port:");writeLog(std::to_string(port_) + "\n");
	build();
}

Server::~Server(){
	for (auto&[fd, connection] : connections_){
		if (fd != -1){
			::close(fd);
		}
		delete connection;
	}
	connections_.clear();
	ip_connection_counts_.clear();
}


void Server::listen(){
	ErrorIf(main_reactor_ == nullptr, "acceptor listen error");
	main_reactor_->listen();
}

void Server::newConnection(std::unique_ptr<Socket>&& client_socket){
	int fd = client_socket->getFd();
	writeLog("有新的连接请求，fd = " + std::to_string(fd) + "\n");
	ErrorIf(connections_.count(fd), "connection already exist");
	ErrorIf(fd == -1, "new connection fd error");
	std::string ip = client_socket->getIp();
	int hadrware_size = std::thread::hardware_concurrency();
	int id = 1;//rand()% hadrware_size;
	writeLog("\n该请求将被subreactor ");
	writeLog(std::to_string(id) +  " 处理\n");
	bridge_->newHttpConnection(fd);
	Connection* conn = new Connection(sub_reactors_[id], std::move(client_socket));
	conn->setHandleEventCallback(std::bind(
			&Bridge::handleEvent, 
			bridge_.get(), std::placeholders::_1, std::placeholders::_2
	));
	connections_.insert(std::pair<int, Connection*>(fd, conn));
	conn->push();
}

bool Server::deleteConnection(int fd){
	auto it = connections_.find(fd);
	if (it != connections_.end()){
		bridge_->closeHttpConnection(fd);
		Connection* ptr = it->second;
		connections_.erase(it);
		//为什么这么改？因为子线程删主线程加，虽然fd时放了但是connection中还有，新连接进来就G了
		delete ptr;			//所以保证fd被释放前所有相关资源都被清空了
		ptr = nullptr;
		return true;
	}
	return false;
}

void Server::updateEvent(int fd, uint32_t event){
	auto it = connections_.find(fd);
	//下面的函数防止出现错乱设置，但是好像用不上了
	if (it == connections_.end()){
		logError("try to update a invalid fd in socketServer, Fd:", fd);
		bridge_->closeHttpConnection(fd);			
		return;
	}
	std::string sfd = std::to_string(fd);
	std::string sevent = (event & EPOLLIN ? "EPOLLIN" : "EPOLLOUT");
	writeLog((sfd + "更新为:" + sevent + "\n"));
	it->second->updateEvent(event);
}

void Server::setBridge(std::shared_ptr<Bridge> bridge){
	bridge_ = bridge;
	writeLog("bridege设置完成\n");
}



void Server::build(){
	main_reactor_ = std::make_shared<Reactor>();
	acceptor_ = std::make_unique<Acceptor>(main_reactor_, ip_.c_str(), port_);
	std::function<void(std::unique_ptr<Socket>&&)> conn_function = 
		std::bind(&Server::newConnection, this, std::placeholders::_1);
	acceptor_->setNewConnectionCallback(conn_function);
	writeLog("Acceptor设置完成\n");
	int temp_size = static_cast<int> (std::thread::hardware_concurrency());
	for (int i = 0; i < temp_size; ++i){
		sub_reactors_.emplace_back(std::make_shared<Reactor> ());
		if (use_timer_){		//如果用了timer,设置回调函数跟timeout
			sub_reactors_.back()->setHeapTimerCallback(
				std::bind(&Server::deleteConnection, this, std::placeholders::_1));
			sub_reactors_.back()->useHeapTimer(timeout_);
			sub_reactors_.back()->id_ = i;
		}
	}
	thread_pool_ = std::make_unique<ThreadPool> (temp_size);
	writeLog("线程池创建完成\n");
	for (int i = 0; i < temp_size; ++i){		
		thread_pool_->push(std::bind(&Reactor::listen, sub_reactors_[i]));		//把subreacotr的listen函数放到线程池，一个线程一个
	}
	writeLog("subreactor的listen函数已经加入到线程池分配给线程\n");
}

uint16_t Server::ipCounts(std::string& ip){
	auto it = ip_connection_counts_.find(ip);
	if (it == ip_connection_counts_.end()){
		ip_connection_counts_[ip] = 1;
	}
	return ip_connection_counts_[ip];
}


