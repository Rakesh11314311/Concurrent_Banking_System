#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_ACCOUNTS 10

typedef struct
{
  int id;
  int bal;
  pthread_mutex_t lock;
} acc_t;

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

void perform_transaction(int id, int amount, int atm_id)
{
  acc_t *account = &accounts[id - 1];

  pthread_mutex_lock(&account->lock);

  account->bal += amount;

  // printf("ATM %d: Account %d, Transaction: %+d, New Balance: %d\n",
  //        atm_id, id, amount, account->bal);

  pthread_mutex_unlock(&account->lock);
}

void *atm_transaction(void *arg)
{
  srand(time(0));

  int *transactions = (int *)arg;
  int atm_id = transactions[0];
  int id;
  int amount;

  for (int i = 0; i < 10; i++)
  {
    int id = (rand() % 10) + 1;
    int amount = (rand() % 20) - 20;

    perform_transaction(id, amount, atm_id);

    if (amount < 0)
    {
      printf("Amount %d was deducted from the account no. %d by ATM no. %d\n", -amount, id, atm_id);
    }
    else
    {
      printf("Amount %d was added to the account no. %d by ATM no. %d\n", amount, id, atm_id);
    }
  }

  return NULL;
}

int main()
{
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

  return 0;
}
