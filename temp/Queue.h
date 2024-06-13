#pragma once
#include <vector>
#include <cassert>




template<typename T>
class Queue{
public:
	Queue(int initialSize = 1024);

	~Queue();

	void push(T fd);
	
	T front() const;
	
	void pop();

	bool empty() const;


private:
	std::vector<T> queue_;
	size_t front_;
	size_t rear_;
};




Queue<T>::Queue::Queue(int initialSize):
	front_(0),
	rear_(0)
{
	queue_.resize(initialSize);
}

Queue<T>::Queue::~Queue(){}


void Queue<T>::Queue::push(T value){
	if ((rear_ + 1) % queue_.size() == front_){
		std::cout << "主线程调用的timer中的fdqueue已满，内存扩大" << queue_.size() + 1 << std::endl;
		queue_.resize(queue_.size() * 2);
	}
	queue_[rear_ ++] = value;
	rear_ %= queue_.size();
}

T Queue<T>::Queue::front() const{
	assert(front_ != rear_);
	return queue_[front_];
}

void Queue<T>::Queue::pop(){
	assert(front_ != rear_);
	front_ ++;
	front_ %= queue_.size();
}

bool Queue<T>::Queue::empty() const{
	return front_ == rear_;
}

