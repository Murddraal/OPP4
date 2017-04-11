#pragma once
#include <queue>
#include <memory>
#include <thread>

class ThreadSafeQueue;
class Task;
typedef std::shared_ptr<Task> TaskPtr;

class ThreadPool
{
private:
	
	void thread_func();
	std::unique_ptr<ThreadSafeQueue> task_queue;
	std::vector<std::thread*> threads;

public:
	ThreadPool(size_t);
	ThreadPool();
	~ThreadPool();
	
	void start(int num_of_threads);
	void stop();
	void submit(TaskPtr task);

};
