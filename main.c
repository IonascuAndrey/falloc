#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include "falloc.h"
#define N 1


//SINGLE THREADED:
//TEST 1 -> FMALLOC -> SIMPLE
//TEST 2 -> FFREE -> SIMPLE + SPLIT + MERGE
//TEST 3 -> REALLOC
//TEST 4 -> CASCADE_UNMAP
//TEST 5 -> allocating a vector of ints and iterating through it

//MULTI THREADED:
//TEST 1 -> FMALLOC -> SIMPLE + SPLIT
//TEST 2 -> FFREE -> SIMPLE + MERGE
//TEST 3 -> REALLOC
//TEST 4 -> CASCADE_UNMAP


//SINGLE THREADED:
//TEST 1 -> FMALLOC -> SIMPLE + SPLIT

void Test1(){
    //ALLOCATING
    void *ptr1 = fmalloc(100*sizeof(int));
    void *ptr2 = fmalloc(100*sizeof(int));
    void *ptr3 = fmalloc(100*sizeof(int));
    void *ptr4 = fmalloc(100*sizeof(int));
    //DEMONSTRATING
    print_memory_list();
    ffree(ptr1);
    ffree(ptr2);
    ffree(ptr3);
    ffree(ptr4);
    return 0;
}

void Test2(){
    //ALLOCATING
    void *ptr1 = fmalloc(100*sizeof(int));
    void *ptr2 = fmalloc(100*sizeof(int));
    void *ptr3 = fmalloc(100*sizeof(int));
    void *ptr4 = fmalloc(100*sizeof(int));
    printf("FFREE:\n");
    print_memory_list();
    printf("----------------------------------\n");
    printf("After FFREEING block number 2:\n");
    ffree(ptr2);
    print_memory_list();
    printf("----------------------------------\n\n");
    printf("AFTER SPLITING:\n");
    printf("----------------------------------\n");
    printf("Allocating only 50 * sizeof(int) -> splitting block 2:\n");
    void *ptr5 = fmalloc(50*sizeof(int));
    print_memory_list();
    printf("----------------------------------\n\n");
    printf("AFTER MERGING:\n");
    printf("----------------------------------\n");
    printf("Freeing the block 2 -> merging block 2 and 3 due to their contigous addresses:\n");
    ffree(ptr5);
    print_memory_list();
    printf("----------------------------------\n\n");
}

void Test3(){
    //ALLOCATING -> A BIGGER SIZE
    int *ptr1 = fmalloc(2*sizeof(int));
    int *ptr2 = fmalloc(10*sizeof(int)); //to prevent emptying the list (YET :)))
    printf("REALLOCATING A BIGGER SIZE:\n");
    printf("----------------------------------\n");
    printf("The first block has a size of 8 bytes:\n");
    print_memory_list();
    printf("----------------------------------\n");
    printf("Reallocting the ptr1 to 4 * sizeof(int):\n"); 
    ptr1 = frealloc(ptr1, 4 * sizeof(int));
    print_memory_list();
    printf("----------------------------------\n\n");
    printf("REALLOCATING A SMALLER SIZE:\n");
    printf("----------------------------------\n");
    printf("The first block has a size of 16 bytes:\n");
    print_memory_list();
    printf("----------------------------------\n");
    printf("Reallocating the ptr1 to 2 * sizeof(int):\n");
    ptr1 = frealloc(ptr1, 2 * sizeof(int));
    print_memory_list();
    printf("----------------------------------\n");
}


void Test4(){
    void *ptr1 = fmalloc(100*sizeof(int));
    void *ptr2 = fmalloc(100*sizeof(int));
    void *ptr3 = fmalloc(100*sizeof(int));
    void *ptr4 = fmalloc(100*sizeof(int));
    void *ptr5 = fmalloc(100*sizeof(int));
    void *ptr6 = fmalloc(100*sizeof(int));
    void *ptr7 = fmalloc(100*sizeof(int));
    void *ptr8 = fmalloc(100*sizeof(int));
    void *ptr9 = fmalloc(100*sizeof(int));
    void *ptr10 = fmalloc(100*sizeof(int));
    printf("ALLOCATING 10 BLOCKS:\n");
    printf("----------------------------------\n");
    print_memory_list();
    printf("----------------------------------\n\n");
    printf("FREING SOME INTERMEDIATE BLOCKS:\n");
    ffree(ptr2);
    ffree(ptr3);
    ffree(ptr5);
    ffree(ptr6);
    ffree(ptr8);
    ffree(ptr9);
    print_memory_list();
    printf("----------------------------------\n\n");
    printf("FIRST CASCADE:\n");
    ffree(ptr10);
    printf("----------------------------------\n");
    print_memory_list();
    printf("----------------------------------\n\n");
    printf("SECOND CASCADE:\n");
    ffree(ptr7);
    printf("----------------------------------\n");
    print_memory_list();
    printf("----------------------------------\n\n");
    printf("THIRD CASCADE:\n");
    ffree(ptr4);
    printf("----------------------------------\n");
    print_memory_list();
    printf("----------------------------------\n\n");
}

void Test5(){
    int *ptr1 = fmalloc(100 * sizeof(int));
    int *ptr2 = fmalloc(1 * sizeof(int));
    *ptr2 = 0;
    for(int i = 0; i < 100; i++)
        ptr1[i] = i;
    for(int i = 0; i < 100; i++)
        *ptr2 = *ptr2 + i;
    printf("The sum of the 100 allocated elements is: %d.\n", *ptr2);
}


int main(){
    Test1();
    Test2();
    Test3();
    Test4();
    Test5();
    return 0;
}




// pthread_mutex_t mtx;
// sem_t sem;

