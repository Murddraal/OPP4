#include "thread_safe_queue.h"
#include "work.h"
#include <mpi.h>
#include <vector>
#include <memory>
#include <cmath>

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
	worker->do_work(balance);

	MPI_Finalize();
	return 0;
}