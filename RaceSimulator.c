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


void teste(struct team *teams){

  for( int i = 0; i <= sizeof(teams)/sizeof(struct team*); i++){
    printf("%s", teams[i].team_name);
    for( int j = 0 ; j<= sizeof(teams[i].cars)/sizeof(struct car*); j++){
      printf("%d", teams[i].cars[j].speed);
    }
  }
}

//Main function. Here the RaceManager and the MalfunctionManager processes will be created
int main(){

  //Initialize the inf_fich struct and populate it
  inf_fich = (struct config_fich_struct*) malloc(sizeof(struct config_fich_struct));
  readConfigFile();

  struct team{
    char team_name[SIZE];
    struct car cars[inf_fich->number_of_cars];
  };


  shmid = shmget(IPC_PRIVATE, sizeof(struct team*) * inf_fich->number_of_teams, IPC_CREAT|0700);

  if (shmid < 1) exit(0);

  struct team team_list[inf_fich->number_of_teams];
  memset(team_list, 0, sizeof(team_list));
  memcpy(team_list, (struct team*) shmat(shmid, NULL, 0),sizeof(team_list));



  system("date|cut -c17-24 >> logs.txt");


  struct car carro2 = {10, 50, 60 ,80};
  struct car carro = { 10, 30, 50, 60};


  int pid=fork();

  if(pid==0){
    printf("Processo main\n");
  }
  else{
    int pid2=fork();
    if(pid2==0){
      team_list[1].team_name = "Sporting";
      team_list[1].cars[0] = carro;
      team_list[1].cars[1] = carro2;

      sleep(2);

      printf("Gerador de Corrida.\n");

      teste(team_list);
      exit(0);
    }
    else{
      team_list[0].team_name = "Sporting";
      team_list[0].cars[0] = carro;
      team_list[0].cars[1] = carro2;
      sleep(1);
      printf("Gerador de Avarias.\n");
      teste(team_list);


      exit(0);
    }
  }

  printf("---------MAIN-------\n");

  sleep(2);

  return 0;

}
