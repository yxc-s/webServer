#pragma once

#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <memory>


class ThreadPool{
public:
	//线程池大小默认为处理器数量,并且为每个线程绑定getTask()函数
	explicit ThreadPool(unsigned int size = std::thread::hardware_concurrency());

	~ThreadPool();
	

	//用来向线程池中增加reactor的listen函数，因为是模板函数，所以在头文件中实现
	template<class T, class... Args>
	auto push(T&& function, Args&& ... args) -> std::future<typename std::result_of<T(Args...)>::type>;


private:
	std::vector<std::thread> threads_;
	std::queue<std::function<void()>> tasks_;
	std::mutex task_mutex_;
	std::condition_variable condition_variable_;
	bool stop_;


	void getTask();		
} ;






template<class T, class...Args>
auto ThreadPool::push(T&& function, Args&& ...args) -> std::future<typename std::result_of<T(Args...)>::type>{
	using return_type = typename std::result_of<T(Args...)>::type;
//std::packaged_task 是一个模板类，将一个可调用对象（如函数、lambda 表达式）封装起来，可以异步获取函数执行结果。
	auto task = 
		std::make_shared<std::packaged_task<return_type()>> (std::bind(std::forward<T>(function), std::forward<Args>(args)...));
//完美转发的主要作用是确保在函数模板中，参数可以保持其原始的值类别（左值或右值），并且正确地转发给其他函数或对象。这在模板编程中特别重要，因为它允许模板函数处理各种类型的参数，同时保留这些参数的左值或右值属性。
	std::future<return_type> rst = task->get_future();
	{
		std::unique_lock<std::mutex> lock(task_mutex_);
		tasks_.emplace([task](){(*task)();});		//*task 来解引用
	}
	//通知（唤醒）一个正在等待的线程,这里线程数刚好等于子reactor数，所以每个子线程一个reactor
	condition_variable_.notify_one();	
	return rst;
}


