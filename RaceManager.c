#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>

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

  //For testing purposes!
  struct car car1 = {10,70,60,19};
  struct car car2 = {20,90,10,90};
  struct car car3 = {100,20,30,80};
  struct car car4 = {50,30,10,70};
  struct car car5 = {60,60,60,60};
  struct car car6 = {30,90,70,65};
  struct car car7 = {40,40,100,20};
  struct car car8 = {50,90,30,50};
  struct car car9 = {70,50,10,35};
  writingNewCarInSharedMem(team_list, &car1, inf_fich, "Sporting", semaphore_list->writingMutex, semaphore_list->logMutex);
  writingNewCarInSharedMem(team_list, &car2, inf_fich, "Porto", semaphore_list->writingMutex, semaphore_list->logMutex);
  writingNewCarInSharedMem(team_list, &car3, inf_fich, "Sporting", semaphore_list->writingMutex, semaphore_list->logMutex);
  writingNewCarInSharedMem(team_list, &car4, inf_fich, "Benfica", semaphore_list->writingMutex, semaphore_list->logMutex);
  writingNewCarInSharedMem(team_list, &car5, inf_fich, "Sporting", semaphore_list->writingMutex, semaphore_list->logMutex);
  writingNewCarInSharedMem(team_list, &car6, inf_fich, "Porto", semaphore_list->writingMutex, semaphore_list->logMutex);
  writingNewCarInSharedMem(team_list, &car7, inf_fich, "Benfica", semaphore_list->writingMutex, semaphore_list->logMutex);
  writingNewCarInSharedMem(team_list, &car8, inf_fich, "Benfica", semaphore_list->writingMutex, semaphore_list->logMutex);
  writingNewCarInSharedMem(team_list, &car9, inf_fich, "Porto", semaphore_list->writingMutex, semaphore_list->logMutex);


  pid_t childpid, wpid;


  int status = 0;

  //Creates the team processes, 1 for each team.
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

 //the father process waits for all his children to die :(
 while ((wpid = wait(&status)) > 0);
 #ifdef DEBUG
 printf("Race Manager %ld is out!\n", (long)getpid());
 #endif
 exit(0);

}
