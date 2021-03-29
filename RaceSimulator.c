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
  strcpy(team_list[0].box_state, "Open" ) ;
  team_list[0].cars[0].speed = 10;
  team_list[0].cars[0].consumption = 70;
  team_list[0].cars[0].reliability = 60;
  team_list[0].cars[0].car_number = 1;
  //Race_Manager(inf_fich->number_of_teams, inf_fich->number_of_cars);

  int pid=fork();
  /*
  if(pid!=0){:::
    printf("Processo main\n");
  }
  else{
    int pid2=fork();
    if(pid2==0){

      printf("Gerador de Corrida.\n");
      strcpy(car_list[1].team_name, "Sporting");
      car_list[1].car_number = 2;
      car_list[1].speed = 70;
      car_list[1].consumption = 30;
      car_list[1].reliability = 10;
      printf("%d", inf_fich->number_of_teams);
      printf("fsakçofsdjjhissd\n");
      Race_Manager(inf_fich->number_of_teams, inf_fich->number_of_cars);


      exit(0);
    }
    else{
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
  */
  free(inf_fich);
  shmdt(team_list);
  shmctl(shmid, IPC_RMID, NULL);
  return 0;

}
