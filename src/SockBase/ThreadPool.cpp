#include "ThreadPool.h"




ThreadPool::ThreadPool(unsigned int size) : stop_(false){
	for (unsigned int i = 0; i < size; ++i){
		threads_.emplace_back(std::thread(&ThreadPool::getTask, this));
	}
}


ThreadPool::~ThreadPool(){
	{
		std::unique_lock<std::mutex> lock(task_mutex_);
		stop_ = true;
	}
		condition_variable_.notify_all();
	//将这两个语句组合在一起使用是一种常见的模式，它确保只有当线程 th 是可加入的时候，才调用 join。这种检查是必要的，因为对一个不可加入的线程调用 join 会导致程序崩溃。
	for (std::thread &trd : threads_) {
		if (trd.joinable()) {	//这个函数检查线程 th 是否是一个可加入的线程。一个线程是可加入的，如果它已经被启动（即正在执行或执行完毕，但还没有加入到任何线程）。如果一个线程已经被加入，或者没有被正确启动（如默认构造的 std::thread 对象），那么它不是可加入的。
		  	trd.join();	//这个函数用于阻塞当前线程（通常是主线程），直到 th 完成其执行。调用 join 是确保程序按照预期顺序执行的一种方式。在多线程编程中，如果您创建了一个线程来执行某项任务，通常需要在某个点上等待该任务完成，特别是在程序退出之前，以确保所有资源都被正确清理并且没有线程被意外终止。
		}
	}
}


void ThreadPool::getTask(){
  while (true){
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(task_mutex_);
			condition_variable_.wait(lock, [this]{return stop_ || !tasks_.empty();});		
			if (stop_ && tasks_.empty()){
				return;
			}
			task = tasks_.front();
			tasks_.pop();
		}
			if (task){
				task();	
			}
		}
}

