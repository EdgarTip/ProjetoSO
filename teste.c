

#define DEBUG
#define SIZE 50


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <semaphore.h> // include POSIX semaphores
#include <fcntl.h>



struct semaphoreStruct{
  sem_t *mutex;
};



struct semaphoreStruct *semaphore_list;
/*
void changeArray(int arr[3][2]){
  arr[1][0] = 0;
  arr[2][1] = 102324;
  return;
}
*/
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
}
