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

/*Struct that retains the information given by the initial file
Not a shared memory struct but the values will be given to the
processes created in this script*/



/*Shared memory struct the will share memory about the current state of all
of the cars. Will be updated by the Race Manager
*/


struct config_fich_struct *inf_fich;



int shmid;

//Reads the initial file and gives values to the inf_fich struct
int readConfigFile(){
  FILE *fp;

  #ifndef DEBUG
  printf("Leitura do ficheiro de configurações");
  #endif

  fp = fopen("/home/user/github/Race_Manager/configs.txt", "r"); // read mode

  int time_units_per_second, lap_distance, number_of_laps, number_of_teams, T_Avaria, T_Box_Min, T_Box_Max, fuel_capacity;
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


void teste(struct car *cars){
  for( int i = 0; i < sizeof(cars)/sizeof(struct car); i++){
    printf("%s %d", cars[i].team_name, cars[i].speed);
  }


}

//Main function. Here the RaceManager and the MalfunctionManager processes will be created
int main(){

  //Initialize the inf_fich struct and populate it
  inf_fich = (struct config_fich_struct*) malloc(sizeof(struct config_fich_struct));
  readConfigFile();

  struct car *car_list;


  shmid = shmget(IPC_PRIVATE, sizeof(struct car*) * inf_fich->number_of_teams*inf_fich->number_of_cars, IPC_CREAT|0700);

  if (shmid < 1) exit(0);


  car_list = (struct car*) shmat(shmid, NULL, 0);


    strcpy(car_list[0].team_name, "asdas");
    car_list[0].car_number = 1;
    car_list[0].speed = 10 ;
    car_list[0].consumption = 60 ;
    car_list[0].reliability = 80;

  system("date|cut -c17-24 >> logs.txt");


  int pid=fork();

  if(pid==0){
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

      exit(0);
    }
    else{
      printf("Gerador de Avarias.\n");
      strcpy(car_list[2].team_name, "Benfica merda");
      car_list[2].car_number = 3;
      car_list[2].speed = 50;
      car_list[2].consumption = 10;
      car_list[2].reliability = 90000;

      exit(0);
    }
  }

  printf("---------MAIN-------\n");

  sleep(2);

  for(int i = 0; i < inf_fich->number_of_cars; i++){
    printf("nome :%s,car number: %d, speed : %d, consumption: %d,reliability%d \n",
    car_list->team_name,
    car_list[i].car_number,
    car_list[i].speed,
    car_list[i].consumption,
    car_list[i].reliability);
  }

  return 0;

}
