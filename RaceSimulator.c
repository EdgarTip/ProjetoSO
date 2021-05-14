//Edgar Filipe Ferreira Duarte 2019216077
//Pedro Guilherme da Cruz Ferreira 2018277677


//COMPILE THE CODE USING: gcc RaceSimulator.c RaceManager.c ReadConfig.c  TeamManager.c BreakDownManager.c MultipleProcessActions.c -lpthread -D_REENTRANT -Wall -o exec
//YOU CAN DEACTIVATE THE DEBUG MESSAGES BY DELITING THE "DEFINE DEBUG" (line 2) IN THE RACESIMULATOR.H FILE

//TO RUN THE CODE AFTER COMPILING IT USE THE FORMAT : ./exec {configuration file name}

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


#include "RaceSimulator.h"
#include "RaceManager.h"
#include "ReadConfig.h"
#include "MultipleProcessActions.h"
#include "BreakDownManager.h"
/*Struct that retains the information given by the initial file
Not a shared memory struct but the values will be given to the
processes created in this script*/



/*Shared memory struct the will share memory about the current state of all
of the cars. Will be updated by the Race Manager
*/


struct config_fich_struct *inf_fich;

struct team *team_list;

struct semaphoreStruct *semaphore_list;

struct ids *idsP;

int shmid;
int named_pipe_fd;
pid_t pids[2];
int race_started =0 ;


//Only for debug purposes will be deleted/changed later
/*
void leituraParaTeste(){

for(int i = 0; i< inf_fich->number_of_teams; i++){
if(strcmp(team_list[i].team_name, "") == 0){
return;
}
for(int j = 0; j < team_list[i].number_of_cars; j++){
if(team_list[i].cars[j].speed ==0){
break;
}
printf("Amount cars: %d\n Box State %s\n", team_list[i].number_of_cars, team_list[i].box_state );
printf("Team name:%s\n Box state:%s\n Car number: %d\n Car speed: %d\n Car consumption: %.2f\n Car reliability: %d\n Number laps: %d\n Amount Breakdown %d\n Amount reffil:%d\n Car state:%s", team_list[i].team_name
, team_list[i].box_state
, team_list[i].cars[j].car_number
, team_list[i].cars[j].speed
, team_list[i].cars[j].consumption
, team_list[i].cars[j].reliability
, team_list[i].cars[j].number_of_laps
, team_list[i].cars[j].amount_breakdown
, team_list[i].cars[j].times_refill
, team_list[i].cars[j].current_state);

printf("Team name:       %s\nBox state:       %s\nCar number:      %d\nCar speed:       %d\nCar consumption: %.2f\nCar reliability: %d\nNumber of laps:       %d\n", team_list[i].team_name
, team_list[i].box_state
, team_list[i].cars[j].car_number
, team_list[i].cars[j].speed
, team_list[i].cars[j].consumption
, team_list[i].cars[j].reliability
, team_list[i].cars[j].number_of_laps);

printf("\n");
}
printf("-----------\n");
}

}*/

//cleans active memory
void clean(){
    free(inf_fich);
    shmdt(team_list);
    msgctl(idsP->msg_queue_id,IPC_RMID,NULL);
    shmctl(shmid, IPC_RMID, NULL);
    sem_close(semaphore_list->logMutex);
    sem_close(semaphore_list->writingMutex);
    sem_close(semaphore_list->readingMutex);

    sem_unlink("MUTEX");
    sem_unlink("WRITING_MUTEX");
    sem_unlink("READING_MUTEX");
    free(semaphore_list);
    free(idsP);
    close(named_pipe_fd);
    unlink(PIPE_NAME);
}



void endRaceSim(int signum){
    pid_t wpid;
    int status = 0;

    while ((wpid = wait(&status)) > 0);

    //leituraParaTeste();
    if (race_started == 1) {
        readStatistics(inf_fich, team_list, semaphore_list);
    }

    clean();
    exit(0);
}


void sigint(int signum){

    printf("\n");
    writeLog("SIMULATOR CLOSING", semaphore_list->logMutex, inf_fich->fp);
    signal(SIGINT, SIG_IGN);

    kill(pids[0],SIGUSR2);
    kill(pids[1],SIGUSR2);

    endRaceSim(0);
}


void sigtstp(int signum){

    signal(SIGTSTP, SIG_IGN);
    if(race_started == 1) readStatistics(inf_fich, team_list, semaphore_list);
    signal(SIGTSTP, sigtstp);

}

void raceStartRaceSim(int signum){
    race_started =1;
}



