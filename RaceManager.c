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

#define DEBUG

void *thread_carro(void* team_number){
    int number=*((int *)team_number);
    #ifdef DEBUG
    printf("Criei carro da equipa %d.\n",number);
    #endif
    pthread_exit(NULL);
    return NULL;
}

#define DEBUG

void Race_Manager(int number_of_teams, int max_cars){

  int workerId[max_cars];
  pthread_t carros[max_cars * number_of_teams];
  //CRIAR OS GESTORES DE EQUIPA
  for(int i=0;i<number_of_teams;i++){
    if(fork()==0){
      #ifdef DEBUG
      printf("Criei um gestor de equipa (%d): %d\n",i,getpid());
      #endif
      sleep(1);
      // A ESTRUTURA EQUIPA VAI TER UM ARRAY DE CARROS, ONDE VAMOS GUARDAR OS IDS
      for(int j=0; j<max_cars;j++){
        workerId[j] = j+1;
        pthread_create(&carros[i*max_cars+ j], NULL, thread_carro,&workerId[j]);

      }
      for(int j=0; j<max_cars; j++){
        pthread_join(carros[j],NULL);
      }

      // wait until all threads finish
   }
   else{
     printf("bye\n");
     sleep(6);
     exit(0);
   }
  }

}
