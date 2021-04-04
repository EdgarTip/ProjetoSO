
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>

#include "RaceSimulator.h"
#include "MultipleProcessActions.h"
#include "TeamManager.h"

struct config_fich_struct *inf_fich;

struct team *team_list;
struct semaphoreStruct *semaphore_list;

void BreakDownManager(struct config_fich_struct *inf_fichP, struct team *team_listP, struct semaphoreStruct *semaphore_listP){

  #ifdef DEBUG
  printf("Breakdown Manager created with id: %ld\n",(long)getpid());
  #endif


  inf_fich = inf_fichP;
  team_list = team_listP;
  semaphore_list = semaphore_listP;

  #ifdef DEBUG
  printf("Breakdown Manager is out!\n");
  #endif
}
