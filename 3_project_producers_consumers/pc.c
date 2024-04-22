#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include "eventbuf.h"

sem_t *producer_event_buff;
sem_t *consumer_event_buff;
int global_num_events;
struct eventbuf *global_eventbuf;
volatile int producers_done = 0;

sem_t *sem_open_temp(const char *name, int value)
{
    sem_t *sem;

    // Create the semaphore
    if ((sem = sem_open(name, O_CREAT, 0600, value)) == SEM_FAILED)
        return SEM_FAILED;

    // Unlink it so it will go away after this process exits
    if (sem_unlink(name) == -1)
    {
        sem_close(sem);
        return SEM_FAILED;
    }

    return sem;
}

void *producer_run(void *arg)
{
    int thread_id = *(int *)arg;
    for (int i = 0; i < global_num_events; i++)
    {
        sem_wait(producer_event_buff);
        printf("P%d: adding event %d\n", thread_id, thread_id * 100 + i);
        eventbuf_add(global_eventbuf, thread_id * 100 + i);
        sem_post(consumer_event_buff);
    }
    printf("P%d: exiting\n", thread_id);
    return NULL;
}

void *consumer_run(void *arg)
{
    int thread_id = *(int *)arg;

    while (1)
    {
        sem_wait(consumer_event_buff);
        if (producers_done == 1){
            printf("C%d: exiting\n", thread_id);
            return NULL;
        }
        int event = eventbuf_get(global_eventbuf);
        printf("C%d: got event %d\n", thread_id, event);
        sem_post(producer_event_buff);
    }
}

int main(int arg, char *argv[])
{
    if (arg != 5)
    {
        fprintf(stderr, "Four arguments are required");
        return -1;
    }

    int producers = atoi(argv[1]);
    if (producers <= 0)
    {
        fprintf(stderr, "Invalid number of producers");
    }

    int consumers = atoi(argv[2]);
    if (consumers <= 0)
    {
        fprintf(stderr, "Invalid number of consumers");
    }

    int num_events = atoi(argv[3]);
    if (num_events <= 0)
    {
        fprintf(stderr, "Invalid number of events");
    }

    global_num_events = num_events;

    int outstanding_event_count = atoi(argv[4]);
    if (outstanding_event_count <= 0)
    {
        fprintf(stderr, "Invalid number of maximum outstanding event count");
    }
    
    producer_event_buff = sem_open_temp("/temp_sem", outstanding_event_count);
    if (producer_event_buff == SEM_FAILED)
    {
        perror("sem_open_temp failure");
        return 1;
    }

    consumer_event_buff = sem_open_temp("/temp_sem", 0);
    if (consumer_event_buff == SEM_FAILED)
    {
        perror("sem_open_temp failure");
        return 1;
    }

    global_eventbuf = eventbuf_create();

    pthread_t producer_threads[producers];
    pthread_t consumer_threads[consumers];

    int *producer_ids = malloc(producers * sizeof(int));
    int *consumer_ids = malloc(consumers * sizeof(int));

    for (int i = 0; i < producers; i++)
    {
        producer_ids[i] = i;
        pthread_create(&producer_threads[i], NULL, producer_run, &producer_ids[i]);
    }

    for (int i = 0; i < consumers; i++)
    {
        consumer_ids[i] = i;
        pthread_create(&consumer_threads[i], NULL, consumer_run, &consumer_ids[i]);
    }

    for (int i = 0; i < producers; i++)
    {
        pthread_join(producer_threads[i], NULL);
    }

    producers_done = 1;

    for (int i = 0; i < consumers; i++)
    {
        sem_post(consumer_event_buff);
    }

    for (int i = 0; i < consumers; i++)
    {
        pthread_join(consumer_threads[i], NULL);
    }

    if (sem_close(producer_event_buff) == -1)
    {
        perror("sem_close error");
        return 1;
    }

    if (sem_close(consumer_event_buff) == -1)
    {
        perror("sem_close error");
        return 1;
    }
}
