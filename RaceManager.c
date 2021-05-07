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

struct ids *ids_P;

int *pids;
int *pipes;

void my_handler(int signum)
{

    if (signum == SIGUSR1)
    {
      #ifdef DEBUG
        printf("Received SIGUSR1! => Interromper corrida\n");
      #endif
    }
}


void endRaceManager(int signum){
  pid_t wpid;
  int status = 0;

  for(int i = 0; i < inf_fich->number_of_teams; i++){
    kill(pids[i], SIGUSR2);
  }
  free(pids);
  while ((wpid = wait(&status)) > 0);

  #ifdef DEBUG
    printf("RaceManeger is out.\n");
  #endif

  exit(0);
}


void endRace(){
  kill(ids_P->pid_breakdown, SIGUSR2);
  endRaceManager(0);
}

void interruptRace(int signum){
  printf("TO DO\n");
}

int getFreeChannel(int n, int pipes[n]){
    for(int i=0;i<n;i++){
        if(pipes[i]==-1){
          return i;
        }
    }
    return -1;

}

int getPipesCreated(int n, int pipes[n]){
  int total=0;
  for(int i=0;i<n;i++){
    if(pipes[i]!=-1){
      total++;
    }
  }
  return total;
}

void Race_Manager(struct config_fich_struct *inf_fichP, struct team *team_listP,  struct semaphoreStruct *semaphore_listP, struct ids *idsP){

  char log[550];
  sigset_t mask, new_mask;

  ids_P = idsP;
  //Ignore all unwanted signals!
  sigfillset(&mask);
  sigprocmask(SIG_SETMASK, &mask, NULL);


  sigemptyset(&new_mask);
  sigaddset(&new_mask,SIGUSR1);
  sigaddset(&new_mask,SIGUSR2);

  sigprocmask(SIG_UNBLOCK,&new_mask, NULL);

  signal(SIGUSR1, interruptRace);
  signal(SIGUSR2, endRaceManager);


   #ifdef DEBUG
    printf("Race_Manager created (%ld)\n",(long)getpid());
   #endif

  inf_fich = inf_fichP;
  team_list = team_listP;
  semaphore_list = semaphore_listP;

  pipes=malloc(sizeof(*pipes)*(inf_fich->number_of_teams+1));
  for(int i=0;i<inf_fich->number_of_teams+1;i++){
    pipes[i]=-1;
  }

  pthread_t team_threads[inf_fich->number_of_teams];
  pids = malloc(sizeof(int) * inf_fich->number_of_teams);
  //For testing purposes!

  pid_t childpid, wpid;


  int status = 0;
  int start = 0;
  int i = 0;
  int index_team = 0;

  #ifdef DEBUG
    printf("Opening named pipe for reading\n");
  #endif
  int fd;
  if ((fd= open(PIPE_NAME, O_RDONLY)) < 0) {
    perror("Cannot open pipe for reading: ");
    exit(0);
  }
  pipes[0]=fd;
  #ifdef DEBUG
    printf("Named pipe open for reading\n");
  #endif


  char teamName[50];
  char received[512];
  int n=1;

  while(1){
    fd_set read_set;
    FD_ZERO(&read_set);
    for(int channel=0;channel<inf_fich->number_of_teams+1;channel++){
      FD_SET(pipes[channel], &read_set);
    }
    if (select(pipes[n-1]+1, &read_set, NULL, NULL, NULL) > 0 ) { //NAO FAÇO -1 PQ O "PIPES" TEM NUMBER_OF_TEAMS + 1 (NAMED)
        if(FD_ISSET(pipes[0], &read_set)){

          read(pipes[0],received,sizeof(received));
          #ifdef DEBUG
            printf("[RaceManager] (NP) Received: %s\n",received);
          #endif

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
                  //printf("CANNOT START, NOT ENOUGH TEAMS\n");
                  writeLog("CANNOT START, NOT ENOUGH TEAMS",semaphore_list->logMutex,inf_fich->fp);
                }
                else{
                  //Notificar os TeamManagers do inicio da corrida
                    for(int i = 0; i<inf_fich->number_of_teams; i++){
                      kill(pids[i], SIGTERM);
                    }

                    kill(idsP->pid_breakdown, SIGTERM);

                    start = 1;
                }
              }
              else if(start == 1){
                sprintf(log,"%s => Rejected, race already started!",received);
                writeLog(log,semaphore_list->logMutex,inf_fich->fp);
              }
          } //is START RACE!
          else{ // Check if it is ADDCAR
            struct car *newCar = (struct car *) malloc(sizeof(struct car));

            int validation = sscanf(received,"ADDCAR TEAM: %[^,], CAR: %d, SPEED: %d, CONSUMPTION: %f, RELIABILITY: %d",teamName,&(newCar->car_number),&(newCar->speed),&(newCar->consumption),&(newCar->reliability));

            if( validation != 5 ||
              newCar->car_number < 0 ||
              newCar->speed <= 0 ||
              newCar->consumption < 0 ||
              newCar->consumption > inf_fich->fuel_capacity ||
              newCar->reliability < 0 ){

              sprintf(log,"WRONG COMMAND => %s",received);
              writeLog(log,semaphore_list->logMutex,inf_fich->fp);
            }
            else{
              if(start == 1){ //Race already started
                sprintf(log,"%s => Rejected, race already started!",received);
                writeLog(log, semaphore_list->logMutex, inf_fich->fp);
              }
              else{ //ADD CAR

                newCar->number_of_laps = 0;
                newCar->amount_breakdown = 0;
                newCar->times_refill = 0;
                newCar->has_breakdown = 0;
                strcpy(newCar->current_state, "CORRIDA");

                int teamCreated = writingNewCarInSharedMem(team_list, newCar, inf_fich, teamName, semaphore_list);

                if(teamCreated ==1){

                  #ifdef DEBUG
                    printf("Getting channel for new team.\n");
                  #endif

                  int c=getFreeChannel(inf_fich->number_of_teams+1,pipes);

                  int channel[2];
                  pipe(channel);
                  n++;
                  pipes[c]=channel[0];

                  if((pids[i] =fork())==0){


                    Team_Manager(inf_fich, team_list, semaphore_list,channel,index_team,idsP);

                  }
		              i++;
		              index_team++;
                  //close(channel[1]);

                  }
                }
              }
          } //ISNt START RACE!

        }//isset
        //else{
          for(int channel=1;channel<(inf_fich->number_of_teams+1);channel++){
            if(FD_ISSET(pipes[channel], &read_set)){

              struct message data;

              read(pipes[channel],&data,sizeof(struct message));
              #ifdef DEBUG
              printf("[RaceManager] (%d) Received %d, %d ,%s.\n",channel,data.car_index, data.team_index,data.message);
              #endif

              printf("MENSAGEM RECEBIDA %s\n",data.message);
              if(strcmp(data.message,"TERMINADO") == 0){
                endRace();
              }
            }
          }
        //}


    } //select
} //while

}
