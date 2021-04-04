#include <semaphore.h>

void writeLog(char * string, sem_t *mutex);
void writingNewCarInSharedMem(struct team *team_list, struct car *new_car, struct config_fich_struct *inf_fichP, char *team_name, sem_t *mutex);
