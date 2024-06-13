#pragma once
#include <memory>
#include <map>
#include <functional>
#include <cstdint>
#include <string>
#include <unordered_map>


//socketbase配置文件的路径，用于读取ip，port，是否使用堆以及堆timeout
constexpr const char* socket_config_path = "../../config/socketbaseconfig.ini";



class Reactor;
class SocketHandler;
class Connection;
class Acceptor;
class Socket;
class ThreadPool;
class Bridge;

class Server{
public:
	Server();

	~Server();
	

	//acceptor开始接受新连接，主线程调用
	void listen();

	//acceptor检测到有事件发生时的回调函数，用于创建客户端connection连接
	void newConnection(std::unique_ptr<Socket>&& client_socket);	
	
	//connection， 堆的回调函数，也可能被bridege调用
	bool deleteConnection(int);	
	
	//由外部函数调用，用于更改某个fd的监听事件
	void updateEvent(int fd, uint32_t event);

	//设置桥，目前的功能是与http库做连接
	void setBridge(std::shared_ptr<Bridge> bridge);

	
private:
	std::shared_ptr<Reactor> main_reactor_;			//main reactor,只监听服务器socket，接受新的连接
	std::map<int, Connection*> connections_;			//存储了每个连接，fd是key，connection的地址是value。
	std::unique_ptr<Acceptor> acceptor_;				//接受新连接
	std::vector<std::shared_ptr<Reactor>> sub_reactors_;	//子reactor
	std::unique_ptr<ThreadPool> thread_pool_;			//线程池
	std::shared_ptr<Bridge> bridge_;				//桥，用于与http通信
	std::unordered_map<std::string, uint16_t> ip_connection_counts_;		//目前没有投入使用，为了防止服务器被恶意连接设置的一个构想
	
	std::string ip_;
	uint16_t port_;
	bool use_timer_;
	int timeout_;

	void build();		
	uint16_t ipCounts(std::string& ip);
};


