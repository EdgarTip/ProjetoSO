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
#include <pthread.h>

#define DEBUG

void thread_carro(void* team_number){
    #ifdef DEBUG
    printf("Criei carro da equipa %d.\n",team_number);
    #endif
}

void Race_Manager(int number_of_teams, int max_cars){
=======

#define DEBUG

void Race_Manager(int number_of_teams){
>>>>>>> main

  //CRIAR OS GESTORES DE EQUIPA
  for(int i=0;i<number_of_teams;i++){
    if(fork()==0){
      #ifdef DEBUG
      printf("Criei um gestor de equipa (%d): %d\n",i,getpid());
      #endif
<<<<<<< HEAD

      //int ids[max_cars]; // A ESTRUTURA EQUIPA VAI TER UM ARRAY DE CARROS, ONDE VAMOS GUARDAR OS IDS
      pthread_t carros[max_cars];
      for(int j=0; j<max_cars;j++){
        pthread_create(&carros[j], NULL, thread_carro,i);
      }

      // wait until all threads finish
     for(int j=0; j<max_cars; j++){
       pthread_join(carros[j],NULL);
     }

   }
=======
    }
>>>>>>> main
    else exit(0);
  }



}
