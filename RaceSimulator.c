//Edgar Filipe Ferreira Duarte 2019216077
//Pedro Guilherme da Cruz Ferreira 2018277677


//COMPILE THE CODE USING: gcc RaceSimulator.c RaceManager.c ReadConfig.c  TeamManager.c BreakDownManager.c MultipleProcessActions.c -lpthread -D_REENTRANT -Wall -o exec
//YOU CAN DEACTIVATE THE DEBUG MESSAGES BY DELITING THE "DEFINE DEBUG" (line 2) IN THE RACESIMULATOR.H FILE

//TO RUN THE CODE AFTER COMPILING IT USE THE FORMAT : ./exec {configuration file name}

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#include "RaceSimulator.h"
#include "RaceManager.h"
#include "ReadConfig.h"
#include "MultipleProcessActions.h"
#include "BreakDownManager.h"
/*Struct that retains the information given by the initial file
Not a shared memory struct but the values will be given to the
processes created in this script*/



/*Shared memory struct the will share memory about the current state of all
of the cars. Will be updated by the Race Manager
*/


struct config_fich_struct *inf_fich;

struct team *team_list;

struct semaphoreStruct *semaphore_list;


int shmid;



//Only for debug purposes will be deleted/changed later
void leituraParaTeste(){
  for(int i = 0; i< inf_fich->number_of_teams; i++){
    if(strcmp(team_list[i].team_name, "") == 0){
      return;
    }
    for(int j = 0; j < inf_fich->number_of_cars; j++){
      if(team_list[i].cars[j].speed ==0){
        break;
      }
      printf("Team name:%s\n Box state:%s\n Car number: %d\n Car speed: %d\n Car consumption: %d\n Car reliability: %d\n", team_list[i].team_name
                                                                                                                       , team_list[i].box_state
                                                                                                                       , team_list[i].cars[j].car_number
                                                                                                                       , team_list[i].cars[j].speed
                                                                                                                       , team_list[i].cars[j].consumption
                                                                                                                       , team_list[i].cars[j].reliability);
    }
    printf("-----------\n");
  }

}

//cleans active memory
void clean(){
  free(inf_fich);
  free(semaphore_list);
  shmdt(team_list);
  shmctl(shmid, IPC_RMID, NULL);
  sem_close(semaphore_list->logMutex);
  sem_close(semaphore_list->writingMutex);
  sem_unlink("MUTEX");
  sem_unlink("WRITING_MUTEX");
}

void endRace(int signum){
  printf("RACE IS ENDING!\n");

  pid_t wpid;
  int status = 0;
  while ((wpid = wait(&status)) > 0);

  clean();
  printf("adios\n");
  exit(0);
}

void printStatistics(int signum){

  readStatistics(team_list, semaphore_list);

}

//Main function. Here the RaceManager and the MalfunctionManager processes will be created
int main(int argc, char* argv[]){

  signal(SIGINT, endRace);
  signal(SIGTSTP, printStatistics);

  system(">logs.txt");  //Limpa o ficheiro logs.txt


  semaphore_list = (struct semaphoreStruct*) malloc(sizeof(struct semaphoreStruct));
  sem_unlink("MUTEX");
  semaphore_list->logMutex = sem_open("MUTEX", O_CREAT|O_EXCL,0700,1);
  sem_unlink("WRITING_MUTEX");
  semaphore_list->writingMutex = sem_open("WRITING_MUTEX", O_CREAT|O_EXCL,0700,1);

  if (argc!=2){
    writeLog("Error with input arguments. Execution aborted!", semaphore_list->logMutex);
  	printf("Invalid number of arguments!\nUse as: executable {name of the configurations file}\n");
  	exit(1);
   }

   //Read the configuration file
   char *file_name = argv[1];
   inf_fich=readConfigFile(file_name);



   #ifdef DEBUG
   printf("Creating shared memory\n");
   #endif

  //Creates shared memory
  shmid = shmget(IPC_PRIVATE, (sizeof(struct team*) + sizeof(struct car*) * inf_fich->number_of_cars)* inf_fich->number_of_teams, IPC_CREAT|0700);
  if (shmid < 1) exit(0);
  team_list = (struct team*) shmat(shmid, NULL, 0);
  if (team_list < (struct team*) 1) exit(0);

  for(int i = 0; i < inf_fich->number_of_teams ; i++){
      team_list[i].cars = (struct car*)(team_list + inf_fich->number_of_teams + i +1);
  }
  printf("SIMULATION STARTING\n");
  writeLog("SIMULATOR STARTING", semaphore_list->logMutex);


  int pid=fork();

  //Creates RaceManager and BreakDownManager
  if(pid==0){
    //Creates the RaceManager
    Race_Manager(inf_fich, team_list, semaphore_list);

  }


  int pid2=fork();
  if(pid2==0){
    //Creates the break down manager
    BreakDownManager(inf_fich, team_list, semaphore_list);


  }
  //The main process waits for its child (BreakDownManager) to die
  pid_t wpid;
  int status = 0;
  while ((wpid = wait(&status)) > 0);

  #ifdef DEBUG
  printf("---SHARED MEMORY BEFORE THE SIMULATOR ENDED---\n");
  //Only for debugging purposes. We know that there are no protections to the shared memory here, but they
  //do not need to exist because it is in a controled envirnoment. In future work there will be a mutex
  //stoping any readers in case someone is changing the shared memory.
  leituraParaTeste();
  #endif

  printf("SIMULATION CLOSING\n");
  writeLog("SIMULATOR CLOSING", semaphore_list->logMutex);
  clean();
  return 0;

}
