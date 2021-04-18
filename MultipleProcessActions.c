//Edgar Filipe Ferreira Duarte 2019216077
//Pedro Guilherme da Cruz Ferreira 2018277677

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <semaphore.h>
#include "RaceSimulator.h"

void writeLog(char * string, sem_t *mutex){
  char buffer[200];


  sprintf(buffer, "date|cut -c17-24 >> logs.txt ; sed -i '$ s/$/ %s/' logs.txt; ",string);


  /* https://unix.stackexchange.com/questions/412835/append-text-with-echo-without-new-line
  sed -i '$ s/$/abc/' file.txt
      -i - modify the file inplace
      $ - indicate the last record/line
      s/$/abc/ - substitute the end of the line $ with substring abc (for the last record)

  */

  sem_wait(mutex);

  system(buffer);

  sem_post(mutex);

}


void writingNewCarInSharedMem(struct team *team_list, struct car *new_car, struct config_fich_struct *inf_fich, char *team_name, struct semaphoreStruct *semaphore_list){

  sem_wait(semaphore_list->writingMutex);
  char carLog[200];
  for(int i=0; i<inf_fich->number_of_teams; i++){
    //a team with the given name was found
    if(strcmp(team_list[i].team_name,team_name)==0){
      for(int j = 0; j<inf_fich->number_of_cars; j++){

        //Sees if there is space for the car
        if(team_list[i].cars[j].speed == 0){
          team_list[i].number_of_cars++;
          team_list[i].cars[j] = *new_car;

          sem_post(semaphore_list->writingMutex);
          sprintf(carLog,"NEW CAR LOADED => TEAM: %s, CAR: %d, SPEED: %d, CONSUMPTION: %.2f, RELIABILITY: %d",team_name,new_car->car_number, new_car->speed,new_car->consumption,new_car->reliability);
          writeLog(carLog,semaphore_list->logMutex);
          return;
        }
      }
      printf("WARNING! THERE WAS NO SPACE IN THE SELECTED TEAM. THE CAR WILL NOT BE CREATED!\n");
      sem_post(semaphore_list->writingMutex);
      return;
    }
    //No team with the given name exist and there is enough space to create a new team
    else if(strcmp(team_list[i].team_name,"") == 0){

      strcpy(team_list[i].team_name, team_name);
      strcpy(team_list[i].box_state, "OPEN");
      team_list[i].number_readers = 0;
      team_list[i].cars[0] = *new_car;
      team_list[i].number_of_cars = 1;

      sem_post(semaphore_list->writingMutex);
      sprintf(carLog,"NEW CAR LOADED => TEAM: %s, CAR: %d, SPEED: %d, CONSUMPTION: %.2f, RELIABILITY: %d",team_name,new_car->car_number, new_car->speed,new_car->consumption,new_car->reliability);
      writeLog(carLog,semaphore_list->logMutex);
      return;
    }
  }
  printf("WARNING! THERE WAS NO SPACE TO CREATE A NEW TEAM. THE CAR WILL NOT BE CREATED!\n");
  sem_post(semaphore_list->writingMutex);
}

//Reads from the shared memory
void readSharedmemory(struct team *team_list, struct semaphoreStruct *semaphore_list){



}
//Prints the statistics of a race (could be midway or at the end). This has priority over writing actions
void readStatistics(struct team *team_list, struct semaphoreStruct *semaphore_list){

  sem_wait(semaphore_list.readingMutex);

  ++team_list.team[0].number_readers;

  if(team_list.team[0].number_readers == 1){
    sem_wait(semaphore_list.writingMutex);
  }

  sem_post(semaphore_list.readingMutex);

  int top5Teams[5][2] = {{NULL,NULL},{NULL,NULL},{NULL,NULL},{NULL,NULL},{NULL,NULL}};
  for( int i= 0; i < )

}
