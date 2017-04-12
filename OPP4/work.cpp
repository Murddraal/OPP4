#include "work.h"
#include <iostream>

DynamicBalance::DynamicBalance(const int &q_size, const int &rank, const int &size) :q_size(q_size), rank(rank), size(size)
{
	queue = std::make_unique<ThreadSafeQueue>();
}

void DynamicBalance::filling_queue(size_t job)
{
	for (int i = 0; i < q_size; ++i)
		queue->push(std::make_shared<MyTask>(rank, job));
}

DynamicBalance::~DynamicBalance()
{
	for each (auto thread in threads)
	{
		if (thread->joinable())
			thread->join();
		thread->join();
	}
	threads.clear();
}

size_t DynamicBalance::get_iterations()
{
	return iterations;
}

double DynamicBalance::get_time()
{
	for each (auto thread in threads)
	{
		if(thread->joinable())
			thread->join();
	}
	threads.clear();

	time_end = std::chrono::high_resolution_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::duration<double>>(time_end - time_start).count();
	return time;
}

void DynamicBalance::do_work(bool _balance)
{
	time_start = std::chrono::high_resolution_clock::now();
	balance = _balance;
	if (balance)
	{
		threads.push_back(new std::thread(&DynamicBalance::provider, this));
		threads.push_back(new std::thread(&DynamicBalance::dispetcher, this));
	}

	iterations = 0;
	while (reason_for_work)
	{
		while (!queue->empty())
		{
			//std::cout << iterations << std::endl;
			auto j = TaskPtr();
			
			if(!queue->empty())
				j = queue->pop_or_wait();
			else
				break;

			j->execute();
			++iterations;

			if (balance && ((q_size / 2) >= queue->size()))
				cond.notify_one();
		}
		if (!balance)
			reason_for_work = false;
	}
	//std::cout << "worker " << rank << " died!\n";
}

void DynamicBalance::dispetcher()
{
	int k = 0;

	while (k != size - 1)
	{
		int rcv_size;
		MPI_Status st;
		MPI_Recv(&rcv_size, 1, MPI_INT, MPI_ANY_SOURCE, tag_ask_job, MPI_COMM_WORLD, &st);
		auto job = TaskPtr();

		mutex.lock();
		if (queue->size() > rcv_size)
			job = queue->pop_or_wait();		
		else
		{
			job = std::make_shared<MyTask>(-1, -1);
			k++;
		}

		mutex.unlock();

		int msg = job->get_rank();
		MPI_Send(&msg, 1, MPI_INT, st.MPI_SOURCE, tag_give_job, MPI_COMM_WORLD);

	}
	
	//std::cout << "dispetcher " << rank << "  died!\n";
}

void DynamicBalance::provider()
{
	std::mutex mutex;
	std::unique_lock<std::mutex> lock(mutex);
	cond.wait(lock, [this] {return (q_size / 2) >= queue->size(); });
	bool stop;
	std::vector<int> receivers;
	for (int i = 0; i < size; ++i)
	{
		if (i != rank)
			receivers.push_back(i);
	}

	do 
	{
		stop = true;
		for (int i = 0; i < size - 1; i++) 
		{
			if (receivers[i]!=-1)
			{
				int msg = queue->size();
				MPI_Send(&msg, 1, MPI_INT, receivers[i], tag_ask_job, MPI_COMM_WORLD);
				int job;
				MPI_Status st;
				MPI_Recv(&job, 1, MPI_INT, receivers[i], tag_give_job, MPI_COMM_WORLD, &st);
				if (job != -1)
				{
					queue->push(std::make_shared<MyTask>(rank, job));
					stop = false;
				}
				else
					receivers[i] = -1;
			}
		}
	} while (!stop);

	reason_for_work = false;
	//std::cout << "provider " << rank << " died!\n";
}