// int bariera;

// void init(int num)
// {
// 	bariera = num;
// }

// void barrier_point()
// {
// 	pthread_mutex_lock(&mtx);
// 	bariera --;
// 	pthread_mutex_unlock(&mtx);
// 	if(bariera > 0)
// 		if(sem_wait(&sem))
// 			perror("IN FCT BAR LA SEM\n");
// 	if(bariera == 0)
// 	{
// 		for(int i = 0; i < N; i++)
// 			if(sem_post(&sem))
// 				perror("IN FCT BAR LA SEM POST\n"); 
//         printf("S-au dus 5");
// 	}
// }


// void *tfun(void *v)
// {
//     int* tid=(int*)v;
// 	// for(int i=0; i<5; i++) {
//     //     void *ptr = fmalloc(5*sizeof(int));
//     //     if (ptr == NULL) {
//     //         printf("Memory allocation failed for thread: %d\n", *tid);
//     //     }
//     //     printf("Allocated memory at %pfor thread: %d\n", ptr, *tid);

//     //     // // Reallocate memory to 256 bytes
//     //     // ptr = frealloc(ptr, 256);
//     //     // if (ptr == NULL) {
//     //     //     printf("Memory reallocation failed\n");
//     //     // }
//     //     // printf("Reallocated memory at %p for thread: %d\n", ptr, *tid);

//     //     // Free the allocated memory
//     //     // if(*tid%2==1) {
//     //     //     ffree(ptr);
//     //     //     printf("Freed memory at %p for thread: %d\n", ptr, *tid);
//     //     // }
//     // }

//     // TESTE SINGLE THREADING
//     // void *ptr1 = fmalloc(100*sizeof(int));
//     // void *ptr2 = fmalloc(100*sizeof(int));
//     // void *ptr3 = fmalloc(100*sizeof(int));
//     // void *ptr4 = fmalloc(100*sizeof(int));
//     // void *ptr5 = fmalloc(100*sizeof(int));
//     // ffree(ptr3);
//     // void *ptr6 = fmalloc(5*sizeof(int));
//     // ffree(ptr6);
//     // void *ptr7 = fmalloc(200*sizeof(int));
//     // ffree(ptr4);

//     //------------
//     void *ptr1 = fmalloc(100*sizeof(int));
//     void *ptr2 = fmalloc(100*sizeof(int));
//     void *ptr3 = fmalloc(100*sizeof(int));
//     void *ptr4 = fmalloc(100*sizeof(int));
//     //print_memory_list();
//     ffree(ptr1);
//     ffree(ptr2);
//     ffree(ptr3);
//     ffree(ptr4);
//     int *ptr5 = fmalloc(1*sizeof(int));
//     *ptr5=12;
//     //print_memory_list();
//     // int* ptr7= frealloc(ptr5, 10*sizeof(int));
//     // printf("Pointer2:----%d", *ptr7);

//     return NULL;
// }

// int main()
// {
// 	// if(pthread_mutex_init(&mtx, NULL))
// 	// {
// 	// 	perror(NULL);
// 	// 	return errno;
// 	// }
// 	// if (sem_init(&sem, 0, 0))
// 	// {
// 	// 	perror(NULL);
// 	// 	return errno;
// 	// }
// 	// init(N);
	
// 	// //5 threaduri
// 	// pthread_t thr[N];
// 	// for(int i = 0; i < N; i++)
// 	// {
// 	// 	int *tid = malloc(sizeof(int));
// 	// 	*tid = i;
// 	// 	if(pthread_create(&thr[i], NULL, tfun, (void *)tid))
// 	// 	{
// 	// 		perror(NULL);
// 	// 		return errno;
// 	// 	}
// 	// }
	
// 	// for(int i = 0; i < N; i++)
// 	// 	pthread_join(thr[i], NULL);
// 	// sem_destroy(&sem);
// 	// pthread_mutex_destroy(&mtx);
// 	// return 0;
//     void *ptr1 = fmalloc(100*sizeof(int));
//     void *ptr2 = fmalloc(100*sizeof(int));
//     void *ptr3 = fmalloc(100*sizeof(int));
//     int *ptr4 = fmalloc(100*sizeof(int));
//     int *ptr5 = fmalloc(1*sizeof(int));
//     void *ptr6 = fmalloc(100*sizeof(int));
//     void *ptr7 = fmalloc(100*sizeof(int));
//     void *ptr8 = fmalloc(100*sizeof(int));
//     void *ptr9 = fmalloc(100*sizeof(int));
//     void *ptr10 = fmalloc(100*sizeof(int));
//     ffree(ptr2);
//     ffree(ptr3);
//     ffree(ptr5);
//     ffree(ptr6);
//     ffree(ptr8);
//     ffree(ptr9);
//     ffree(ptr10);
//     print_memory_list();
//     ffree(ptr7);
//     print_memory_list();
//     ffree(ptr4);
//     print_memory_list();
    
//     // print_memory_list();
//     // ffree(ptr5);
//     // print_memory_list();
//     // ffree(ptr5);
//     // print_memory_list();
//     // ffree(ptr5);
//     // print_memory_list();
//     // ffree(ptr5);
//     // print_memory_list();
//     // ffree(ptr6);
//     // print_memory_list();
//     // ffree(ptr7);
//     // print_memory_list();
//     // ffree(ptr4);
//     // print_memory_list();
//     // int *ptr5 = fmalloc(1*sizeof(int));
//     // *ptr5=12;
//     // print_memory_list();
//     // // int* ptr7= frealloc(ptr5, 10*sizeof(int));
//     // // printf("Pointer2:----%d", *ptr7);

//     return 0;
// }


