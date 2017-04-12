#pragma once
#include <memory>
#include <mpi.h>
#include <mutex>
#include "thread_safe_queue.h"
#include <thread>

class DynamicBalance
{
public:
	DynamicBalance(const int &q_size, const int &rank, const int &size);
	~DynamicBalance();
	void filling_queue(size_t job);
	void do_work(bool balance);
	void dispetcher();
	void provider();
	size_t get_iterations();
	double get_time();

private:
	bool balance;

	int size, rank, iterations;
	std::chrono::time_point<std::chrono::steady_clock> time_start, time_end;
	int tag_ask_job = 0, tag_give_job = 1;
	int q_size;
	std::shared_ptr<ThreadSafeQueue> queue;
	std::mutex mutex;
	std::condition_variable cond;
	bool reason_for_work = true;

	std::vector<std::thread*> threads;
};
