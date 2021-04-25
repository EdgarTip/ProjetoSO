//Edgar Filipe Ferreira Duarte 2019216077
//Pedro Guilherme da Cruz Ferreira 2018277677
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "RaceSimulator.h"
#include "MultipleProcessActions.h"



struct config_fich_struct *inf_fich;
struct team *team_list;
struct semaphoreStruct *semaphore_list;

pthread_t *cars;

int team_index;

int *channel;

int start_teams = 0;
int number_of_cars;

//Ends the team
void teamEnd(int signum){
  for(int j=0; j<number_of_cars; j++){
    pthread_cancel(cars[j]);
  }
  printf("Team %ld killed\n",(long)getpid());
  free(cars);
  printf("morreu\n");
  exit(0);
}

//Cars are racing
void racing(int arrayNumber){

  //Variables
  char logMessage[MAX];
  char update[MAX];
  float current_fuel = inf_fich->fuel_capacity;
  int distance_in_lap = 0;
  struct message data;

  //escrever para o gestor de equipas que o carro começou em corrida com o estado "CORRIDA"
  data.car_index = arrayNumber;
  data.team_index = team_index;
  strcpy(data.message,"CORRIDA");

  write(channel[1],&data, sizeof(data));

  while(1){

    printf("Current index of car in array: %d\n",arrayNumber);

    //Racing normally
    if (strcmp(team_list[team_index].cars[arrayNumber].current_state,"CORRIDA") == 0) {
      current_fuel -= team_list[team_index].cars[arrayNumber].consumption;
      distance_in_lap += team_list[team_index].cars[arrayNumber].speed;

    }
    //Needs to got to the box
    else if (strcmp(team_list[team_index].cars[arrayNumber].current_state,"SEGURANCA") == 0) {

      current_fuel -= 0.4 * team_list[team_index].cars[arrayNumber].consumption;
      distance_in_lap += 0.3 * team_list[team_index].cars[arrayNumber].speed;
    }

    //Gave up
    else if(strcmp(team_list[team_index].cars[arrayNumber].current_state,"DESISTENCIA") == 0){
      sprintf(logMessage, "Car %d from team %s gave up", team_list[team_index].cars[arrayNumber].car_number, team_list[team_index].team_name);
      writeLog(logMessage,semaphore_list->logMutex, inf_fich->fp);
      pthread_exit(NULL);
    }

    //para debugging apagar mais tarde!!!!!!!!!!!!!!!!!!!!!!!!
    else{
      printf("ERRO NO CODIGO!!!! %s RECEBIDO!\n",team_list[team_index].cars[arrayNumber].current_state);
    }
    
    printf("amount of fuel %.02f\n",current_fuel);

    //If the car does not have enough fuel for the next 4 laps. The car enters security mode
    if(current_fuel < 4 * ((inf_fich->lap_distance / team_list[team_index].cars[arrayNumber].speed)*team_list[team_index].cars[arrayNumber].consumption)){

      strcpy(data.message,"SEGURANCA");
      write(channel[1],&data, sizeof(data));

    }

    //The car ended the race!
    if(team_list[team_index].cars[arrayNumber].number_of_laps >= inf_fich->number_of_laps){
      printf("i'm out\n");
      break;
    }


    sleep(1/inf_fich->time_units_per_second);
  }

}




//Car thread. For now it dies immediatly after being created
void *carThread(void* team_number){

  int number=*((int *)team_number);

  #ifdef DEBUG
  //printf("I %ld created car %d.\n",(long)getpid(),number);
  #endif
  //printf("i am waiting\n");

  //Have a condition variable

  racing(number);

  pthread_exit(NULL);


}

//Signals the race manager that the race has started
void raceStart(int signum){
  start_teams = 1;
}

void teste(int signum){
  printf("wqe fuuiowqe we gioq ioeg uio io\n");
}
//Team manager. Will create the car threads
void Team_Manager(struct config_fich_struct *inf_fichP, struct team *team_listP, struct semaphoreStruct *semaphore_listP, int channelP[2],int team_indexP){
  signal(SIGUSR2, teamEnd);
  signal(SIGUSR1, SIG_IGN);
  signal(SIGTSTP, teste);
  signal(SIGTERM, raceStart);



  #ifdef DEBUG
  printf("Team Manager created with id: %ld\n", (long)getpid());
  #endif

  channel = channelP;

  inf_fich = inf_fichP;
  team_list = team_listP;
  semaphore_list = semaphore_listP;
  team_index = team_indexP;

  //waits for race to start
  while(start_teams == 0){
    pause();
  }


  signal(SIGTERM, SIG_IGN);

  number_of_cars = team_list[team_index].number_of_cars;

  int workerId[number_of_cars];

  cars = malloc(sizeof(pthread_t) * number_of_cars);


  //Create the car threads
  for(int i=0; i<number_of_cars;i++){
    workerId[i] = i;
    pthread_create(&cars[i], NULL, carThread,&workerId[i]);
  }
  //Waits for all the cars to die
  for(int j=0; j<number_of_cars; j++){
    pthread_join(cars[j],NULL);
  }

  free(cars);
  printf("I am dying\n");

  return;
}
