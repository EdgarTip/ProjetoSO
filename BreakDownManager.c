//Edgar Filipe Ferreira Duarte 2019216077
//Pedro Guilherme da Cruz Ferreira 2018277677
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include "RaceSimulator.h"
#include "MultipleProcessActions.h"
#include "TeamManager.h"

struct config_fich_struct *inf_fich;

struct team *team_list;
struct semaphoreStruct *semaphore_list;

int start = 0;

void endBreakDown(int signum){

  #ifdef DEBUG
  printf("Break down manager killed!\n");
  #endif
  exit(0);

}

void raceStart(int signum){
  start = 1;
}

void createBreakdowns(){


  sem_wait(semaphore_list->readingMutex);

  ++team_list[0].number_readers;

  if(team_list[0].number_readers == 1){
    sem_wait(semaphore_list->writingMutex);
  }

  sem_post(semaphore_list->readingMutex);

  srand((unsigned) time(NULL));
  for(int i= 0; i < inf_fich->number_of_teams; i++){
    for(int j = 0; j < team_list[i].number_of_cars; j++){

      int r = rand()%100;

      if(r >= team_list[i].cars[j].reliability){
        //Notify the car via the message queue
        printf("A breakdown was created\n");
      }

    }
  }

  sem_wait(semaphore_list->readingMutex);
  --team_list[0].number_readers;

  if(team_list[0].number_readers == 0){
    sem_post(semaphore_list->writingMutex);
  }

  sem_post(semaphore_list->readingMutex);
}


void BreakDownManager(struct config_fich_struct *inf_fichP, struct team *team_listP, struct semaphoreStruct *semaphore_listP){

  signal(SIGINT, endBreakDown);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGUSR2,raceStart);

  #ifdef DEBUG
  printf("Breakdown Manager created with id: %ld\n",(long)getpid());
  #endif

  inf_fich = inf_fichP;
  team_list = team_listP;
  semaphore_list = semaphore_listP;

  while(start != 1) {

    #ifdef DEBUG
    printf("[Proc %d] Waiting for a signal...\n",getpid());
    #endif
    pause();
  }


  while(start == 1) {
    sleep(inf_fich->T_Avaria);

    createBreakdowns();

}

  #ifdef DEBUG
  printf("Breakdown Manager is out!\n");
  #endif
  exit(0);
}
