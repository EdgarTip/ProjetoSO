
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
struct semaphoreStruct *semaphore_list;



void *carThread(void* team_number){
    int number=*((int *)team_number);
    writeLog("Car thread created", semaphore_list->mutexFicheiro);
    #ifdef DEBUG
    printf("I %ld created car %d.\n",(long)getpid(),number);
    #endif
    pthread_exit(NULL);
    return NULL;

}



void Team_Manager(struct config_fich_struct *inf_fichP, struct team *team_listP,  struct semaphoreStruct *semaphore_listP){
  #ifdef DEBUG
  printf("Team Manager created with id: %ld\n", (long)getpid());
  #endif
  writeLog("Team Manager process created", semaphore_list->mutexFicheiro);

  inf_fich = inf_fichP;
  team_list = team_listP;
  semaphore_list = semaphore_listP;

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
