#include "thread_safe_queue.h"
#include <iostream>


MyTask::MyTask(size_t rank, size_t job) : rank(rank), job(job)  {};

void MyTask::execute()
{
	std::this_thread::sleep_for(std::chrono::seconds(job*job));
	std::cout << "rank: " << rank << " job " << job << " end job " << std::endl;
}

size_t MyTask::get_job()
{
	return job;
}

size_t MyTask::get_rank()
{
	return rank;
}

void ThreadSafeQueue::push(TaskPtr task) 
{
	std::lock_guard<std::mutex> lock(mutex);
	tasks.push(task);
	cond.notify_one();
}

TaskPtr ThreadSafeQueue::pop_or_wait() 
{
	std::unique_lock<std::mutex> lock(mutex);
	cond.wait(lock, [this] {return !tasks.empty(); });
	TaskPtr task = tasks.front();
	tasks.pop();
	return task;
}

bool ThreadSafeQueue::empty()
{
	std::unique_lock<std::mutex> lock(mutex);
	return tasks.empty();
}

int ThreadSafeQueue::size()
{
	std::unique_lock<std::mutex> lock(mutex);
	return tasks.size();
}
