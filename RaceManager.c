#define DEBUG
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

#include "MultipleProcessActions.h"
#include "RaceSimulator.h"


void *thread_carro(void* team_number){
    int number=*((int *)team_number);
    #ifdef DEBUG
    printf("Criei carro da equipa %d.\n",number);
    #endif
    pthread_exit(NULL);
    return NULL;

}



void Race_Manager(struct config_fich_struct *inf_fich, struct team *team_list, sem_t *mutex){

  #ifdef DEBUG
  printf("Race_Manager created with id: %ld" long(getpid());

  int workerId[max_cars];
  pthread_t carros[max_cars * number_of_teams];
  //CRIAR OS GESTORES DE EQUIPA
  for(int i=0;i<number_of_teams;i++){
    if(fork()==0){

      Team_Manager(struct config_fich_struct *inf_fich, struct team *team_list, sem_t *mutex);

      sleep(1);

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
