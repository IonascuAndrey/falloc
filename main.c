#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include "falloc.h"
#define N 2 //The number of threads synchronized to run at the same time


//SINGLE THREADED:
//TEST 1 -> FMALLOC -> SIMPLE
//TEST 2 -> FFREE -> SIMPLE + SPLIT + MERGE
//TEST 3 -> REALLOC
//TEST 4 -> CASCADE_UNMAP
//TEST 5 -> allocating a vector of ints and iterating through it

//MULTI THREADED: -> allocating memory to demonstrate matrix multiplication using threads



pthread_mutex_t mtx;
sem_t sem;

int bariera;

void Test1(){
    printf("TEST 1\n");
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
    printf("TEST 2\n");
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
    ffree(ptr1);
    ffree(ptr4);
    ffree(ptr3);
}

void Test3(){
    printf("TEST 3\n");
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
    ffree(ptr1);
    ffree(ptr2);

}


void Test4(){
    printf("TEST 4\n");
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
    printf("TEST 5\n");
    int *ptr1 = fmalloc(100 * sizeof(int));
    int *ptr2 = fmalloc(1 * sizeof(int));
    *ptr2 = 0;
    for(int i = 0; i < 100; i++)
        ptr1[i] = i;
    for(int i = 0; i < 100; i++)
        *ptr2 = *ptr2 + i;
    printf("The sum of the 100 allocated elements is: %d.\n", *ptr2);
}

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
	}
}


void *Test6(void *v)
{
    int* tid=(int*)v;
    barrier_point();
    void *ptr1 = fmalloc(5*sizeof(int));
    printf("Threadul %d: ptr1:%p\n", *tid, ptr1);
    void *ptr2 = fmalloc(5*sizeof(int));
    printf("Threadul %d: ptr2:%p\n", *tid, ptr2);
    print_memory_list();
    sleep(2);
    ffree(ptr1);
    ffree(ptr2);

    return NULL;
    }


void *Test7(void *v) {
    int* tid=(int*)v;
    barrier_point();
    void *ptr1 = fmalloc(10*sizeof(int));
    void *ptr2 = fmalloc(10*sizeof(int));
    ffree(ptr2);
    printf("SPLIT\n");
    print_memory_list();
    void *ptr3 = fmalloc(2*sizeof(int));
    ffree(ptr3);
    printf("MERGE\n");
    print_memory_list();
    sleep(2);
    ffree(ptr1);
    ffree(ptr2);

    return NULL;
}

void Test8(){
	pid_t pid = fork();
	if(pid < 0)
		return errno;
    else if(pid == 0)
    {
        char *argv2[] = {"matrix_multiplication", NULL};
        execve("/home/ioana/SO/Proiect/SO/matrix_multiplication", argv2, NULL);
        perror(NULL);
    }
    else 
    {	
        wait(NULL);
        printf("Done parent %d Me %d\n", getpid(), pid);
    }
}


int main(){
    //TESTE SINGLE THREADED
    //Test1();
    //Test2();
    //Test3();
    //Test4();
    //Test5();
    //TESTE MULTI THREADED
    //Test6();
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
	// 	int *tid = malloc(sizeof(int)); // We use the standard malloc to better show the functionality of fmalloc
	// 	*tid = i;
	// 	// if(pthread_create(&thr[i], NULL, Test6, (void *)tid))
	// 	// if(pthread_create(&thr[i], NULL, Test8, (void *)tid))
	// 	if(pthread_create(&thr[i], NULL, Test7, (void *)tid))
	// 	// if(pthread_create(&thr[i], NULL, Test9, (void *)tid))
	// 	{
	// 		perror(NULL);
	// 		return errno;
	// 	}
	// }
    // for(int i = 0; i < N; i++)
	// 	pthread_join(thr[i], NULL);
	// sem_destroy(&sem);
	// pthread_mutex_destroy(&mtx);

    return 0;
}


// //MATRIX MULTIPLICATION
// #include <stdio.h>
// #include <stdlib.h>
// #include <errno.h>
// #include <pthread.h>
// #include "fmalloc.h"

// int n1, n2, n3, m1, m2, m3;
// int *matrice1, *matrice2, *matrice3;

// typedef struct{
// 	int linie, coloana;
// }param;


// int *calculeaza_elem(void *v)
// {
// 	int sum = 0;
// 	int *s = fmalloc(sizeof(int));
// 	//convert la ce am eu nevoie din ce am primit ca parametrii
// 	param *p = (param *)v;
// 	int col = p -> coloana;
	
// 	for(int i = p -> linie * m1; i < p -> linie * m1 + m1; i++)
// 		{
// 			//printf("%d %d\n", matrice1[i], matrice2[col]);
// 			sum += matrice1[i] * matrice2[col];
// 			col = col + m2;
// 		}
// 	//printf("%d\n", sum);
// 	*s = sum;
// 	ffree(p);
// 	return s;
// }


// int main()
// {
// 	//CITIRE SI VALIDARE
// 	printf("Dimensiuni matrice1:\n");
// 	scanf("%d", &n1);
// 	scanf("%d", &m1);
// 	printf("Dimensiuni matrice2:\n");
// 	scanf("%d", &n2);
// 	scanf("%d", &m2);
// 	if(m1 != n2)
// 	{
// 		printf("Matricile nu pot fi inmultite");
// 		return 0;
// 	}
// 	n3 = n1;
// 	m3 = m2;
// 	matrice1 = fmalloc(n1 * m1 * sizeof(int));
// 	matrice2 = fmalloc(n2 * m2 * sizeof(int));
// 	matrice3 = fmalloc(n3 * m3 * sizeof(int));
// 	for(int i = 0; i < n1 * m1; i++)
// 		scanf("%d", &matrice1[i]);
// 	for(int i = 0; i < n2 * m2; i++)
// 		scanf("%d", &matrice2[i]);
	
// 	//TREBUIE SA FAC MAI MULTE THREADURI (MAI EXACT NR DE ELEM
// 	//ALE MATRICEI3) PT A CALCULA CONCOMITENT ELEM MATRICEI
// 	pthread_t thr[n3 * m3];
// 	int linie = 0, coloana = 0;
// 	for(int i = 0; i < n3 * m3; i++)
// 		{
// 			//trebuie neaparat unul separat pt ca s-ar putea ca var sa nu apuce sa se actualizeze la inceperea unui nou thread
// 			param *p = fmalloc(sizeof(param));
// 			p -> linie = linie;
// 			p -> coloana = coloana;
// 			if(pthread_create(&thr[i], NULL, calculeaza_elem, p))
// 			{
// 				perror(NULL);
// 				printf("Nu merge\n");
// 				return errno;
// 			}
// 			else
// 			{

// 				if(coloana == m2 - 1)
// 				{
// 					linie ++;
// 					coloana = 0;
// 				}
// 				else coloana ++;
// 			}
// 		}
// 	int *rez;
// 	for(int i = 0; i < n3 * m3; i++)
// 		{
// 			if(pthread_join(thr[i], &rez))
// 			{
// 				perror(NULL);
// 				return errno;
// 			}
// 			matrice3[i] = *rez;
// 		}
// 	ffree(rez);
// 	for(int i = 0; i < n3 * m3; i++)
// 	{
// 		if(i % m3 == 0)
// 			printf("\n");
// 		printf("%d ", matrice3[i]);
// 	}
// 	ffree(matrice1);
// 	ffree(matrice2);
// 	ffree(matrice3);
// 	return 0;
// }
