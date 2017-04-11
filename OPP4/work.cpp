#include "work.h"
#include <iostream>

balance::balance(const int &q_size, const int &rank, const int &size) :q_size(q_size), rank(rank), size(size)
{
	queue = std::make_unique<ThreadSafeQueue>();

	for (int i = 0; i < q_size; ++i)
		queue->push(std::make_shared<MyTask>(abs(rank - (i%size))));

	threads.push_back(new std::thread(&balance::do_work, this));
	threads.push_back(new std::thread(&balance::provider, this));
	threads.push_back(new std::thread(&balance::dispetcher, this));
}

balance::~balance()
{
	for each (auto thread in threads)
	{
		thread->join();
	}
	threads.clear();
	MPI_Finalize();
	show_iterations();
}

void balance::show_iterations()
{
	std::cout << "iterations in process: "<<iterations << std::endl;
}

void balance::do_work()
{
	iterations = 0;
	while (reason_for_work || !queue->empty())
	{
		
		++iterations;
		std::cout << iterations << std::endl;
			
		auto j = queue->pop_or_wait();
		j->execute();

		if ((q_size / 2) >= queue->size())
			cond.notify_one();
	
		
	}
	std::cout << "worker " << rank << " died!\n";
}

void balance::dispetcher()
{

	while (reason_for_work)
	{		
		MPI_Barrier(MPI_COMM_WORLD);
		int recv_size = 0, recv;
		MPI_Status status;

		
		std::cout << "dispetcher " << rank << " going to recieve!\n";			
		MPI_Recv(&recv_size, 1, MPI_INT, MPI_ANY_SOURCE, tag_ask_job, MPI_COMM_WORLD, &status); // Приём сообщения		
		
		if (queue->size() <= recv_size)
			recv = 0;
		else
			recv = queue->pop_or_wait()->get_data();

		int all_recv;

		MPI_Allreduce(&recv, &all_recv, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
		if (all_recv == 0)
		{
			MPI_Barrier(MPI_COMM_WORLD);
			reason_for_work = false;
			recv = -1;
		}

		std::cout << "dispetcher " << rank << " going to send!\n";	
		MPI_Send(&recv, 1, MPI_INT, status.MPI_SOURCE, tag_give_job, MPI_COMM_WORLD); // Передача сообщения
		
	}

	int end = -1;
	MPI_Send(&end, 1, MPI_INT, (rank+1)%size, tag_give_job, MPI_COMM_WORLD); // Передача сообщения

	std::cout << "dispetcher " << rank << "  died!\n";
}

void balance::provider()
{
	std::unique_lock<std::mutex> lock(mutex);
	cond.wait(lock, [this] {return (q_size / 2) >= queue->size(); });
	while (reason_for_work)
	{
		MPI_Barrier(MPI_COMM_WORLD);
		auto j = get_job();
		/*if (is_provider == 0)
			break;*/
		if(j->get_data() != 0)
			queue->push(j);
	}

	std::cout << "provider " << rank << " died!\n";
}

TaskPtr balance::get_job()
{
	int recv = -2;

	MPI_Status status;

	for (int i = 0; i < size; ++i)
	{

		if (i == rank)
			continue;

		std::cout << "provider " << rank << " going to send!\n";
		int s = queue->size();
		MPI_Send(&s, 1, MPI_INT, i, tag_ask_job, MPI_COMM_WORLD);

		std::cout << "provider " << rank << " going to recieve!\n";
		MPI_Recv(&recv, 1, MPI_INT, i, tag_give_job, MPI_COMM_WORLD, &status);


		if (recv != 0)
			break;
	}

	/*if (recv == 0)
	{
		std::cout << "provider " << rank << "going to die!\n";
		is_provider = 0;
	}*/
	if (recv == -1)
		reason_for_work = false;
	return std::make_shared<MyTask>(recv);
}
