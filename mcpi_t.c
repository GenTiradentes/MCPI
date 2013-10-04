#include <stdio.h>
#include <math.h>

#include <pthread.h>

#include "mcpi_const.h"

typedef struct { int thread_id; vec_t*** buffer; int_t* hits; } thread_args;

void* calculate_pi_thread(void* arg)
{
	thread_args* args = (thread_args*)arg;

	int_t work_size = BUFFER_SIZE_SQRT / NUM_THREADS;
	int_t work_start = work_size * args->thread_id;
	int_t work_end = work_start + work_size;

	vec_t increment = 1.0 / (vec_t)BUFFER_SIZE_SQRT;

	int_t hits = 0;
	for(int x = work_start; x < work_end; x++)
		for(int y = 0; y < BUFFER_SIZE_SQRT; y++)
			if((powf((x * increment), 2) + powf(y * increment, 2)) < 1)
				hits++;

	// Huge performance hit if we increment this directly
	args->hits[args->thread_id] = hits;
	return NULL;
}

int main()
{
	thread_args targs[NUM_THREADS];
	int_t hits[NUM_THREADS];

	for(int i = 0; i < NUM_THREADS; i++)
	{
		targs[i].thread_id = i;
		targs[i].hits = (int_t*)hits;
	}

	pthread_t calcpi[NUM_THREADS];

	for(int i = 0; i < NUM_THREADS; i++)
		pthread_create(&calcpi[i], NULL, calculate_pi_thread, (void*)&targs[i]);

	for(int i = 0; i < NUM_THREADS; i++)
		pthread_join(calcpi[i], NULL);


	int_t hits_sum = 0;
	for(int i = 0; i < NUM_THREADS; i++)
		hits_sum += hits[i];

	printf("%f\n", (hits_sum * 4) / (vec_t)(BUFFER_SIZE_SQRT * BUFFER_SIZE_SQRT));
}
