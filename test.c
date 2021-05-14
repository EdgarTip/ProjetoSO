


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/msg.h>


int i = 0;

sem_t *car_in_box;
pthread_mutex_t mutex_cond = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t 	variavel_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_interruption = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t 	interruption_cond = PTHREAD_COND_INITIALIZER;

pthread_t *cars;

sem_t *semaphore;


void child2(){
    sem_unlink("BOX_MUTEX");
    car_in_box = sem_open("BOX_MUTEX", O_CREAT|O_EXCL,0700,2);

    sleep(1);

    sem_wait(car_in_box);

    printf("2 - I am out :)");
}

void child1(){

    sem_unlink("BOX_MUTEX");
    car_in_box = sem_open("BOX_MUTEX", O_CREAT|O_EXCL,0700,2);

    sem_wait(car_in_box);

    printf("1-waiting for 2 seconds\n");
    sleep(2);

    sem_wait(car_in_box);


}

int main(){

    if(fork() == 0){
        child1();
        printf("1-I am leaving\n");
        exit(0);
    }
    if(fork() == 0){
        child2();
        printf("2-I am leaving\n");
        exit(0);
    }

    for(int i = 0; i < 2 ; i++){
        wait(NULL);
    }

}
