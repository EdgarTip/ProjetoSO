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

sem_t *mutex;




void Race_Manager(struct config_fich_struct *inf_fichP, struct team *team_listP, sem_t *mutexP){

  #ifdef DEBUG
  printf("Race_Manager created with id: %ld\n",(long)getpid());
  #endif

  inf_fich = inf_fichP;
  team_list = team_listP;
  mutex = mutexP;

  //CRIAR OS GESTORES DE EQUIPA

  for(int i=0;i<inf_fich->number_of_teams;i++){
    if(fork()==0){

        Team_Manager(inf_fich, team_list, mutex);
        exit(0);

   }
 }


 wait(NULL);
 exit(0);
}
