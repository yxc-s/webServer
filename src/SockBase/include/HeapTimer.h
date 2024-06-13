#pragma once

#include <vector>
#include <chrono>
#include <functional>
#include <map>
#include <mutex>
#include <queue>

//堆节点结构
struct Node{
	int fd;
	std::chrono::high_resolution_clock::time_point expire_time;
	Node(int fd_, std::chrono::high_resolution_clock::time_point expire_time_):
		fd(fd_), expire_time(expire_time_){}
	bool operator >(const Node& other) const{
		return expire_time > other.expire_time;
	}
};




class MinHeap {
public:
	//默认reserve一个堆大小（避免频繁操作内存出现坏内存，非常有用）
	MinHeap(size_t initialCapacity = 1024);

	~MinHeap();


	//被subreactor线程调用，用于在处理连接事件前或者刚建立连接时加入到堆中进行计时
	void emplace(int fd, std::chrono::high_resolution_clock::time_point timeout);

	//fd已经存在，更新该fd的倒计时时间
	void update(int fd, std::chrono::high_resolution_clock::time_point new_expire_time);

	//弹出堆顶节点
	void pop();

	//删除堆中指定fd的节点
	void erase(int fd);

	//获得堆顶节点，在堆清理过期时间节点时使用
	Node top() const;

	//堆是否为空
	bool empty() const;

	//获得堆大小
	size_t size() const;
	

private:
	std::vector<Node> heap_;
	std::map<int, size_t> hash_;
	

	void shiftUp(size_t cur_pos);
	void shiftDown(size_t cur_pos);
};




class Queue{
public:
	Queue(int initialSize = 20);

	~Queue();

	void push(int fd);
	
	int front() const;
	
	void pop();

	bool empty() const;


private:
	std::vector<int> queue_;
	size_t front_;
	size_t rear_;
};







class HeapTimer{
public:
	HeapTimer();

	~HeapTimer();
	
	//主线程调用，为了资源安全，先加到队列中，由子线程处理
	void pushToQueue(int fd);
	
	//在执行本次检测到的事件前，先处理主线程检测到的要加入到timer中的fd
	void handleFdQueue();
	
	//subreactor建立新连接或者处理该连接上的事件之前调用，开启或者重置堆计时
	void push(int fd, int timeout);
	
	//删除堆中指定fd节点，在外部检测到该fd已经关闭连接时由server中的删除连接函数调用
	void erase(int fd);
	
	//清理过期节点，同时调用server中的删除连接函数
	void removeExpiry();
	
	//获得堆顶倒计时时间，由subreactor调用，用于设置下一次epoll的监听时间
	int getNextTick();	

	//在server设置使用堆计时器时由subreactor设置
	void setTimeout(int timeout);
	
	//在server设置使用堆时由subreactor设置
	void setDeleteCallback(std::function<bool(int)>& cb);

	
private:
	std::function<bool(int)> delete_callback_;
	Queue fd_queue_;
	MinHeap heap_;
	int timeout_;
};




