#include "thread_pool.h"
#include "thread_safe_queue.h"
#include "task.h"
#include <exception>

ThreadPool::ThreadPool() 
{
	task_queue = std::make_unique<ThreadSafeQueue>();
}

ThreadPool::ThreadPool(size_t num_of_threads) : ThreadPool() 
{
	start(num_of_threads);
}

ThreadPool::~ThreadPool() 
{
	stop();
}

void ThreadPool::start(int num_of_threads) 
{
	if (!threads.empty()) 
	{
		throw std::runtime_error("Thread pool is started already!");
	}
	for (int i = 0; i < num_of_threads; i++) 
	{
		threads.push_back(new std::thread(&ThreadPool::thread_func, this));
	}
}

void ThreadPool::stop() 
{
	// Push empty task for each thread to signal shutdown.
	for (auto &thread : threads) 
	{
		submit(TaskPtr());
	}
	for (auto &thread : threads) 
	{
		thread->join();
	}
	threads.clear();
}

void ThreadPool::submit(TaskPtr task) 
{
	task_queue->push(task);
}

void ThreadPool::thread_func() 
{
	while (true) 
	{
		TaskPtr task = task_queue->pop_or_wait();
		// Exit if received empty task.
		if (!task) 
		{
			break;
		}
		task->execute();
	}
}
