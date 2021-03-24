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

<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> Escrita da hora para o log file.
#include "RaceManager.c"


#define DEBUG

<<<<<<< HEAD
=======
>>>>>>> Processes created
=======
>>>>>>> Escrita da hora para o log file.
int * shared_var;


int main(){
  FILE *fp_configs;
  fp_configs = fopen("/home/user/github/Race_Manager/configs.txt", "r");

  int time_units_per_second, lap_distance, number_of_laps, number_of_teams, T_Avaria, T_Box_Min, T_Box_Max, fuel_capacity;
  if (fp_configs == NULL)
  {
     #ifdef DEBUG
     perror("Error while opening the file.\n");
     #endif
     exit(EXIT_FAILURE);
  }
  #ifdef DEBUG
  printf("Ficheiro aberto com exito\n");
  #endif
<<<<<<< HEAD

  fscanf(fp_configs,"%d\n%d, %d\n%d\n%d\n%d, %d\n%d",&time_units_per_second, &lap_distance, &number_of_laps,&number_of_teams, &T_Avaria, &T_Box_Min, &T_Box_Max, &fuel_capacity);

=======

  fscanf(fp_configs,"%d\n%d, %d\n%d\n%d\n%d, %d\n%d",&time_units_per_second, &lap_distance, &number_of_laps,&number_of_teams, &T_Avaria, &T_Box_Min, &T_Box_Max, &fuel_capacity);

>>>>>>> Escrita da hora para o log file.
  #ifdef DEBUG
  printf("Número de unidades de tempo por segundo: %d\n",time_units_per_second);
  printf("Distância de cada volta, Número de voltas da corrida: %d, %d\n",lap_distance,number_of_laps);
  printf("Número de equipas: %d\n",number_of_teams);
  printf("T_Avaria: %d\n",T_Avaria);
  printf("T_Box_Min, T_Box_Max: %d, %d\n",T_Box_Min,T_Box_Max);
  printf("Capacidade do depósito de combustível: %d\n",fuel_capacity);
  #endif
  fclose(fp_configs);
<<<<<<< HEAD

  system("date|cut -c17-24 >> logs.txt");

<<<<<<< HEAD
  int pid=fork();

  if(pid==0){
    #ifdef DEBUG
    printf("Simulador de Corrida.\n");
    #endif
=======
  fclose(fp);


  // Create shared memory
	int shmid=shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT|0700);
  if (shmid < 1){
	 perror("Error in shmget with IPC_CREAT\n");
	 exit(0);
	}

  // Attach shared memory
  shared_var = (int*) shmat(shmid,NULL,0);
=======

  system("date|cut -c17-24 >> logs.txt");
>>>>>>> Escrita da hora para o log file.

  int pid=fork();

  if(pid==0){
<<<<<<< HEAD
    printf("Processo main\n");
>>>>>>> Processes created
=======
    #ifdef DEBUG
    printf("Simulador de Corrida.\n");
    #endif
>>>>>>> Escrita da hora para o log file.
  }
  else{
    int pid2=fork();
    if(pid2==0){
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> Escrita da hora para o log file.
      #ifdef DEBUG
      printf("Gestor de Corrida criado.\n");
      #endif
      Race_Manager(number_of_teams);
<<<<<<< HEAD
    }
    else{
      #ifdef DEBUG
      printf("Gestor de Avarias criado.\n");
      #endif

    }
  }
sleep(3);
=======
      printf("Gerador de Corrida.\n");
=======
>>>>>>> Escrita da hora para o log file.
    }
    else{
      #ifdef DEBUG
      printf("Gestor de Avarias criado.\n");
      #endif

    }
  }
<<<<<<< HEAD

>>>>>>> Processes created
=======
sleep(3);
>>>>>>> Escrita da hora para o log file.
  return 0;
}
