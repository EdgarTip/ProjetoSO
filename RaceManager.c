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


  pid_t mypid;
  pid_t childpid;

  mypid = getpid();
  childpid = fork();

  //CRIAR OS GESTORES DE EQUIPA
  for(int i=0;i<inf_fich->number_of_teams;i++){
    if(childpid==0){

        Team_Manager(inf_fich, team_list, semaphore_list);
        childpid = fork();
   }
   else{
     if(getpid() == mypid){
        wait(NULL);
        #ifdef DEBUG
        printf("Race Manager is out!\n");
        #endif
        exit(0);
     }

     else{
       wait(NULL);
       #ifdef DEBUG
       printf("Team Manager %ld is out!\n", (long)getpid());
       #endif

       wait(NULL);
       exit(0);
     }


   }
 }
 #ifdef DEBUG
 printf("Team Manager %ld is out!\n", (long)getpid());
 #endif
 exit(0);

}
