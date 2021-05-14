//Pedro Guilherme da Cruz Ferreira 2018277677

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
#include <signal.h>

#include "RaceSimulator.h"

//Reads the initial file and gives values to the inf_fich struct
struct config_fich_struct * readConfigFile (char *file_name){

    struct config_fich_struct * inf_fich;

    inf_fich = (struct config_fich_struct*) malloc(sizeof(struct config_fich_struct));
    FILE *fp;

    fp = fopen(file_name, "r"); // read mode

    if (fp == NULL)
    {
        perror("Error while opening the file\n");
        exit(EXIT_FAILURE);
    }

    #ifdef DEBUG
    printf("Reading from the configurations file\n");
    #endif
    printf("File opened succesfully\n");


    int validation = fscanf(fp,"%d\n%d, %d\n%d\n%d\n%d\n%d, %d\n%d",&(inf_fich->time_units_per_second),
    &(inf_fich->lap_distance),
    &(inf_fich->number_of_laps),
    &(inf_fich->number_of_teams),
    &(inf_fich->number_of_cars),
    &(inf_fich->T_Avaria),
    &(inf_fich->T_Box_Min),
    &(inf_fich->T_Box_Max),
    &(inf_fich->fuel_capacity));
    if( validation != 9 ||
        inf_fich->time_units_per_second < 0 ||
        inf_fich->lap_distance < 0 ||
        inf_fich->number_of_laps <  0 ||
        inf_fich->number_of_teams < 3 ||
        inf_fich->number_of_cars < 0 ||
        inf_fich->T_Avaria < 0 ||
        inf_fich->T_Box_Min < 0 ||
        inf_fich->T_Box_Max < 0 ||
        inf_fich->fuel_capacity < 0 ||
        inf_fich->T_Box_Max < inf_fich->T_Box_Min){
            printf("Ficheiro de Configurações inválido.\n");
            exit(1);
        }

        FILE * fp2;
        fp2 = fopen("logs.txt","w");
        if (fp2 == NULL)
        {
            perror("Error while opening the file\n");
            exit(EXIT_FAILURE);
        }
        inf_fich->fp=fp2;
        /* printf("%d\n%d, %d\n%d\n%d\n%d\n%d, %d\n%d",(inf_fich->time_units_per_second),
        (inf_fich->lap_distance),
        (inf_fich->number_of_laps),
        (inf_fich->number_of_teams),
        (inf_fich->number_of_cars),
        (inf_fich->T_Avaria),
        (inf_fich->T_Box_Min),
        (inf_fich->T_Box_Max),
        (inf_fich->fuel_capacity));*/

        fclose(fp);
        return inf_fich;
    }