//Main function. Here the RaceManager and the MalfunctionManager processes will be created
int main(int argc, char* argv[]){

    sigset_t mask, new_mask;

    //Ignore all unwanted signals!
    sigfillset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    //Create the wanted signals
    sigemptyset(&new_mask);
    sigaddset(&new_mask,SIGINT);
    sigaddset(&new_mask,SIGTSTP);
    sigaddset(&new_mask,SIGTERM);
    sigaddset(&new_mask, SIGUSR2);

    sigprocmask(SIG_UNBLOCK,&new_mask, NULL);
    signal(SIGINT, sigint);
    signal(SIGTSTP, sigtstp);
    signal(SIGUSR2, endRaceSim);
    signal(SIGTERM, raceStartRaceSim);


    semaphore_list = (struct semaphoreStruct*) malloc(sizeof(struct semaphoreStruct));
    sem_unlink("MUTEX");
    semaphore_list->logMutex = sem_open("MUTEX", O_CREAT|O_EXCL,0700,1);
    sem_unlink("WRITING_MUTEX");
    semaphore_list->writingMutex = sem_open("WRITING_MUTEX", O_CREAT|O_EXCL,0700,1);
    sem_unlink("READING_MUTEX");
    semaphore_list->readingMutex = sem_open("READING_MUTEX", O_CREAT|O_EXCL,0700,1);


    if (argc!=2){
        writeLog("Error with input arguments. Execution aborted!", semaphore_list->logMutex,  inf_fich->fp);
        printf("Invalid number of arguments!\nUse as: executable {name of the configurations file}\n");
        exit(1);
    }

    //Read the configuration file
    char *file_name = argv[1];
    inf_fich=readConfigFile(file_name);

    sem_post(semaphore_list->logMutex);

    #ifdef DEBUG
    printf("Creating shared memory\n");
    #endif

    //Creates shared memory
    shmid = shmget(IPC_PRIVATE, inf_fich->number_of_teams * (sizeof(struct team*) + sizeof(struct car*) * inf_fich->number_of_cars), IPC_CREAT|0700);
    if (shmid < 1) exit(0);
    team_list = (struct team*) shmat(shmid, NULL, 0);
    if (team_list < (struct team*) 1) exit(0);


    for(int i = 0; i <= inf_fich->number_of_teams ; i++){
        team_list[i].cars = (struct car*)(team_list + inf_fich->number_of_teams +1   + i * (inf_fich->number_of_cars));
    }

    writeLog("SIMULATOR STARTING", semaphore_list->logMutex, inf_fich->fp);

    idsP = (struct ids*) malloc(sizeof(struct ids));


    assert( (idsP->msg_queue_id= msgget(IPC_PRIVATE, IPC_CREAT|0700)) != -1 );


    if((pids[0] = fork()) == 0){
        //Creates the break down manager
        idsP->pid_breakdown = getpid();

        BreakDownManager(inf_fich, team_list, semaphore_list, idsP);


    }

    idsP->pid_breakdown = pids[0];

    //Creates RaceManager and BreakDownManager
    if( (pids[1] = fork()) == 0){
        //Creates the RaceManager
        Race_Manager(inf_fich, team_list, semaphore_list,  idsP);

    }


    unlink(PIPE_NAME);
    #ifdef DEBUG
    printf("Creating named pipe.\n");
    #endif
    if ((mkfifo(PIPE_NAME, O_CREAT|O_EXCL|0600)<0) && (errno!= EEXIST)) {
        perror("Cannot create pipe: ");
        exit(0);
    }
    #ifdef DEBUG
    printf("Named pipe created.\n");
    #endif




    #ifdef DEBUG
    printf("Opening named pipe.\n");
    #endif
    if ((named_pipe_fd = open(PIPE_NAME, O_WRONLY)) < 0) {
        perror("Cannot open pipe for writing: ");
        exit(0);
    }
    #ifdef DEBUG
    printf("Named pipe open.\n");
    #endif

    // Do some work
    char readed[500];  //="ADDCAR TEAM: A, CAR: 20, SPEED: 30, CONSUMPTION: 0.04, RELIABILITY: 95";
    char toSend[512];
    char c;
    while(1){
        fflush(stdin);

        memset(readed, '\0', sizeof(readed));
        memset(toSend, '\0', sizeof(toSend));

        if((c=getchar())!='\n'){
            toSend[0]=c;
            scanf("%[^\n]%*c", readed);
            strcat(toSend,readed);

            #ifdef DEBUG
            printf("[RaceSimulator] (NP) Sending: %s\n",toSend);
            #endif

            write(named_pipe_fd, toSend, sizeof(toSend));
        }
    }

    return 0;

}
