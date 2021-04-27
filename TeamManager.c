//Edgar Filipe Ferreira Duarte 2019216077
//Pedro Guilherme da Cruz Ferreira 2018277677
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>


#include "RaceSimulator.h"
#include "MultipleProcessActions.h"



struct config_fich_struct *inf_fich;
struct team *team_list;
struct semaphoreStruct *semaphore_list;

pthread_t *cars;
pthread_mutex_t mutex_cond = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t 	variavel_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_interruption = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t 	interruption_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t  mutex_stop = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t 	stop_cond= PTHREAD_COND_INITIALIZER;

sem_t *car_in_box;
sem_t *reservation;
sem_t *update_waiting;


int team_index;
int car_index;

int *channel;

int start_teams = 0;
int number_of_cars;
int is_reserved = 0;
int terminating = 0;
int interrupting = 0;
int all_exited = 1;
int amount_terminated = 0;



//Ends the team
void teamEnd(int signum){
  terminating = 1;

  int number_cars = team_list[team_index].number_of_cars;

  while(amount_terminated != number_cars){
    sleep(1/inf_fich->time_units_per_second);
  }

  pthread_cond_broadcast(&stop_cond);

  for(int j=0; j<number_of_cars; j++){
    pthread_join(cars[j], NULL);
  }

  printf("Team %ld killed\n",(long)getpid());
  free(cars);
  sem_close(car_in_box);
  sem_close(reservation);
  sem_close(update_waiting);
  pthread_mutex_destroy(&mutex_cond);
  pthread_cond_destroy(&variavel_cond);
  printf("morreu\n");
  exit(0);
}



void waitForEnd(){
  pthread_cond_wait(&stop_cond,&mutex_stop);
  pthread_mutex_unlock(&mutex_stop);
  pthread_exit(NULL);
}


