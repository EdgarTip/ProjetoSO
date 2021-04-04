#define DEBUG
#define SIZE 50


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/fcntl.h>
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

  //Only for debug purposes
  #ifdef DEBUG
  printf("%ld is writing something to the log.txt file\n", (long)getpid());
  #endif

  system(buffer);

  sem_post(mutex);

}


void writingNewCarInSharedMem(struct team *team_list, struct car *new_car, struct config_fich_struct *inf_fich, char *team_name, sem_t *mutex){


  sem_wait(mutex);

  for(int i=0; i<inf_fich->number_of_teams; i++){
    //a team with the given name was found
    if(strcmp(team_list[i].team_name,team_name)==0){
      for(int j = 0; j<inf_fich->number_of_cars; j++){

        if(team_list[i].cars[j].speed == 0){
          team_list[i].cars[j] = *new_car;
          sem_post(mutex);
          return;
        }
      }
      printf("WARNING! THERE WAS NO SPACE IN THE SELECTED TEAM. THE CAR WILL NOT BE CREATED!\n");
      sem_post(mutex);
      return;
    }
    //No team with the given name exist and there is enough space to create a new team
    else if(strcmp(team_list[i].team_name,"") == 0){
      strcpy(team_list[i].team_name, team_name);
      strcpy(team_list[i].box_state, "OPEN");
      team_list[i].cars[0] = *new_car;
      sem_post(mutex);
      return;
    }
  }
  printf("WARNING! THERE WAS NO SPACE TO CREATE A NEW TEAM. THE CAR WILL NOT BE CREATED!\n");
  sem_post(mutex);
}
