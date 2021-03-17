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

 struct mem_struct{
  int time_units_per_second;
  int lap_distance;
  int number_of_laps;
  int number_of_teams;
  int T_Avaria;
  int T_Box_Min;
  int T_Box_Max;
  int fuel_capacity;
};

struct mem_struct *mem;
int shmid;

int readConfigFile(){
  FILE *fp;
  fp = fopen("/home/user/github/Race_Manager/configs.txt", "r"); // read mode

  int time_units_per_second, lap_distance, number_of_laps, number_of_teams, T_Avaria, T_Box_Min, T_Box_Max, fuel_capacity;
  if (fp == NULL)
  {
     perror("Error while opening the file.\n");
     exit(EXIT_FAILURE);
  }
  printf("Ficheiro aberto com exito\n");

  fscanf(fp,"%d\n%d, %d\n%d\n%d\n%d, %d\n%d",&(mem->time_units_per_second),
                                             &(mem->lap_distance),
                                             &(mem->number_of_laps),
                                             &(mem->number_of_teams),
                                             &(mem->T_Avaria),
                                             &(mem->T_Box_Min),
                                             &(mem->T_Box_Max),
                                             &(mem->fuel_capacity));






  fclose(fp);
  return 0;
}

int main(){

  mem = (struct mem_struct*) malloc(sizeof(struct mem_struct));

  readConfigFile();

  printf("%d\n", (mem->fuel_capacity));

  return 0;

}
