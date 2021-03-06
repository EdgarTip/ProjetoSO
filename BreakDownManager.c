//Edgar Filipe Ferreira Duarte 2019216077
//Pedro Guilherme da Cruz Ferreira 2018277677
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/msg.h>


#include "RaceSimulator.h"
#include "MultipleProcessActions.h"
#include "TeamManager.h"

struct config_fich_struct *inf_fich;
struct team *team_list;
struct semaphoreStruct *semaphore_list;


int start_breakdown = 0;

void endBreakDown(int signum){
  free(semaphore_list);
  free(inf_fich);
  #ifdef DEBUG
    printf("Break Down Manager is out!\n");
  #endif
  exit(0);

}

void raceStartBreakdown(int signum){
  start_breakdown = 1;
}

void createBreakdowns(struct ids *idsP){

  char log[200];
  sem_wait(semaphore_list->writingMutex);


  srand((unsigned) time(NULL));
  for(int i= 0; i < inf_fich->number_of_teams; i++){
    for(int j = 0; j < team_list[i].number_of_cars; j++){

      int r = rand()%100;

      if(r >= team_list[i].cars[j].reliability && team_list[i].cars[j].has_breakdown != 1){

        sprintf(log, "NEW PROBLEM IN CAR %02d",team_list[i].cars[j].car_number);
        //printf("%s\n",log);
        writeLog(log,semaphore_list->logMutex,inf_fich->fp);
        struct messageQ msg;

        msg.mtype=i*inf_fich->number_of_cars+j+1;
        msg.response = 1;

        #ifdef DEBUG
          printf("Breakdown Sent(%s)\n", team_list[i].team_name);
        #endif

        msgsnd(idsP->msg_queue_id, &msg, sizeof(msg)-sizeof(long), 0);
      }

    }
  }

  sem_post(semaphore_list->writingMutex);

}


int BreakDownManager(struct config_fich_struct *inf_fichP, struct team *team_listP, struct semaphoreStruct *semaphore_listP, struct ids *idsP){

  sigset_t mask, new_mask;

  //Ignore all unwanted signals!
  sigfillset(&mask);
  sigprocmask(SIG_SETMASK, &mask, NULL);


  sigemptyset(&new_mask);
  sigaddset(&new_mask, SIGUSR2);
  sigaddset(&new_mask, SIGTERM);

  sigprocmask(SIG_UNBLOCK,&new_mask, NULL);

  signal(SIGUSR2,endBreakDown);
  signal(SIGTERM,raceStartBreakdown);

  #ifdef DEBUG
      printf("Breakdown Manager created(%ld)\n",(long)getpid());
  #endif

  inf_fich = inf_fichP;
  team_list = team_listP;
  semaphore_list = semaphore_listP;


  pause();




  while(start_breakdown == 1) {
    sleep(inf_fich->T_Avaria);
    createBreakdowns(idsP);

}
}
