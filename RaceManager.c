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
#include <pthread.h>

#include "RaceSimulator.h"
#include "MultipleProcessActions.h"
#include "TeamManager.h"

struct config_fich_struct *inf_fich;
struct team *team_list;
struct semaphoreStruct *semaphore_list;
int fd;
int *pids;
int named_pipe_fd;
int (*channel)[2];

void endRaceManager(int signum){
  pid_t wpid;
  int status = 0;

  for(int i = 0; i < inf_fich->number_of_teams; i++){
    kill(pids[i], SIGUSR2);
  }
  free(pids);
  while ((wpid = wait(&status)) > 0);
  printf("racemanager died\n");
  exit(0);
}
void interruptRace(int signum){
  printf("TO DO\n");
}

int getFreeChannel(int n, int channel[n][2]){
    for(int i=0;i<inf_fich->number_of_teams;i++){
        if(channel[i][0]==0){
          return i;
        }
    }
    return -1;

}

void *teamThread(void *arg){
   int channel_number = *((int *) arg);
  close(channel[channel_number][1]);  //close writing edge
  int n=0;
  printf("Estou num ciclo a ler o channel da team %d\n",channel_number);
    while (1) {
      	fd_set read_set;
        FD_ZERO(&read_set);
        FD_SET(channel[channel_number][0], &read_set);
        if (select(channel[channel_number][0]+1, &read_set, NULL, NULL, NULL) > 0 ) {
          read(channel[channel_number][0],&n,sizeof(int));
          printf("Race Manager (%d) leu %d.\n",channel_number,n);
       }
             //sleep(1);
      }

}

void Race_Manager(struct config_fich_struct *inf_fichP, struct team *team_listP,  struct semaphoreStruct *semaphore_listP){

  signal(SIGINT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGUSR1, interruptRace);
  signal(SIGUSR2, endRaceManager);

  printf("Creating named pipe.\n");

  printf("Named pipe created.\n");

  #ifdef DEBUG
  printf("Race_Manager created with id: %ld\n",(long)getpid());
  #endif

  inf_fich = inf_fichP;
  team_list = team_listP;
  semaphore_list = semaphore_listP;

  channel=malloc(sizeof(*channel)*inf_fich->number_of_teams);
  for(int i=0;i<inf_fich->number_of_teams;i++){
    channel[i][0]=0;
    channel[i][1]=0;
  }

  pthread_t team_threads[inf_fich->number_of_teams];

  pids = malloc(sizeof(int) * inf_fich->number_of_teams);
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
// writingNewCarInSharedMem(team_list, &car10, inf_fich, "Sporting", semaphore_list);
//  writingNewCarInSharedMem(team_list, &car11, inf_fich, "Boavista", semaphore_list);
//  writingNewCarInSharedMem(team_list, &car12, inf_fich, "Boavista", semaphore_list);


  pid_t childpid, wpid;



  if ((named_pipe_fd = open(PIPE_NAME, O_RDONLY)) < 0) {
    perror("Cannot open pipe for reading: ");
    exit(0);
  }

  int status = 0;
  int start = 0;

  int i = 0;
  int index_team = 0;

  char teamName[50];
  char received[512];
  while(1){
    // Do some work

    read(named_pipe_fd, received, sizeof(received));
    printf("[RaceManager] Received (%s)\n", received);
    /*NAMED PIPE*/

    //char received[512] ="ADDCAR TEAM: A, CAR: 20, SPEED: 30, CONSUMPTION: 0.04, RELIABILITY: 95";
    //char received[512] ="ADDCAR TEAM: C, CAR: 21, SPEED: 30, CONSUMPTION: 0.04, RELIABILITY: 95";
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
            writeLog("CANNOT START, NOT ENOUGH TEAMS",semaphore_list->logMutex,inf_fich->fp);
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
        writeLog(wrong_command_string,semaphore_list->logMutex,inf_fich->fp);
      }
      else{
        if(start == 1){ //Race already started
            printf("%s => ",received);
            printf("Rejected, race already started!\n");
            writeLog("Rejected, race already started!",semaphore_list->logMutex,inf_fich->fp);
        }
        else{ //ADD CAR


          int teamCreated = writingNewCarInSharedMem(team_list, newCar, inf_fich, teamName, semaphore_list);

          if(teamCreated ==1){
            int c=getFreeChannel(inf_fich->number_of_teams,channel);
            if(c==-1){
              printf("Não há channel para equipa\n"); //PARA TESTES, ISTO NUNCA VAI ACONTECER NA VERSAO FINAL PQ NAO HÁ EQUIPAS PRE CRIADAS
              exit(0);
            }
            pipe(channel[c]);
            //printf("Channel: %d %d\n",channel[c][0],channel[c][1]);

            pthread_create(&team_threads[c], NULL, teamThread,&c);
            if((pids[i] =fork())==0){


              Team_Manager(inf_fich, team_list, semaphore_list,channel[c],index_team);
              #ifdef DEBUG
              printf("Team Manager %ld is out!\n", (long)getpid());
              #endif
              pthread_join(team_threads[c],NULL);   //Quebrar a thread de alguma forma
              exit(0);
            }
            i++;
            index_team++;
            }
          }
        }
    }


    }

 //the father process waits for all his children to die :(
 while ((wpid = wait(&status)) > 0);

for(int i=0;i<inf_fich->number_of_teams;i++){
  close(channel[i][0]);
  close(channel[i][1]);
}
 free(channel);
 #ifdef DEBUG
 printf("Race Manager %ld is out!\n", (long)getpid());
 #endif
 exit(0);

}
