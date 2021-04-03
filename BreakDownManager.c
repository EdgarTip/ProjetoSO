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

void BreakDownManager(struct config_fich_struct *inf_fichP, struct team *team_listP, sem_t *mutexP){

  #ifdef DEBUG
  printf("Breakdown Manager created with id: %ld\n",(long)getpid());
  #endif

  inf_fich = inf_fichP;
  team_list = team_listP;
  mutex = mutexP;

  #ifdef DEBUG
  printf("Breakdown Manager is out!\n");
  #endif
}