//Cars are racing
void racing(int arrayNumber){

  //Variables
  char logMessage[MAX];
  char update[MAX];
  float current_fuel = inf_fich->fuel_capacity;
  float distance_in_lap = 0;
  struct message data;

  //escrever para o gestor de equipas que o carro começou em corrida com o estado "CORRIDA"
  data.car_index = arrayNumber;
  data.team_index = team_index;


  while(1){



    //Racing normally
    if (strcmp(team_list[team_index].cars[arrayNumber].current_state,"CORRIDA") == 0) {
      current_fuel -= team_list[team_index].cars[arrayNumber].consumption;
      distance_in_lap += team_list[team_index].cars[arrayNumber].speed;

    }

    //Needs to got to the box
    else if (strcmp(team_list[team_index].cars[arrayNumber].current_state,"SEGURANCA") == 0) {

      current_fuel -= 0.4 * team_list[team_index].cars[arrayNumber].consumption;
      distance_in_lap += 0.3 * team_list[team_index].cars[arrayNumber].speed;
    }

    //The car has no more fuel. This means that the car is giving up on the race
    if(current_fuel <= 0){

      //TO-DO add a semaphore

      strcpy(data.message,"DESISTENCIA");
      write(channel[1], &data, sizeof(data));

      amount_terminated++;
      break;

    }
    printf("Team %s car amount of fuel %.02f\n",team_list[team_index].team_name, current_fuel);

    //If the car does not have enough fuel for the next 4 laps. The car enters security mode
    if(current_fuel < 2 * ((inf_fich->lap_distance / team_list[team_index].cars[arrayNumber].speed)*team_list[team_index].cars[arrayNumber].consumption )){


      // TODO : Add a semaphore here
      if(strcmp(team_list[team_index].cars[arrayNumber].current_state,"SEGURANCA") != 0){
        printf("Entrei para again estar no modo seguranca :) \n");
        strcpy(data.message,"SEGURANCA");
        write(channel[1],&data, sizeof(data));
      }

      sem_wait(update_waiting);

      if(strcmp(team_list[team_index].box_state, "LIVRE") == 0){
        is_reserved = 1;
        printf("Fiz uma reserva :) \n");
        sem_post(reservation);

      }
      else{
        sem_post(update_waiting);
      }

    }

    printf("Current distance in lap, team name (%.2f, %s)\n",distance_in_lap, team_list[team_index].team_name);

    //Passes a lap. Sees if the car has a breakdown or need a refill, if it does then it goes, else it continues the race
    if(distance_in_lap >= inf_fich->lap_distance || terminating == 1 || interrupting == 1){
      printf("ENTROU AGORA MORRE GORDO\n");
      //race forced to terminate!
      if(terminating == 1){
        amount_terminated++;
        printf("I am going away ;(\n");
        break;
      }

      //the race has been interrupted. Race will start again if a RACE START is detected via the named pipe
      else if(interrupting == 1){
        //This temp needs to exist in case that the a SIGINT is casted after a SIGUSR1 without a START RACE in the middle (the program would end in a deadlock)
        amount_terminated++;

        printf("I am being interrupted\n");
        printf("I am going to sleep until the race restarts\n");

        pthread_cond_wait(&interruption_cond, &mutex_interruption);

        pthread_mutex_unlock(&mutex_interruption);

        if(terminating == 1){
          printf("I am going away 2 ;( \n");
          break;
        }
        amount_terminated--;

      }

      //If the race has no interruptions
      else{
        team_list[team_index].cars[arrayNumber].number_of_laps++;

        printf("Number of laps, name car (%d, %s)\n", team_list[team_index].cars[arrayNumber].number_of_laps, team_list[team_index].team_name);
        //The car ended the race!
        if(team_list[team_index].cars[arrayNumber].number_of_laps >= inf_fich->number_of_laps){

          //TO-DO Add a semaphore here
          strcpy(data.message, "CORRIDA");
          write(channel[1], &data, sizeof(data));

          printf("I finished the race!\n");
          break;
        }

        distance_in_lap = distance_in_lap - inf_fich->lap_distance;

        sem_wait(update_waiting);
        //Put distance in lap to 0
        if((strcmp(team_list[team_index].cars[arrayNumber].current_state, "SEGURANCA") == 0) && (strcmp(team_list[team_index].box_state, "RESERVADO") == 0)){

          car_index = arrayNumber;

          //Notify team manager that a car has entered the box
          sem_post(car_in_box);

          //TO-DO add semaphore
          strcpy(data.message, "BOX");
          write(channel[1], &data, sizeof(data));
          //Waits for the everything to get sorted in the car
          pthread_cond_wait(&variavel_cond, &mutex_cond);

          current_fuel = team_list[team_index].cars[car_index].consumption;

          //if the race has terminated while he was in the box
          if(terminating == 1){
            amount_terminated++;
            printf("I am going away ;(\n");
            break;
          }

          else if(interrupting == 1){
            amount_terminated++;

            printf("I am being interrupted\n");
            printf("I am going to sleep until the race restarts\n");

            pthread_cond_wait(&interruption_cond, &mutex_interruption);

            pthread_mutex_unlock(&mutex_interruption);

            if(terminating == 1){
              printf("yeet\n");
              break;
            }
            amount_terminated--;

          }

          distance_in_lap = 0;
          //TO-DO Add a semaphore here
          strcpy(data.message, "CORRIDA");
          write(channel[1], &data, sizeof(data));


        }
        else if(current_fuel < 4 * ((inf_fich->lap_distance / team_list[team_index].cars[arrayNumber].speed)*team_list[team_index].cars[arrayNumber].consumption) && strcmp(team_list[team_index].box_state, "LIVRE") == 0){
          //car reserves and immediatly goes into box
          //TO-DO add semaphore
          is_reserved = 0;

          sem_post(reservation);

          car_index = arrayNumber;

          sem_post(car_in_box);
          distance_in_lap = 0;


        }
          //Normal time
          else{
            sem_post(update_waiting);
            sleep(1/inf_fich->time_units_per_second);
          }
        }
    }
      //Normal time
    else{
      sleep(1/inf_fich->time_units_per_second);
    }

  }

  waitForEnd();
}




