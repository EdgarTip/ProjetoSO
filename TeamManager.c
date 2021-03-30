
#define DEBUG
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h> // include POSIX semaphores
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>

#include "MultipleProcessActions.h"
#include "RaceSimulator.h"

struct config_fich_struct *inf_fich;
struct team *team_list;
sem_t *mutex;



void *carThread(void* team_number){
    int number=*((int *)team_number);
    #ifdef DEBUG
    printf("Criei carro da equipa %d.\n",number);
    #endif
    pthread_exit(NULL);
    return NULL;

}



void Team_Manager(struct config_fich_struct *inf_fichP, struct team *team_listP, sem_t *mutexP){
  #ifdef DEBUG
  printf("Team Manager created with id: %ld\n", (long)getpid());
  #endif

  sleep(1);
  inf_fich = inf_fichP;
  team_list = team_listP;
  mutex = mutexP;

  int workerId[inf_fich->number_of_cars];
  pthread_t carros[inf_fich->number_of_cars];

  for(int i=0; i<inf_fich->number_of_cars;i++){
    workerId[i] = i+1;
    pthread_create(&carros[i], NULL, carThread,&workerId[i]);
  }
  //waits for all the cars to die
  for(int j=0; j<inf_fich->number_of_cars; j++){
    pthread_join(carros[j],NULL);
  }

  return;
}
