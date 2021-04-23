

#define DEBUG
#define SIZE 50


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <semaphore.h> // include POSIX semaphores
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>


pthread_cond_t count_threshold_cv=PTHREAD_COND_INITIALIZER;
pthread_mutex_t exit_threads_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

struct semaphoreStruct{
  sem_t *mutex;
};

int number_laps = 0;
int exited_threads = 0;

struct semaphoreStruct *semaphore_list;
/*
void changeArray(int arr[3][2]){
  arr[1][0] = 0;
  arr[2][1] = 102324;
  return;
}
*/


void *increment(void *t){

  sleep(1);


  pthread_cond_broadcast(&count_threshold_cv);


  pthread_exit(NULL);
}


void *wait_increment(void *t){
  printf("waiting\n");
  pthread_cond_wait(&count_threshold_cv, &exit_threads_mutex);
  pthread_mutex_unlock(&exit_threads_mutex);
  printf("exited\n");
  pthread_exit(NULL);




}
int main(){
  /*semaphore_list = (struct semaphoreStruct*) malloc(sizeof(struct semaphoreStruct));
  sem_unlink("MUTEX");
  semaphore_list->mutex = sem_open("MUTEX", O_CREAT|O_EXCL,0700,1);
  return 0;*/
  /*
  for(int i = 0; i<1; ++i){
    printf("%d",i);
  }
  */

  /*
  int array[3][2] ={{1,2},{3,4},{5,7}};
  changeArray(array);

  printf("%d\n",array[2][1]);
  */

  long id[6];
  id[5] = 5;
  pthread_t threads[6];
  for(int i = 0 ; i<5; i++){
    id[i] = i+1;
    pthread_create(&threads[i], NULL, wait_increment, (void*)&id[i]);
  }

  pthread_create(&threads[5], NULL, increment,(void*)&id[6]);


  for(int i=0;i<6;i++){
    pthread_join(threads[i],NULL);
  }
  pthread_mutex_destroy(&count_mutex);
  pthread_mutex_destroy(&exit_threads_mutex);
  pthread_cond_destroy(&count_threshold_cv);
  pthread_exit(NULL);
}
