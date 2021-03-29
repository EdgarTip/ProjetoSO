#define DEBUG



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

#include "RaceManager.h"


/*Struct that retains the information given by the initial file
Not a shared memory struct but the values will be given to the
processes created in this script*/



/*Shared memory struct the will share memory about the current state of all
of the cars. Will be updated by the Race Manager
*/


struct config_fich_struct *inf_fich;

struct team *team_list;


int shmid;


void teste(){
  for(i = 0; i< inf_fich->number_of_teams; i++){
    if(sizeof(team_list[i].team_name) == 0){
      return;
    }
    for(j = 0; j < inf_fich->number_of_cars){
      if(sizeof(team_list[i].cars[j].is_empty) ==0){
        break;
      }
      printf("Team name:%s\n Box state:%s\n Car number: %d\n Car speed: %d\n Car consumption: %d\n Car reliability: %d", team_list[i]->team_name
                                                                                                                       , team_list[i]->.box_state
                                                                                                                       , team_list[i]->cars[j]->car_number
                                                                                                                       , team_list[i]->cars[j]->speed
                                                                                                                       , team_list[i]->cars[j]->consumption
                                                                                                                       , team_list[i]->cars[j]->reliability);

    }
  }

}
//Reads the initial file and gives values to the inf_fich struct
int readConfigFile(){
  FILE *fp;

  #ifndef DEBUG
  printf("Leitura do ficheiro de configurações");
  #endif

  fp = fopen("/home/user/github/Race_Manager/configs.txt", "r"); // read mode

  if (fp == NULL)
  {
     perror("Error while opening the file.\n");
     exit(EXIT_FAILURE);
  }
  printf("Ficheiro aberto com exito\n");


  fscanf(fp,"%d\n%d, %d\n%d\n%d\n%d\n%d, %d\n%d",&(inf_fich->time_units_per_second),
                                             &(inf_fich->lap_distance),
                                             &(inf_fich->number_of_laps),
                                             &(inf_fich->number_of_teams),
                                             &(inf_fich->number_of_cars),
                                             &(inf_fich->T_Avaria),
                                             &(inf_fich->T_Box_Min),
                                             &(inf_fich->T_Box_Max),
                                             &(inf_fich->fuel_capacity));

  fclose(fp);
  return 0;
}


//Main function. Here the RaceManager and the MalfunctionManager processes will be created
int main(){

  //Initialize the inf_fich struct and populate it
  inf_fich = (struct config_fich_struct*) malloc(sizeof(struct config_fich_struct));
  readConfigFile();


  //Creates shared memory
  shmid = shmget(IPC_PRIVATE, (sizeof(struct team*) + sizeof(struct car) * inf_fich->number_of_cars)* inf_fich->number_of_teams, IPC_CREAT|0700);
  if (shmid < 1) exit(0);
  team_list = (struct team*) shmat(shmid, NULL, 0);

  for(int i = 0; i < inf_fich->number_of_teams ; i++){
      struct car *car_list = (struct car*) malloc(sizeof(struct car) * inf_fich->number_of_cars);
      team_list[i].cars = car_list;
  }


  system("date|cut -c17-24 >> logs.txt");

  //Apenas para teste
  strcpy(team_list[0].team_name,"Sporting");
  strcpy(team_list[0].box_state, "OPEN" ) ;
  strcpy(team_list[0].cars[0].is_empty , "N");
  team_list[0].cars[0].speed = 10;
  team_list[0].cars[0].consumption = 70;
  team_list[0].cars[0].reliability = 60;
  team_list[0].cars[0].car_number = 1;


  int pid=fork();

  if(pid!=0){
    printf("Processo main\n");
  }
  else{
    int pid2=fork();
    if(pid2==0){
      sleep(3);
      printf("Gerador de Corrida.\n");
      strcpy(team_list[1].team_name,"Porto");
      strcpy(team_list[1].box_state, "OPEN" );
      strcpy(team_list[0].cars[0].is_empty , "N");
      team_list[1].cars[0].speed = 90;
      team_list[1].cars[0].consumption = 100;
      team_list[1].cars[0].reliability = 70;
      team_list[1].cars[0].car_number = 8;

      //Race_Manager(inf_fich->number_of_teams, inf_fich->number_of_cars);

      printf("Gerador de Corrida is out!");
      exit(0);
    }
    else{
      sleep(2);
      printf("Gerador de Corrida.\n");
      strcpy(team_list[2].team_name,"Benfica");
      strcpy(team_list[2].box_state, "Reservado");
      strcpy(team_list[0].cars[0].is_empty , "N");
      team_list[2].cars[0].speed = 50;
      team_list[2].cars[0].consumption = 10;
      team_list[2].cars[0].reliability = 40;
      team_list[2].cars[0].car_number = 9;
      printf("Gerador de Avarias.\n");
      exit(0);
    }
  }

  printf("---------MAIN-------\n");

  sleep(7);


  for(int i = 0; i < inf_fich->number_of_cars; i++){
    printf("nome :%s,car number: %d, speed : %d, consumption: %d,reliability%d \n",
    car_list->team_name,
    car_list[i].car_number,
    car_list[i].speed,
    car_list[i].consumption,
    car_list[i].reliability);
  }

  free(inf_fich);
  shmdt(team_list);
  shmctl(shmid, IPC_RMID, NULL);
  return 0;

}
