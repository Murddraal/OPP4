#include "multiply.h"
#include "thread_pool.h"
#include "task.h"
#include "matrix.h"




void multiplying_matr(const size_t& matr_size, const size_t& num_blocks, const size_t& num_threads,
	cpc_elm matr_a, cpc_elm matr_b, mut_matr * const result_matr)
{
	const size_t block_width = size_t(matr_size / num_blocks);

	ThreadPool tp(num_threads);

	// шаг цикла - ширина блока.
	for (size_t i = 0, ib = 0; i < matr_size; i += block_width, ++ib)
	{
		for (size_t j = 0, jb = 0; j < matr_size; j += block_width, ++jb)
		{
			for (size_t k = 0; k < matr_size; k += block_width)
			{
				size_t  a_ind = i * matr_size + k;
				size_t  b_ind = k * matr_size + j;
				// добавление данных в очередь
				tp.submit(std::make_shared<MyTask>(th_data(&matr_a[a_ind], &matr_b[b_ind], block_width, matr_size, result_matr, i, j)));
			}
		}
	}
}
pc_elm simply_mupliply(cpc_elm matr_a, cpc_elm matr_b, const size_t& matr_size)
{
	pc_elm test_matr = new elm[matr_size*matr_size];
	for (size_t i = 0; i < matr_size*matr_size; ++i)
		test_matr[i] = 0;

	for (size_t i = 0; i < matr_size; ++i)
	{
		for (size_t j = 0; j < matr_size; ++j)
		{
			for (size_t k = 0, el = i*matr_size + j; k < matr_size; ++k)
			{
				test_matr[el] += matr_a[i*matr_size + k] * matr_b[k*matr_size + j];
			}
		}
	}
	return test_matr;
}

elm test(cpc_elm matr_a, cpc_elm matr_b, const size_t& matr_size)
{
	elm summ = 0;
	for (size_t i = 0; i < matr_size * matr_size; ++i)
		summ += matr_a[i] - matr_b[i];

	return summ;
}