#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_OF_THREADS 2

void *identify(void *arg)
{
    char *thread_name = (char *)arg;
    for (int i = 0; i <=4 ; i++)
    {

        printf("%s: %d\n", thread_name, i);
    }
    return NULL;
}

int main()
{
    pthread_t threads[NUM_OF_THREADS];
    char *thread_names[NUM_OF_THREADS] = {"thread 1", "thread 2"};

    printf("Launching thread\n");

    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, identify, (void *)thread_names[i]);
    }

    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }
    printf("Threads complete!\n");
}