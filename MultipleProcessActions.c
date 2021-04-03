#define DEBUG


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <semaphore.h>

void writeLog(char * string, sem_t *mutex){
  char buffer[200];


  sprintf(buffer, "date|cut -c17-24 >> logs.txt ; sed -i '$ s/$/ %s/' logs.txt; ",string);


  /* https://unix.stackexchange.com/questions/412835/append-text-with-echo-without-new-line
  sed -i '$ s/$/abc/' file.txt
      -i - modify the file inplace
      $ - indicate the last record/line
      s/$/abc/ - substitute the end of the line $ with substring abc (for the last record)

  */

  sem_wait(mutex);

  //Only for debug purposes
  #ifdef DEBUG
  printf("%ld is writing something to the log.txt file\n", (long)getpid());
  #endif

  system(buffer);

  sem_post(mutex);

}
