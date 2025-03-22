#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define N 1000
#define K 400

int counter = 0;
pthread_mutex_t lock;
pthread_cond_t cond_var;

void *increment_counter(void *arg)
{
    for (int i = 0; i < N; i++)
    {
        pthread_mutex_lock(&lock);
        counter++;

        if (counter == K)
        {
            pthread_cond_signal(&cond_var);
        }

        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void *watch_counter(void *arg)
{
    pthread_mutex_lock(&lock);

    while (counter < K)
    {
        pthread_cond_wait(&cond_var, &lock);
    }

    printf("Counter reached %d and T3 is running.\n", K);

    pthread_mutex_unlock(&lock);
    return NULL;
}

int main()
{
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond_var, NULL);

    pthread_t t1, t2, t3;

    pthread_create(&t1, NULL, increment_counter, NULL);
    pthread_create(&t2, NULL, increment_counter, NULL);
    pthread_create(&t3, NULL, watch_counter, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    printf("Final counter value: %d\n", counter);

    return 0;
}
