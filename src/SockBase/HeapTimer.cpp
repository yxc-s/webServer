#include <cassert>
#include <algorithm> 
#include "HeapTimer.h"
#include "util.h"
#include <iostream>



MinHeap::MinHeap(size_t initialCapacity){
	heap_.reserve(initialCapacity);
}
	
MinHeap::~MinHeap(){}


void MinHeap::emplace(int fd, std::chrono::high_resolution_clock::time_point timeout) {
	if (hash_.count(fd)){
		update(fd, timeout);	
	}
	else{
		heap_.emplace_back(fd, timeout);
		hash_[fd] = heap_.size() - 1;
		shiftUp(heap_.size() - 1);
	}
}

void MinHeap::update(int fd, std::chrono::high_resolution_clock::time_point new_expire_time){
	assert(hash_.count(fd));
	size_t index = hash_.find(fd)->second;
	heap_[index].expire_time = new_expire_time;
	shiftDown(index);
}

void MinHeap::pop() {
	assert(!empty());
	auto it = hash_.find(heap_[0].fd);
	//assert(it != hash_.end());
	if (it == hash_.end()){
		std::cout << "timer:pop调用删除"<<"时为空!" << std::endl;return;
	}
	hash_.erase(it);
	std::swap(heap_[0], heap_.back());
	heap_.pop_back();
	if (empty()){
		return;
	}
	shiftDown(0);
}

void MinHeap::erase(int fd){
	auto it = hash_.find(fd);
	//assert(it != hash_.end());
	if (it == hash_.end()){
		std::cout << "timer:erase调用删除Fd" << fd << "时为空!" << std::endl;
		return;
	}
	else if (empty()){
		hash_.clear();
		std::cout << "尝试删除fd" << fd<< "时heap是空的，已清空hash" << std::endl;		
		return;
	}
	size_t index = it->second;
	std::swap(heap_.back(), heap_[index]);
	hash_.erase(it);
	heap_.pop_back();
	if (empty() || index >= heap_.size()){
		ErrorIf(index != heap_.size(), "wrong index out of range in timer");
		return;
	}
	shiftDown(index);
}

Node MinHeap::top() const {
	ErrorIf(empty(), "try to get top value from a empty MinHeap");
	return heap_[0];
}

bool MinHeap::empty() const {
	return heap_.empty();
}

size_t MinHeap::size() const {
	return heap_.size();
}


void MinHeap::shiftUp(size_t cur_pos) {
	while (cur_pos > 0) {
		size_t par_pos = (cur_pos - 1) / 2;
		if (heap_[par_pos] > heap_[cur_pos]) {
			std::swap(heap_[cur_pos], heap_[par_pos]);
			hash_[heap_[cur_pos].fd] = cur_pos;
			hash_[heap_[par_pos].fd] = cur_pos;
			cur_pos = par_pos;
		}
		else {
			break;
		}
	}
	hash_[heap_[cur_pos].fd] = cur_pos;
}

void MinHeap::shiftDown(size_t cur_pos) {
	while (2 * cur_pos + 1 < heap_.size()) {
		size_t chi_pos = 2 * cur_pos + 1;
		if (chi_pos + 1 < heap_.size() && heap_[chi_pos] > heap_[chi_pos + 1]) {
			chi_pos ++;
		}
		if (heap_[cur_pos] > heap_[chi_pos]) {
			std::swap(heap_[cur_pos], heap_[chi_pos]);
			hash_[heap_[cur_pos].fd] = cur_pos;
			hash_[heap_[chi_pos].fd] = cur_pos;
			cur_pos = chi_pos;
		}
		else {
			break;
		}
	}
	hash_[heap_[cur_pos].fd] = cur_pos;
}





Queue::Queue(int initialSize):
	front_(0),
	rear_(0)
{
	queue_.resize(initialSize);
}

Queue::~Queue(){}


void Queue::push(int fd){
	if ((rear_ + 1) % queue_.size() == front_){
		std::cout << "主线程调用的timer中的fdqueue已满，内存扩大" << queue_.size() + 1 << std::endl;
		queue_.resize(queue_.size() * 2);
	}
	queue_[rear_ ++] = fd;
	rear_ %= queue_.size();
}

int Queue::front() const{
	assert(front_ != rear_);
	return queue_[front_];
}

void Queue::pop(){
	assert(front_ != rear_);
	front_ ++;
	front_ %= queue_.size();
}

bool Queue::empty() const{
	return front_ == rear_;
}





HeapTimer::HeapTimer(): 
	timeout_(-1){}

HeapTimer::~HeapTimer(){}


void HeapTimer::push(int fd, int timeout){
	heap_.emplace(fd, std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(timeout));
}

void HeapTimer::pushToQueue(int fd){
	fd_queue_.push(fd);
}

void HeapTimer::handleFdQueue(){
	while (!fd_queue_.empty()){
		int fd = fd_queue_.front();
		fd_queue_.pop();
		push(fd, timeout_);
	}
}

void HeapTimer::erase(int fd){
	heap_.erase(fd);
}

void HeapTimer::removeExpiry(){		
	while (!heap_.empty()){
		auto temp = heap_.top();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(
		temp.expire_time - std::chrono::high_resolution_clock::now()).count() <= 0){
			int fd = temp.fd;
			if (delete_callback_(fd)){
				//之前是temp.fd，但是如果在handler的析构函数中删掉了这个节点，temp就空了
				std::cout << "fd:" << fd << "超时断开" << std::endl;
			}
			//heap_.pop();//	为了保证资源安全，pop操作将在deleteconnection函数中调用
		}
		else{
			break;	
		}
	}
}

int HeapTimer::getNextTick(){
	removeExpiry();
	int res = timeout_;
	if (!heap_.empty()){
		res = std::chrono::duration_cast<std::chrono::milliseconds>(
			heap_.top().expire_time - std::chrono::high_resolution_clock::now()).count();
	}
	return res;
}


void HeapTimer::setTimeout(int timeout){
	timeout_ = timeout;
}

void HeapTimer::setDeleteCallback(std::function<bool(int)>& cb){
	delete_callback_ = cb;
}









