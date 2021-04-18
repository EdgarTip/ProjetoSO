//Edgar Filipe Ferreira Duarte 2019216077
//Pedro Guilherme da Cruz Ferreira 2018277677
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "RaceSimulator.h"
#include "MultipleProcessActions.h"

struct config_fich_struct *inf_fich;
struct team *team_list;
struct semaphoreStruct *semaphore_list;

pthread_t *cars;



void teamEnd(int signum){
  for(int j=0; j<inf_fich->number_of_cars; j++){
    pthread_cancel(cars[j]);
  }
  printf("Team %ld killed\n",(long)getpid());
  free(cars);
  printf("morreu\n");
  exit(0);
}


//Car thread. For now it dies immediatly after being created
void *carThread(void* team_number){

  int number=*((int *)team_number);

  #ifdef DEBUG
  printf("I %ld created car %d.\n",(long)getpid(),number);
  #endif
  printf("i am waiting\n");

  pthread_exit(NULL);
  return NULL;

}


//Team manager. Will create the car threads
void Team_Manager(struct config_fich_struct *inf_fichP, struct team *team_listP,  struct semaphoreStruct *semaphore_listP){
  signal(SIGINT, teamEnd);

  #ifdef DEBUG
  printf("Team Manager created with id: %ld\n", (long)getpid());
  #endif


  inf_fich = inf_fichP;
  team_list = team_listP;
  semaphore_list = semaphore_listP;

  int workerId[inf_fich->number_of_cars];

  cars = malloc(sizeof(pthread_t) * inf_fich->number_of_cars);

  //Create the car threads
  for(int i=0; i<inf_fich->number_of_cars;i++){
    workerId[i] = i+1;
    pthread_create(&cars[i], NULL, carThread,&workerId[i]);
  }
  //Waits for all the cars to die
  for(int j=0; j<inf_fich->number_of_cars; j++){
    pthread_join(cars[j],NULL);
  }

  free(cars);
  sleep(4);
  return;
}
