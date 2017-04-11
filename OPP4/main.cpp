#include "thread_safe_queue.h"
#include "work.h"
#include <mpi.h>
#include <vector>
#include <memory>
#include <cmath>

int main(int argc, char ** argv)
{

	int size, rank, required = MPI_THREAD_MULTIPLE, provided;
	size_t queue_size = 10;
	MPI_Init_thread(&argc, &argv, required, &provided); // Инициализация MPI
	if (required != provided)
		exit(-1);
	MPI_Comm_size(MPI_COMM_WORLD, &size); // Определение числа процессов в области связи
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Определение номера процесса

	std::shared_ptr<balance> bal = std::make_shared<balance>(queue_size, rank, size);
	return 0;
}