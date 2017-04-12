#include "thread_safe_queue.h"
#include "work.h"
#include <mpi.h>
#include <vector>
#include <memory>
#include <cmath>
#include <iostream>

void show_iterations(int rank, size_t iterations)
{
	std::cout << "iterations in process " << rank << " : " << iterations << std::endl;
}

int main(int argc, char ** argv)
{
	int size, rank, required = MPI_THREAD_MULTIPLE, provided;
	size_t queue_size = 5;
	int b = 1;

	if (argc > 1)
		queue_size = std::atoi(argv[1]);
	if (argc > 2)
		b = std::atoi(argv[2]);

	MPI_Init_thread(&argc, &argv, required, &provided); // Инициализация MPI
	if (required != provided)
		exit(-1);
	MPI_Comm_size(MPI_COMM_WORLD, &size); // Определение числа процессов в области связи
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Определение номера процесса

	std::shared_ptr<DynamicBalance> worker = std::make_shared<DynamicBalance>(queue_size, rank, size);
	bool balance = b == 0 ? false : true;

	worker->filling_queue(rank);
	worker->do_work(balance);

	auto time_local = worker->get_time();
	auto iters = worker->get_iterations();
	double time;
	MPI_Allreduce(&time_local, &time, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD); // Сохранение результата в адресном пространстве всех процессов
	show_iterations(rank, iters);

	MPI_Finalize();

	if (rank == 0) std::cout << "\nВремя выполнения: " << time << std::endl;

	return 0;
}