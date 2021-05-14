//Edgar Filipe Ferreira Duarte 2019216077
//Pedro Guilherme da Cruz Ferreira 2018277677

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>


#include "RaceSimulator.h"
#include "MultipleProcessActions.h"
#include "TeamManager.h"

struct config_fich_struct *inf_fich;
struct team *team_list;
struct semaphoreStruct *semaphore_list;

struct ids *ids_proc;

int *pids;
int *pipes;

int number_of_teams_stopped;
int number_added_this_iteration;
int race_has_paused;
int start = 0;

void skip(int signum){

    number_of_teams_stopped++;

    //means all the teams have finished
    if(race_has_paused == 0 && number_of_teams_stopped == inf_fich->number_of_teams){
        writeLog("EVERY SINGLE CAR GAVE UP", semaphore_list->logMutex, inf_fich->fp);
        kill(getppid(), SIGINT);
    }
}

void pauseRace(int signum){

    signal(SIGUSR1, SIG_IGN);

    if(start == 1){

        race_has_paused =1;
        writeLog("SIMULATOR PAUSING", semaphore_list->logMutex, inf_fich->fp);

        for(int i = 0; i < inf_fich->number_of_teams; i++){
            kill(pids[i], SIGUSR1);
        }

        kill(ids_proc->pid_breakdown, SIGUSR1);

        start = 0;

        number_added_this_iteration = 0;

        writeLog("WAITING FOR ALL CARS TO PAUSE", semaphore_list->logMutex, inf_fich->fp);
        while(number_of_teams_stopped != inf_fich->number_of_teams){
            pause();
            number_added_this_iteration++;
        }

        writeLog("ALL CARS PAUSED", semaphore_list->logMutex, inf_fich->fp);
        printf("IF YOU WANT TO RESTART RACE TYPE: START RACE!\n");
    }

}

void endRaceManager(int signum){


    for(int i = 0; i < inf_fich->number_of_teams; i++){
        kill(pids[i], SIGUSR2);
    }
    free(pids);

    for(int i = 0; i < inf_fich->number_of_teams; i++){
        wait(NULL);
    }

    #ifdef DEBUG
    printf("RaceManeger is out.\n");
    #endif

    exit(0);
}


void endRace(){
    kill(ids_proc->pid_breakdown, SIGUSR2);
    kill(getppid(), SIGUSR2);
    endRaceManager(0);
}


int getFreeChannel(int n, int pipes[n]){
    for(int i=0;i<n;i++){
        if(pipes[i]==-1){
            return i;
        }
    }
    return -1;

}

int getPipesCreated(int n, int pipes[n]){
    int total=0;
    for(int i=0;i<n;i++){
        if(pipes[i]!=-1){
            total++;
        }
    }
    return total;
}

