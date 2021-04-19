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

  struct car car1 = {1,10,10,10,10,10,10,"WAITING"};
  struct car car2 = {2,20,20,20,20,20,20,"WAITING"};
  struct car car3 = {3,30,30,30,30,30,30,"RACING"};
  struct car car4 = {4,40,40,40,40,40,40,"IN BOX"};
  struct car car5 = {5,50,50,50,50,50,50,"RACING"};
  struct car car6 = {6,60,60,60,60,60,60,"GAVE UP"};
  struct car car7 = {7,70,70,70,70,70,70,"SAFETY MODE"};
  struct car car8 = {8,80,80,80,80,80,80,"RACING"};
  struct car car9 = {9,90,90,90,90,90,90,"ENDED"};
  struct car car10 = {7,70,70,70,70,70,70,"SAFETY MODE"};
  struct car car11 = {8,80,80,80,80,80,80,"RACING"};
  struct car car12 = {9,90,90,90,90,90,90,"ENDED"};


  writingNewCarInSharedMem(team_list, &car1, inf_fich, "Sporting", semaphore_list);
  writingNewCarInSharedMem(team_list, &car2, inf_fich, "Sporting", semaphore_list);
  writingNewCarInSharedMem(team_list, &car3, inf_fich, "Sporting", semaphore_list);
  writingNewCarInSharedMem(team_list, &car4, inf_fich, "Porto", semaphore_list);
  writingNewCarInSharedMem(team_list, &car5, inf_fich, "Porto", semaphore_list);
  writingNewCarInSharedMem(team_list, &car6, inf_fich, "Porto", semaphore_list);
  writingNewCarInSharedMem(team_list, &car7, inf_fich, "Benfica", semaphore_list);
  writingNewCarInSharedMem(team_list, &car8, inf_fich, "Benfica", semaphore_list);
  writingNewCarInSharedMem(team_list, &car9, inf_fich, "Benfica", semaphore_list);
  writingNewCarInSharedMem(team_list, &car10, inf_fich, "Sporting", semaphore_list);
  writingNewCarInSharedMem(team_list, &car11, inf_fich, "Boavista", semaphore_list);
  writingNewCarInSharedMem(team_list, &car12, inf_fich, "Boavista", semaphore_list);


  pid_t childpid, wpid;


  int status = 0;
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
