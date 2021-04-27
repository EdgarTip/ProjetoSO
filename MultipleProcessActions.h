//Edgar Filipe Ferreira Duarte 2019216077
//Pedro Guilherme da Cruz Ferreira 2018277677


void writeLog(char * string, sem_t *mutex,FILE *fp);
int writingNewCarInSharedMem(struct team *team_list, struct car *new_car, struct config_fich_struct *inf_fich, char *team_name, struct semaphoreStruct *semaphore_list);
void readStatistics(struct config_fich_struct *inf_fich, struct team *team_list, struct semaphoreStruct *semaphore_list);
void updateState(struct team *team_list,  struct semaphoreStruct *semaphore_list,struct message data);
