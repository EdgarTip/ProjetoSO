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
  printf("%d\n",inf_fich->number_of_teams);

  sem_wait(semaphore_list->writingMutex);

  char carLog[200];
  for(int i=0; i<inf_fich->number_of_teams; i++){
    //a team with the given name was found
    printf("attemp %d",i);
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



void getTop5Teams(struct config_fich_struct *inf_fich, struct team *team_list, int top5Teams[5][2]){

  for(int i= 0; i < inf_fich->number_of_teams; i++){

    for(int j = 0; j < team_list[i].number_of_cars; j++){
      for(int k = 0; k < 5; k++){

        //First sees if there is an empty space, then sees if its value is superior to another
        if(top5Teams[k][0]==-1){
          top5Teams[k][0] = i;
          top5Teams[k][1] = j;
          break;
        }
        else if(team_list[i].cars[j].number_of_laps > team_list[top5Teams[k][0]].cars[top5Teams[k][1]].number_of_laps){
          int temp1 = top5Teams[k][0];
          int temp2 = top5Teams[k][1];

          int temp3 = 0;
          int temp4 = 0;
          //get the index to the team and car of the current value
          top5Teams[k][0] = i;
          top5Teams[k][1] = j;

          for(int l = k+1; l< 5;l++){
            if(top5Teams[l][0] == -1){
              top5Teams[l][0] = temp1;
              top5Teams[l][1] = temp2;
              break;
            }
            temp3 = top5Teams[l][0];
            temp4 = top5Teams[l][1];

            top5Teams[l][0] = temp1;
            top5Teams[l][1] = temp2;

            temp1 = temp3;
            temp2 = temp4;
          }
          break;
        }
      }
    }
  }
}
//Prints the statistics of a race (could be midway or at the end). This has priority over writing actions
void readStatistics(struct config_fich_struct *inf_fich, struct team *team_list, struct semaphoreStruct *semaphore_list){


  sem_wait(semaphore_list->readingMutex);

  ++team_list[0].number_readers;

  if(team_list[0].number_readers == 1){
    sem_wait(semaphore_list->writingMutex);
  }

  sem_post(semaphore_list->readingMutex);

  //First line for team index second line for car index
  int top5Teams[5][2] = {{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}};

  getTop5Teams(inf_fich, team_list, top5Teams);


  for(int i = 1; i<=5; i++){
    printf("Lugar: %d, Número Carro: %d, Nome Equipa: %s, Número Voltas: %d, Número de Paragens na Box: %d\n", i, team_list[top5Teams[i][0]].cars[top5Teams[i][1]].car_number
                                                                                                              , team_list[top5Teams[i][0]].team_name
                                                                                                              , team_list[top5Teams[i][0]].cars[top5Teams[i][1]].number_of_laps
                                                                                                              , team_list[top5Teams[i][0]].cars[top5Teams[i][1]].amount_breakdown);
  }

  sem_wait(semaphore_list->readingMutex);

  --team_list[0].number_readers;

  if(team_list[0].number_readers == 0){
    sem_post(semaphore_list->writingMutex);
  }

  sem_post(semaphore_list->readingMutex);


}
