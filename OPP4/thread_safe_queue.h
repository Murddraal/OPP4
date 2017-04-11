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
	virtual size_t get_data() = 0;
};

class MyTask : public Task
{
public:
	MyTask(size_t time);

	void execute();
	size_t get_data();

private:
	size_t time;
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