void Race_Manager(struct config_fich_struct *inf_fichP, struct team *team_listP,  struct semaphoreStruct *semaphore_listP, struct ids *idsP){

    char log[550];

    sigset_t mask, new_mask;
    printf("Race manager id: %d\n", getpid());
    ids_proc = idsP;
    //Ignore all unwanted signals!
    sigfillset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);


    sigemptyset(&new_mask);
    sigaddset(&new_mask,SIGUSR1);
    sigaddset(&new_mask,SIGUSR2);
    sigaddset(&new_mask,SIGALRM);
    sigprocmask(SIG_UNBLOCK,&new_mask, NULL);

    signal(SIGUSR1, pauseRace);
    signal(SIGUSR2, endRaceManager);
    signal(SIGALRM, skip);

    number_of_teams_stopped = 0;

    #ifdef DEBUG
    printf("Race_Manager created (%ld)\n",(long)getpid());
    #endif

    inf_fich = inf_fichP;
    team_list = team_listP;
    semaphore_list = semaphore_listP;

    pipes=malloc(sizeof(*pipes)*(inf_fich->number_of_teams+1));
    for(int i=0;i<inf_fich->number_of_teams+1;i++){
        pipes[i]=-1;
    }

    pthread_t team_threads[inf_fich->number_of_teams];
    pids = malloc(sizeof(int) * inf_fich->number_of_teams);
    //For testing purposes!

    pid_t childpid, wpid;


    int status = 0;
    int i = 0;
    int index_team = 0;
    int firstStart = 0;

    #ifdef DEBUG
    printf("Opening named pipe for reading\n");
    #endif
    int fd;
    if ((fd= open(PIPE_NAME, O_RDONLY)) < 0) {
        perror("Cannot open pipe for reading: ");
        exit(0);
    }
    pipes[0]=fd;
    #ifdef DEBUG
    printf("Named pipe open for reading\n");
    #endif

    char teamName[50];
    char received[512];
    int n=1;

    while(1){
        fd_set read_set;
        FD_ZERO(&read_set);
        for(int channel=0;channel<inf_fich->number_of_teams+1;channel++){
            FD_SET(pipes[channel], &read_set);
        }
        if (select(pipes[n-1]+1, &read_set, NULL, NULL, NULL) > 0 ) { //NAO FAÇO -1 PQ O "PIPES" TEM NUMBER_OF_TEAMS + 1 (NAMED)
        if(FD_ISSET(pipes[0], &read_set)){
            sigprocmask(SIG_BLOCK,&new_mask, NULL);
            read(pipes[0],received,sizeof(received));
            #ifdef DEBUG
            printf("[RaceManager] (NP) Received: %s\n",received);
            #endif

            //char received[512] ="ADDCAR TEAM: A, CAR: 20, SPEED: 30, CONSUMPTION: 0.04, RELIABILITY: 95";
            //char received[512] ="ADDCAR TEAM: C, CAR: 21, SPEED: 30, CONSUMPTION: 0.04, RELIABILITY: 95";
            //char received[512] ="START RACE!";
            if(strcmp(received,"START RACE!")==0){
                printf("here!\n");
                //verificar se o numero de equipas é suficiente => erro no ecra e nos logs
                if(start == 0){

                    if(firstStart == 0){
                        int n_equipas=0;
                        for(int i=0; i<inf_fich->number_of_teams; i++){
                            if(strcmp(team_list[i].team_name,"")!=0){
                                n_equipas++;
                            }
                        }
                        if(n_equipas!=inf_fich->number_of_teams){
                            writeLog("CANNOT START, NOT ENOUGH TEAMS",semaphore_list->logMutex,inf_fich->fp);
                        }
                        else{
                            //Notificar os TeamManagers do inicio da corrida
                            for(int i = 0; i<inf_fich->number_of_teams; i++){
                                kill(pids[i], SIGTERM);

                            }
                            kill(getppid(), SIGTERM);
                            kill(idsP->pid_breakdown, SIGTERM);
                            start = 1;
                            firstStart = 1;

                        }
                    }
                    else{
                        //Notificar os TeamManagers do inicio da corrida
                        for(int i = 0; i<inf_fich->number_of_teams; i++){
                            kill(pids[i], SIGTERM);

                        }
                        kill(idsP->pid_breakdown, SIGTERM);

                        writeLog("RACE UNPAUSED",semaphore_list->logMutex,inf_fich->fp);
                        start = 1;
                        race_has_paused =0;
                        number_of_teams_stopped -= number_added_this_iteration;
                        signal(SIGUSR1, pauseRace);
                    }
                }
                else if(start == 1){
                    sprintf(log,"%s => Rejected, race already started!",received);
                    writeLog(log,semaphore_list->logMutex,inf_fich->fp);
                }
            } //is START RACE!
            else{ // Check if it is ADDCAR
                struct car *newCar = (struct car *) malloc(sizeof(struct car));

                int validation = sscanf(received,"ADDCAR TEAM: %[^,], CAR: %d, SPEED: %d, CONSUMPTION: %f, RELIABILITY: %d",teamName,&(newCar->car_number),&(newCar->speed),&(newCar->consumption),&(newCar->reliability));

                if( validation != 5 ||
                    newCar->car_number < 0 ||
                    newCar->speed <= 0 ||
                    newCar->consumption < 0 ||
                    newCar->consumption > inf_fich->fuel_capacity ||
                    newCar->reliability < 0 ){

                        sprintf(log,"WRONG COMMAND => %s",received);
                        writeLog(log,semaphore_list->logMutex,inf_fich->fp);
                    }
                    else{
                        if(start == 1){ //Race already started
                            sprintf(log,"%s => Rejected, race already started!",received);
                            writeLog(log, semaphore_list->logMutex, inf_fich->fp);
                        }
                        else{ //ADD CAR

                            newCar->number_of_laps = 0;
                            newCar->amount_breakdown = 0;
                            newCar->times_refill = 0;
                            newCar->has_breakdown = 0;
                            strcpy(newCar->current_state, "CORRIDA");

                            int teamCreated = writingNewCarInSharedMem(team_list, newCar, inf_fich, teamName, semaphore_list);

                            if(teamCreated ==1){

                                #ifdef DEBUG
                                printf("Getting channel for new team.\n");
                                #endif

                                int c=getFreeChannel(inf_fich->number_of_teams+1,pipes);

                                int channel[2];
                                pipe(channel);
                                n++;
                                pipes[c]=channel[0];

                                if((pids[i] =fork())==0){


                                    Team_Manager(inf_fich, team_list, semaphore_list,channel,index_team,idsP);

                                }
                                i++;
                                index_team++;
                                //close(channel[1]);

                            }
                        }
                    }
                } //ISNt START RACE!
                sigprocmask(SIG_UNBLOCK,&new_mask, NULL);

            }//isset
            //else{
            for(int channel=1;channel<(inf_fich->number_of_teams+1);channel++){
                if(FD_ISSET(pipes[channel], &read_set)){
                    sigprocmask(SIG_BLOCK,&new_mask, NULL);

                    struct message data;

                    read(pipes[channel],&data,sizeof(struct message));
                    #ifdef DEBUG
                    printf("[RaceManager] (%d) Received %d, %d ,%s.\n",channel,data.car_index, data.team_index,data.message);
                    #endif

                    printf("MENSAGEM RECEBIDA %s\n",data.message);
                    if(strcmp(data.message,"TERMINADO") == 0){
                        signal(SIGUSR1, SIG_IGN);
                        sigprocmask(SIG_UNBLOCK,&new_mask, NULL);
                        endRace();

                    }
                    sigprocmask(SIG_UNBLOCK,&new_mask, NULL);
                }
            }
            //}


        } //select
    } //while


}
