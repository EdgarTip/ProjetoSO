//Edgar Filipe Ferreira Duarte 2019216077
//Pedro Guilherme da Cruz Ferreira 2018277677

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#include "RaceSimulator.h"
#include "MultipleProcessActions.h"
#include "TeamManager.h"

struct config_fich_struct *inf_fich;
struct team *team_list;
struct semaphoreStruct *semaphore_list;

void endRaceManager(int signum){
  pid_t wpid;
  int status = 0;

  while ((wpid = wait(&status)) > 0);
  printf("racemanager died\n");
  exit(0);
}

void Race_Manager(struct config_fich_struct *inf_fichP, struct team *team_listP,  struct semaphoreStruct *semaphore_listP){

  signal(SIGINT, endRaceManager);
  signal(SIGTSTP, SIG_IGN);


  #ifdef DEBUG
  printf("Race_Manager created with id: %ld\n",(long)getpid());
  #endif

  inf_fich = inf_fichP;
  team_list = team_listP;
  semaphore_list = semaphore_listP;

  //For testing purposes!

  struct car car1 = {10,70,60,19,0,0,0};
  struct car car2 = {20,90,10,90,0,0,0};
  struct car car3 = {100,20,30,80,0,0,0};
  struct car car4 = {50,30,10,70,0,0,0};
  struct car car5 = {60,60,60,60,0,0,0};
  struct car car6 = {30,90,70,65,0,0,0};
  struct car car7 = {40,40,100,20,0,0,0};
  struct car car8 = {50,90,30,50,0,0,0};
  struct car car9 = {70,50,10,35,0,0,0};

  printf("eoiuioe uioewquio \n");
  writingNewCarInSharedMem(team_list, &car1, inf_fich, "Sporting", semaphore_list);
  writingNewCarInSharedMem(team_list, &car2, inf_fich, "Porto", semaphore_list);
  writingNewCarInSharedMem(team_list, &car3, inf_fich, "Sporting", semaphore_list);
  writingNewCarInSharedMem(team_list, &car4, inf_fich, "Benfica", semaphore_list);
  writingNewCarInSharedMem(team_list, &car5, inf_fich, "Sporting", semaphore_list);
  writingNewCarInSharedMem(team_list, &car6, inf_fich, "Porto", semaphore_list);
  writingNewCarInSharedMem(team_list, &car7, inf_fich, "Benfica", semaphore_list);
  writingNewCarInSharedMem(team_list, &car8, inf_fich, "Benfica", semaphore_list);
  writingNewCarInSharedMem(team_list, &car9, inf_fich, "Porto", semaphore_list);

  pid_t childpid, wpid;


  int status = 0;
  printf("eoiuioe uioewquio \n");
  printf("NUMBER TEAMS %d\n", inf_fich->number_of_teams);
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
