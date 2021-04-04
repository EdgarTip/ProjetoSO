#define DEBUG
#define SIZE 50


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h> // include POSIX semaphores
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include "RaceSimulator.h"

#include "RaceManager.h"
#include "ReadConfig.h"
#include "MultipleProcessActions.h"
#include "BreakDownManager.h"
/*Struct that retains the information given by the initial file
Not a shared memory struct but the values will be given to the
processes created in this script*/



/*Shared memory struct the will share memory about the current state of all
of the cars. Will be updated by the Race Manager
*/


struct config_fich_struct *inf_fich;

struct team *team_list;

struct semaphoreStruct *semaphore_list;


int shmid;

//PARA TESTE PARA APAGAR MAIS TARDE
void teste(){
  for(int i = 0; i< inf_fich->number_of_teams; i++){
    if(strcmp(team_list[i].team_name, "") == 0){
      return;
    }
    for(int j = 0; j < inf_fich->number_of_cars; j++){
      if(team_list[i].cars[j].speed ==0){
        break;
      }
      printf("Team name:%s\n Box state:%s\n Car number: %d\n Car speed: %d\n Car consumption: %d\n Car reliability: %d\n", team_list[i].team_name
                                                                                                                       , team_list[i].box_state
                                                                                                                       , team_list[i].cars[j].car_number
                                                                                                                       , team_list[i].cars[j].speed
                                                                                                                       , team_list[i].cars[j].consumption
                                                                                                                       , team_list[i].cars[j].reliability);
    }
    printf("-----------\n");
  }

}

void clean(){
  free(inf_fich);
  free(semaphore_list);
  shmdt(team_list);
  shmctl(shmid, IPC_RMID, NULL);
  sem_close(semaphore_list->mutexFicheiro);
  sem_close(semaphore_list->writingMutex);
  sem_unlink("MUTEX");
  sem_unlink("WRITING_MUTEX");
}

//Main function. Here the RaceManager and the MalfunctionManager processes will be created
int main(int argc, char* argv[]){
  system(">logs.txt");  //Limpa o ficheiro logs.txt

  semaphore_list = (struct semaphoreStruct*) malloc(sizeof(struct semaphoreStruct));
  sem_unlink("MUTEX");
  semaphore_list->mutexFicheiro = sem_open("MUTEX", O_CREAT|O_EXCL,0700,1);
  sem_unlink("WRITING_MUTEX");
  semaphore_list->writingMutex = sem_open("WRITING_MUTEX", 0_CREAT|0_EXCL,0700,1);

  if (argc!=2){
    writeLog("Error with input arguments. Execution aborted!", semaphore_list->mutexFicheiro);
  	printf("Invalid number of arguments!\nUse as: executable {name of the configurations file}\n");
  	exit(1);
   }

   //Read the configuration file
   char *file_name = argv[1];
   inf_fich=readConfigFile(file_name);


   writeLog("Configuration file read", semaphore_list->mutexFicheiro);




  //Creates shared memory
  shmid = shmget(IPC_PRIVATE, (sizeof(struct team*) + sizeof(struct car*) * inf_fich->number_of_cars)* inf_fich->number_of_teams, IPC_CREAT|0700);
  if (shmid < 1) exit(0);
  team_list = (struct team*) shmat(shmid, NULL, 0);
  if (team_list < (struct team*) 1) exit(0);

  for(int i = 0; i < inf_fich->number_of_teams ; i++){
      team_list[i].cars = (struct car*)(team_list + inf_fich->number_of_teams + i +1);
  }





  //Apenas para teste
  /*
  strcpy(team_list[0].team_name,"Sporting");
  strcpy(team_list[0].box_state, "OPEN" ) ;
  team_list[0].cars[0].speed = 10;
  team_list[0].cars[0].consumption = 70;
  team_list[0].cars[0].reliability = 60;
  team_list[0].cars[0].car_number = 19;

  team_list[0].cars[1].speed = 50;
  team_list[0].cars[1].consumption = 80;
  team_list[0].cars[1].reliability = 90;
  team_list[0].cars[1].car_number = 29;


  strcpy(team_list[1].team_name,"Benfica");
  strcpy(team_list[1].box_state, "Reservado");
  team_list[1].cars[0].speed = 50;
  team_list[1].cars[0].consumption = 10;
  team_list[1].cars[0].reliability = 40;
  team_list[1].cars[0].car_number = 9;

  strcpy(team_list[2].team_name,"Porto");
  strcpy(team_list[2].box_state, "OPEN" );
  team_list[2].cars[0].speed = 90;
  team_list[2].cars[0].consumption = 100;
  team_list[2].cars[0].reliability = 70;
  team_list[2].cars[0].car_number = 8;
  */
  int pid=fork();

  //Creates RaceManager and BreakDownManager
  if(pid==0){

    int pid2=fork();

    if(pid2==0){

      /*strcpy(team_list[4].team_name,"Boavista");
      strcpy(team_list[4].box_state, "OPEN" );
      team_list[4].cars[0].speed = 90;
      team_list[4].cars[0].consumption = 100;
      team_list[4].cars[0].reliability = 70;
      team_list[4].cars[0].car_number = 8;
      printf("---Gerador de Corrida.---\n");

      */
      Race_Manager(inf_fich, team_list, semaphore_list);

      #ifdef DEBUG
      printf("Race Manager is out!\n");
      #endif

      exit(0);
    }

    else{

      //printf("---Gerador de Avarias.---\n");
      /*strcpy(team_list[3].team_name,"Rio Ave");
      strcpy(team_list[3].box_state, "Reservado");
      team_list[3].cars[0].speed = 50;
      team_list[3].cars[0].consumption = 10;
      team_list[3].cars[0].reliability = 40;
      team_list[3].cars[0].car_number = 9;

      */
      BreakDownManager(inf_fich, team_list, semaphore_list);

      wait(NULL);
      exit(0);
    }
  }



  wait(NULL);
  clean();
  return 0;

}
