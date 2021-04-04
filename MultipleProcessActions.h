//Edgar Filipe Ferreira Duarte 2019216077
//Pedro Guilherme da Cruz Ferreira 2018277677
#include <semaphore.h>

void writeLog(char * string, sem_t *mutex);
void writingNewCarInSharedMem(struct team *team_list, struct car *new_car, struct config_fich_struct *inf_fich, char *team_name, sem_t *mutex1, sem_t *mutex2);
