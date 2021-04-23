//Edgar Filipe Ferreira Duarte 2019216077
//Pedro Guilherme da Cruz Ferreira 2018277677

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#include "RaceSimulator.h"
#include "MultipleProcessActions.h"
#include "TeamManager.h"

struct config_fich_struct *inf_fich;
struct team *team_list;
struct semaphoreStruct *semaphore_list;
int fd;

void endRaceManager(int signum){
  pid_t wpid;
  int status = 0;

  fclose(fd);

  while ((wpid = wait(&status)) > 0);
  printf("racemanager died\n");
  exit(0);
}

void Race_Manager(struct config_fich_struct *inf_fichP, struct team *team_listP,  struct semaphoreStruct *semaphore_listP){

  signal(SIGINT, endRaceManager);
  signal(SIGTSTP, SIG_IGN);

  printf("Creating named pipe.\n");
    if ((mkfifo(PIPE_NAME, O_CREAT|O_EXCL|0600)<0) && (errno!= EEXIST)) {
      perror("Cannot create pipe: ");
      exit(0);
  }
  printf("Named pipe created.\n");

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
//  struct car car10 = {7,70,70,70,70,70,70,"SAFETY MODE"};
//  struct car car11 = {8,80,80,80,80,80,80,"RACING"};
//  struct car car12 = {9,90,90,90,90,90,90,"ENDED"};


  writingNewCarInSharedMem(team_list, &car1, inf_fich, "Sporting", semaphore_list);
  writingNewCarInSharedMem(team_list, &car2, inf_fich, "Sporting", semaphore_list);
  writingNewCarInSharedMem(team_list, &car3, inf_fich, "Sporting", semaphore_list);
  writingNewCarInSharedMem(team_list, &car4, inf_fich, "Porto", semaphore_list);
  writingNewCarInSharedMem(team_list, &car5, inf_fich, "Porto", semaphore_list);
  writingNewCarInSharedMem(team_list, &car6, inf_fich, "Porto", semaphore_list);
  writingNewCarInSharedMem(team_list, &car7, inf_fich, "Benfica", semaphore_list);
  writingNewCarInSharedMem(team_list, &car8, inf_fich, "Benfica", semaphore_list);
  writingNewCarInSharedMem(team_list, &car9, inf_fich, "Benfica", semaphore_list);
//  writingNewCarInSharedMem(team_list, &car10, inf_fich, "Sporting", semaphore_list);
//  writingNewCarInSharedMem(team_list, &car11, inf_fich, "Boavista", semaphore_list);
//  writingNewCarInSharedMem(team_list, &car12, inf_fich, "Boavista", semaphore_list);


  pid_t childpid, wpid;


  int status = 0;
  int start = 0;

  while(1){

    char teamName[50];
    char received[512];
    /*NAMED PIPE*/
    //if ((mkfifo("NAMED_PIPE", O_CREAT|O_EXCL|0600)<0) && (errno!= EEXIST)) {
    //  perror("Cannot create pipe: ");
    //  exit(0);
  //  }
    // Opens the pipe for reading

    int fd;
    if ((fd = open(PIPE_NAME, O_RDONLY)) < 0) {
      perror("Cannot open pipe for reading: ");
      exit(0);
    }
    // Do some work

      read(fd, received, sizeof(received));
      printf("[RaceManager] Received (%s)\n", received);
    /*NAMED PIPE*/

    //char received[512] ="ADDCAR TEAM: A, CAR: 20, SPEED: 30, CONSUMPTION: 0.04, RELIABILITY: 95";
    //char received[512] ="START RACE!";
    if(strcmp(received,"START RACE!")==0){
        //verificar se o numero de equipas é suficiente => erro no ecra e nos logs
        if(start == 0){
          int n_equipas=0;
          for(int i=0; i<inf_fich->number_of_teams; i++){
            if(strcmp(team_list[i].team_name,"")!=0){
              n_equipas++;
            }
          }
          if(n_equipas!=inf_fich->number_of_teams){
            printf("CANNOT START, NOT ENOUGH TEAMS\n");
            writeLog("CANNOT START, NOT ENOUGH TEAMS",semaphore_list->writingMutex);
          }
          else{
              //começar corrida
          }
        }
        else if(start == 1){
          printf("Rejected, race already started!\n");
        }
    }
    else{ // Check if it is ADDCAR
      struct car *newCar = (struct car *) malloc(sizeof(struct car));

      int validation = sscanf(received,"ADDCAR TEAM: %[^,], CAR: %d, SPEED: %d, CONSUMPTION: %f, RELIABILITY: %d",teamName,&(newCar->car_number),&(newCar->speed),&(newCar->consumption),&(newCar->reliability));

      if( validation != 5 ||
        newCar->car_number < 0 ||
        newCar->speed <= 0 ||
        newCar->consumption < 0 ||
        newCar->consumption > inf_fich->fuel_capacity ||
        newCar->reliability < 0 ){
        char wrong_command_string[530] = "";
        strcat(wrong_command_string,"WRONG COMMAND => ");
        strcat(wrong_command_string,received);
        printf("%s\n",wrong_command_string);
        writeLog(wrong_command_string,semaphore_list->writingMutex);
      }
      else{
        if(start == 1){ //Race already started
            printf("%s => ",received);
            printf("Rejected, race already started!\n");
            writeLog("Rejected, race already started!",semaphore_list->writingMutex);
        }
        else{ //ADD CAR


          int teamCreated = writingNewCarInSharedMem(team_list, newCar, inf_fich, teamName, semaphore_list);

          if(teamCreated ==1){

            int channel[2];

            pipe(channel);

            if(fork()==0){

              Team_Manager(inf_fich, team_list, semaphore_list,channel);
              #ifdef DEBUG
              printf("Team Manager %ld is out!\n", (long)getpid());
              #endif
              exit(0);
            }

            //Le do pipe
            int n;
            read(channel[0],&n,sizeof(n));
            printf("Race Manager leu %d.\n",n);
            }
          }
        }
    }
    }

 //the father process waits for all his children to die :(
 while ((wpid = wait(&status)) > 0);
 #ifdef DEBUG
 printf("Race Manager %ld is out!\n", (long)getpid());
 #endif
 exit(0);

}
