


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>


pthread_t *cars;


void carEnd(){
  printf("Car from team %ld killed.\n",(long)getpid());
  pthread_exit(NULL);
}

void teamEnd(int signum){
  for(int j=0; j<3; j++){

    pthread_join(cars[j],NULL);
  }

  printf("Team %ld killed\n",(long)getpid());
  exit(0);
}

void *carThread(void* team_number){
  signal(SIGINT, carEnd);
  sleep(10);
  pthread_exit(NULL);
  return NULL;

}

int main(){
  signal(SIGINT, teamEnd);

  int workerId[3];

  pthread_t cars[3];

  //Create the car threads
  for(int i=0; i<3;i++){
    workerId[i] = i+1;
    pthread_create(&cars[i], NULL, carThread,&workerId[i]);
  }
  //Waits for all the cars to die
  for(int j=0; 3; j++){
    pthread_join(cars[j],NULL);
  }

  return 0;
}
