#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

class Task 
{
public:
	virtual ~Task() {}

	virtual void execute() = 0;
	virtual size_t get_rank() = 0;
	virtual size_t get_job() = 0;
};

class MyTask : public Task
{
public:
	MyTask(size_t rank, size_t job);

	void execute();
	size_t get_rank();
	size_t get_job();

private:
	size_t rank, job;
};

typedef std::shared_ptr<Task> TaskPtr;
/**
* Thread safe queue of tasks.
*/
class ThreadSafeQueue 
{
public:
	ThreadSafeQueue() {}
	~ThreadSafeQueue() {}

	/// Add task to the queue.
	void push(TaskPtr task);

	/// Get task from the queue.
	/// Wait for a task if the queue is empty.
	TaskPtr pop_or_wait();
	
	bool empty();
	int size();

private:
	std::queue<TaskPtr> tasks;
	std::mutex mutex;
	std::condition_variable cond;
};