//Car thread. For now it dies immediatly after being created
void *carThread(void* team_number){

  int number=*((int *)team_number);

  #ifdef DEBUG
  //printf("I %ld created car %d.\n",(long)getpid(),number);
  #endif
  //printf("i am waiting\n");

  //Have a condition variable

  racing(number);

  pthread_exit(NULL);


}

//Signals the race manager that the race has started
void raceStart(int signum){
  start_teams = 1;
}

void teste(int signum){
  printf("wqe fuuiowqe we gioq ioeg uio io\n");
}

void interruptRaceTeam(){
  interrupting = 1;
}


//Team manager. Will create the car threads
void Team_Manager(struct config_fich_struct *inf_fichP, struct team *team_listP, struct semaphoreStruct *semaphore_listP, int channelP[2],int team_indexP){
  signal(SIGUSR2, teamEnd);
  signal(SIGUSR1, interruptRaceTeam);
  signal(SIGTSTP, teste);
  signal(SIGTERM, raceStart);

  sem_unlink("BOX_MUTEX");
  car_in_box = sem_open("BOX_MUTEX", O_CREAT|O_EXCL,0700,0);
  sem_unlink("RESERVATION_MUTEX");
  reservation = sem_open("RESERVATION_MUTEX", O_CREAT|O_EXCL,0700,0);
  sem_unlink("UPDATE_MUTEX");
  update_waiting = sem_open("UPDATE_MUTEX", O_CREAT|O_EXCL,0700,1);


  #ifdef DEBUG
  printf("Team Manager created with id: %ld\n", (long)getpid());
  #endif

  channel = channelP;

  inf_fich = inf_fichP;
  team_list = team_listP;
  semaphore_list = semaphore_listP;
  team_index = team_indexP;

  //waits for race to start
  while(start_teams == 0){
    pause();
  }


  signal(SIGTERM, SIG_IGN);

  number_of_cars = team_list[team_index].number_of_cars;

  int workerId[number_of_cars];

  cars = malloc(sizeof(pthread_t) * number_of_cars);


  //Create the car threads
  for(int i=0; i<number_of_cars;i++){
    workerId[i] = i;
    pthread_create(&cars[i], NULL, carThread,&workerId[i]);
  }



  //wait for cars to update box start_teams

  while(1){
    //Means a car has made a reservation
    sem_wait(reservation);

    //Team updates the current box state and next waits for a car to enter the box
    strcpy(team_list[team_index].box_state, "RESERVADO");

    //This is to prevent 2 post update_waiting due to the fact that when there is no reservation a car with less than 4 * the amount of fuel to complete a lap can go into the box without a reservation
    if(is_reserved == 1) sem_post(update_waiting);


    sem_wait(car_in_box);

    //update the box state
    strcpy(team_list[team_index].box_state, "OCUPADO");

    //let cars go past the point of update
    sem_post(update_waiting);

    printf("entrei na box para fazer reparacoes e/ou recarregar a gasolina\n");
    if(team_list[team_index].cars[car_index].has_breakdown == 1){
      //Sleep for a random amount of time
      sleep((rand() % (inf_fich->T_Box_Min - inf_fich->T_Box_Max + 1)) + inf_fich->T_Box_Min);
    }

    //refill
    sleep(2);
    pthread_cond_signal(&variavel_cond);

  }



  //Waits for all the cars to die
  for(int j=0; j<number_of_cars; j++){
    pthread_join(cars[j],NULL);
  }

  free(cars);
  sem_close(car_in_box);
  sem_close(reservation);
  pthread_mutex_destroy(&mutex_cond);
  pthread_cond_destroy(&variavel_cond);
  sem_close(update_waiting);
  printf("I am dying\n");

  return;
}
