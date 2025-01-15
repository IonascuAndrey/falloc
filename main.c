#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include "falloc.h"
#define N 1

pthread_mutex_t mtx;
sem_t sem;

int bariera;

void init(int num)
{
	bariera = num;
}

void barrier_point()
{
	pthread_mutex_lock(&mtx);
	bariera --;
	pthread_mutex_unlock(&mtx);
	if(bariera > 0)
		if(sem_wait(&sem))
			perror("IN FCT BAR LA SEM\n");
	if(bariera == 0)
	{
		for(int i = 0; i < N; i++)
			if(sem_post(&sem))
				perror("IN FCT BAR LA SEM POST\n"); 
        printf("S-au dus 5");
	}
}


void *tfun(void *v)
{
    int* tid=(int*)v;
	// for(int i=0; i<5; i++) {
    //     void *ptr = fmalloc(5*sizeof(int));
    //     if (ptr == NULL) {
    //         printf("Memory allocation failed for thread: %d\n", *tid);
    //     }
    //     printf("Allocated memory at %pfor thread: %d\n", ptr, *tid);

    //     // // Reallocate memory to 256 bytes
    //     // ptr = frealloc(ptr, 256);
    //     // if (ptr == NULL) {
    //     //     printf("Memory reallocation failed\n");
    //     // }
    //     // printf("Reallocated memory at %p for thread: %d\n", ptr, *tid);

    //     // Free the allocated memory
    //     // if(*tid%2==1) {
    //     //     ffree(ptr);
    //     //     printf("Freed memory at %p for thread: %d\n", ptr, *tid);
    //     // }
    // }

    // TESTE SINGLE THREADING
    // void *ptr1 = fmalloc(100*sizeof(int));
    // void *ptr2 = fmalloc(100*sizeof(int));
    // void *ptr3 = fmalloc(100*sizeof(int));
    // void *ptr4 = fmalloc(100*sizeof(int));
    // void *ptr5 = fmalloc(100*sizeof(int));
    // ffree(ptr3);
    // void *ptr6 = fmalloc(5*sizeof(int));
    // ffree(ptr6);
    // void *ptr7 = fmalloc(200*sizeof(int));
    // ffree(ptr4);

    //------------
    void *ptr1 = fmalloc(100*sizeof(int));
    void *ptr2 = fmalloc(100*sizeof(int));
    void *ptr3 = fmalloc(100*sizeof(int));
    void *ptr4 = fmalloc(100*sizeof(int));
    ffree(ptr1);
    ffree(ptr2);

    ffree(ptr3);
    ffree(ptr4);
    int *ptr5 = fmalloc(1*sizeof(int));
    *ptr5=12;
    printf("Pointer1:----%d", *ptr5);
    // int* ptr7= frealloc(ptr5, 10*sizeof(int));
    // printf("Pointer2:----%d", *ptr7);

    return NULL;
}

int main()
{
	// if(pthread_mutex_init(&mtx, NULL))
	// {
	// 	perror(NULL);
	// 	return errno;
	// }
	// if (sem_init(&sem, 0, 0))
	// {
	// 	perror(NULL);
	// 	return errno;
	// }
	// init(N);
	
	// //5 threaduri
	// pthread_t thr[N];
	// for(int i = 0; i < N; i++)
	// {
	// 	int *tid = malloc(sizeof(int));
	// 	*tid = i;
	// 	if(pthread_create(&thr[i], NULL, tfun, (void *)tid))
	// 	{
	// 		perror(NULL);
	// 		return errno;
	// 	}
	// }
	
	// for(int i = 0; i < N; i++)
	// 	pthread_join(thr[i], NULL);
	// sem_destroy(&sem);
	// pthread_mutex_destroy(&mtx);
	// return 0;
    void *ptr1 = fmalloc(100*sizeof(int));
    void *ptr2 = fmalloc(100*sizeof(int));
    void *ptr3 = fmalloc(100*sizeof(int));
    void *ptr4 = fmalloc(100*sizeof(int));
    ffree(ptr1);
    ffree(ptr2);

    ffree(ptr3);
    ffree(ptr4);
    int *ptr5 = fmalloc(1*sizeof(int));
    *ptr5=12;
    int* ptr7= frealloc(ptr5, 10*sizeof(int));
    printf("Pointer1:----%d\n", *ptr5);
    // printf("Pointer2:----%d", *ptr7);
    return 0;
}


