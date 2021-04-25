#define PIPE_NAME "pipe"
#define MAX 5

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


int pipes[MAX];

void clean(){
  unlink(PIPE_NAME);
}

void sigint(int signum){
  clean();
  exit(0);
}


void newProcess(){


  //Initialize the pipes
  for(int i=0;i<MAX;i++){
    pipes[i]=-1;
  }

  //OPEN NAMED PIPE FOR READING
  int fd;
  if ((fd= open(PIPE_NAME, O_RDONLY)) < 0) {
    perror("Cannot open pipe for reading: ");
    exit(0);
  }


  pipes[0]=fd;
  char received[512];

  //Loop that waits for data to appear in the pipes
  while(1){

    fd_set read_set;
    FD_ZERO(&read_set);

    for (int channel=0;channel<MAX;channel++) {
      FD_SET(pipes[channel], &read_set);
    }

    if (select(pipes[MAX-1]+1, &read_set, NULL, NULL, NULL) > 0 ) {

       if (FD_ISSET(pipes[0], &read_set)) {

          read(pipes[0],received,sizeof(received));
          printf("[Named pipe] Received %s.\n",received);

      }

      //Unamed pipes are still not created!
      for (int channel=1;channel<MAX;channel++) {

        if (FD_ISSET(pipes[channel], &read_set)) {

          read(pipes[channel],received,sizeof(received));
          printf("[Unamed pipe] Received %s.\n",received);

        }
      }
    }
  }
}


int main(){
  signal(SIGINT, sigint);

  if(fork()==0){
    newProcess();
    exit(0);
  }



  unlink(PIPE_NAME);

  printf("Creating named pipe.\n");
  if ((mkfifo(PIPE_NAME, O_CREAT|O_EXCL|0600)<0) && (errno!= EEXIST)) {
    perror("Cannot create pipe: ");
    exit(0);
  }
  printf("Named pipe created.\n");


  int named_pipe_fd;
  printf("Opening named pipe.\n");
  if ((named_pipe_fd = open(PIPE_NAME, O_WRONLY)) < 0) {
    perror("Cannot open pipe for writing: ");
    exit(0);
  }
  printf("Named pipe open.\n");


  char toSend[512];
  //Sends data via the NAMED pipe to our child process
  while(1){
    scanf("%[^\n]%*c", toSend);
    printf("[RaceSimulator] Sending (%s)\n",toSend);
    write(named_pipe_fd, toSend, sizeof(toSend));
  }


  return 0;
}
