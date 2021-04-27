


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <assert.h>
#include <sys/msg.h>
#include <pthread.h>

int i = 0;

pthread_mutex_t mutex_cond = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t 	variavel_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_interruption = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t 	interruption_cond = PTHREAD_COND_INITIALIZER;

pthread_t *cars;
/*pthread_mutex_t mutex_cond = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t 	variavel_cond = PTHREAD_COND_INITIALIZER;
sem_t *comunicate;
sem_t *reserva;*/
/*
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
*/
//void *thread(void *cancro){

/*  while(1){

    sleep(1);

    sem_post(comunicate);
    pthread_cond_wait(&variavel_cond, &mutex_cond);

    printf("It worked :O\n");
  }
}*/
/*
void weee(int signum){
  printf("I received it clear and loud\n");
}*/

void *thread(void *yeet){
  printf("brah");
  pthread_exit(NULL);
}

int main(){
/*  signal(SIGINT, teamEnd);

  int workerId[3];

  pthread_t cars[3];

  //Create the car threads
  for(int i=0; i<3;i++){
    workerId[i] = i+1;
    pthread_create(&cars[i], NULL, carThread,&workerId[i]);
  }*/
  //Waits for all the cars to die



  /*signal(SIGINT, weee);
  int workerId = 0;

  pthread_t cars[1];

  pthread_create(&cars[0], NULL, thread,&workerId);

  pause();
  for(int j=0; j<1; j++){
    pthread_join(cars[j],NULL);
  }*/
/*
  sem_unlink("MUTEX");
  comunicate = sem_open("MUTEX", O_CREAT|O_EXCL,0700,0);
  sem_unlink("ATUATIA");
  reserva= sem_open("A TUA TIA", O_CREAT|O_EXCL,0700,0);

  int workerId = 0;
  pthread_t cars[1];
  pthread_create(&cars[0], NULL, thread,&workerId);


  while(1){

    sem_wait(reserva);
    printf("está reservado!");
    sem_wait(comunicate);
    printf("yoooo\n");
      sleep(1);
      pthread_cond_signal(&variavel_cond);


  }
  */

  int workerId[5];

  pthread_t cars[5];
  printf("wer\n");
    //Create the car threads
  for(int i=0; i<5;i++){
    workerId[i] = i+1;
    pthread_create(&cars[i], NULL, thread,&workerId[i]);
  }

  sleep(1);

  for(int j=0; j<5; j++){
    printf("yup\n");
    pthread_join(cars[j],NULL);
  }



}
