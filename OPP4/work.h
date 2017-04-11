#pragma once
#include <memory>
#include <mpi.h>
#include <mutex>
#include "thread_safe_queue.h"
#include <thread>

class balance
{
public:
	balance(const int &q_size, const int &rank, const int &size);
	~balance();
	void do_work();
	void dispetcher();
	void provider();
	TaskPtr get_job();
	void show_iterations();

private:
	int is_provider = 1;
	bool can_iter = false;

	int size, rank, iterations;
	int tag_ask_job = 0, tag_give_job = 1;
	int q_size;
	std::shared_ptr<ThreadSafeQueue> queue;
	std::mutex mutex;
	std::condition_variable cond;
	bool reason_for_work = true;

	std::vector<std::thread*> threads;
};
