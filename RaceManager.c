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


#include "RaceSimulator.h"
#include "MultipleProcessActions.h"
#include "TeamManager.h"

struct config_fich_struct *inf_fich;
struct team *team_list;
struct semaphoreStruct *semaphore_list;


void Race_Manager(struct config_fich_struct *inf_fichP, struct team *team_listP,  struct semaphoreStruct *semaphore_listP){

  #ifdef DEBUG
  printf("Race_Manager created with id: %ld\n",(long)getpid());
  #endif

  inf_fich = inf_fichP;
  team_list = team_listP;
  semaphore_list = semaphore_listP;

  struct car car4 = {50,30,10,70};
  writingNewCarInSharedMem(team_list, &car4, inf_fich, "Benfica", semaphore_list->writingMutex, semaphore_list->logMutex);

  pid_t mypid;
  pid_t childpid, wpid;
  mypid = getpid();

  int status = 0;

  //CRIAR OS GESTORES DE EQUIPA
  for(int i=0;i<inf_fich->number_of_teams;i++){
    childpid = fork();
    if(childpid==0){

        Team_Manager(inf_fich, team_list, semaphore_list);
        #ifdef DEBUG
        printf("Team Manager %ld is out!\n", (long)getpid());
        #endif
        exit(0);
   }

 }

 while ((wpid = wait(&status)) > 0);
 #ifdef DEBUG
 printf("Race Manager %ld is out!\n", (long)getpid());
 #endif
 exit(0);

}
