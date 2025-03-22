#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

#define NUM_ACCOUNTS 10

typedef struct
{
    int id;
    int bal;
    pthread_mutex_t lock;
} acc_t;

FILE *fp;
pthread_mutex_t csv_lock;

acc_t accounts[NUM_ACCOUNTS];

void initialize_accounts()
{
    for (int i = 0; i < NUM_ACCOUNTS; i++)
    {
        accounts[i].id = i + 1;
        accounts[i].bal = 0;
        pthread_mutex_init(&accounts[i].lock, NULL);
    }
}

void perform_transaction(int from_id, int to_id, int amount, int atm_id)
{
    acc_t *from_account = &accounts[from_id - 1];
    acc_t *to_account = &accounts[to_id - 1];

    pthread_mutex_lock(&from_account->lock);
    usleep(10);
    // pthread_mutex_lock(&to_account->lock);

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 5;

    if (pthread_mutex_timedlock(&to_account->lock, &ts) != 0)
    {
        printf("\nDeadlock occurred while trying to lock account no. %d from atm no. %d while\n trying to transfer amount %d to it from account no. %d\n\n", to_id, atm_id, amount, from_id);
        pthread_mutex_unlock(&from_account->lock);
        exit(1);
    }

    from_account->bal -= amount;
    to_account->bal += amount;

    pthread_mutex_unlock(&from_account->lock);
    pthread_mutex_unlock(&to_account->lock);
}

void *atm_transaction(void *arg)
{
    srand(time(0));

    int *transactions = (int *)arg;
    int atm_id = transactions[0];
    int id;
    int amount;

    for (int i = 0; i < 1000; i++)
    {
        int from_id = (rand() % 10) + 1;
        int to_id = (rand() % 10) + 1;
        if (from_id == to_id)
        {
            if (from_id == 0)
            {
                to_id++;
            }
            else
            {
                to_id--;
            }
        }

        int amount = (rand() % 20) + 1;

        perform_transaction(from_id, to_id, amount, atm_id);

        printf("Amount %d was transferred from the acc no. %d to acc no. %d by ATM no. %d\n", amount, from_id, to_id, atm_id);

        pthread_mutex_lock(&csv_lock);
        fprintf(fp, "%d,%d,%d,%d\n", amount, from_id, to_id, atm_id);
        fflush(fp);
        pthread_mutex_unlock(&csv_lock);
    }

    return NULL;
}

int main()
{
    fp = fopen("transaction.csv", "w");

    if (fp == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }

    fprintf(fp, "From_Account, To_Account, Amount, ATM\n");

    initialize_accounts();

    int atms[5][1] = {{1}, {2}, {3}, {4}, {5}};

    // Considering there are 5 atms
    pthread_t threads[5];
    for (int i = 0; i < 5; i++)
    {
        pthread_create(&threads[i], NULL, atm_transaction, (void *)atms[i]);
    }

    for (int i = 0; i < 5; i++)
    {
        pthread_join(threads[i], NULL);
    }

    fclose(fp);

    return 0;
}
