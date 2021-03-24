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
#include "LinkedList.h"

/*Struct that retains the information given by the initial file
Not a shared memory struct but the values will be given to the
processes created in this script*/



/*Shared memory struct the will share memory about the current state of all
of the cars. Will be updated by the Race Manager
*/


struct config_fich_struct *inf_fich;
//Struct that will be in the shared memory
struct teams *team_list;


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


  fscanf(fp,"%d\n%d, %d\n%d\n%d\n%d, %d\n%d",&(inf_fich->time_units_per_second),
                                             &(inf_fich->lap_distance),
                                             &(inf_fich->number_of_laps),
                                             &(inf_fich->number_of_teams),
                                             &(inf_fich->T_Avaria),
                                             &(inf_fich->T_Box_Min),
                                             &(inf_fich->T_Box_Max),
                                             &(inf_fich->fuel_capacity));

  fclose(fp);
  return 0;
}

void teste1(){
  printf("%s\n" ,team_list[0].team_name);
  printList(team_list[0].car_list_root);

}


void teste2(){

  printf("%s\n" ,team_list[0].team_name);

  printList(team_list[0].car_list_root);
}
//Main function. Here the RaceManager and the MalfunctionManager processes will be created
int main(){

  //Initialize the inf_fich struct and populate it
  inf_fich = (struct config_fich_struct*) malloc(sizeof(struct config_fich_struct));
  readConfigFile();



  shmid = shmget(IPC_PRIVATE, sizeof(struct teams ) * inf_fich->number_of_teams, IPC_CREAT|0700);
  if (shmid < 1) exit(0);
  team_list = (struct teams*) shmat(shmid, NULL, 0);

  printf("%ld\n", sizeof(struct teams ) * inf_fich->number_of_teams);
  system("date|cut -c17-24 >> logs.txt");

  strcpy(team_list[0].team_name,"Team A");

  struct car carro = { 10, 30, 50, 60};
  struct car carro2 = { 100, 80, 90, 70};
  insert(carro, &team_list[0].car_list_root);


  printf("%s\n" ,team_list[0].team_name);

  int pid=fork();

  if(pid==0){
    printf("Processo main\n");
  }
  else{
    int pid2=fork();
    if(pid2==0){
      sleep(3);
      printf("Gerador de Corrida.\n");

      teste1();
      exit(0);
    }
    else{
      sleep(3);
      printf("Gerador de Avarias.\n");

      teste2();

      exit(0);
    }
  }

  printf("---------MAIN-------\n");


  printf("%s\n" ,team_list[0].team_name);

  printList(team_list[0].car_list_root);
  sleep(2);
  return 0;

}
