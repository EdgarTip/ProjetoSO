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

#define DEBUG

void Race_Manager(int number_of_teams){

  //CRIAR OS GESTORES DE EQUIPA
  for(int i=0;i<number_of_teams;i++){
    if(fork()==0){
      #ifdef DEBUG
      printf("Criei um gestor de equipa (%d): %d\n",i,getpid());
      #endif
    }
    else exit(0);
  }



}
