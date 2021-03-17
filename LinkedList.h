
#include <stdio.h>
#define SIZE 50
struct car{
  int car_number;
  int speed;
  int consumption;
  int reliability;
};

struct teams{
  char team_name[SIZE];
  struct elem_fila *car_list_root;
};

struct elem_fila{
  struct car car_info;
  struct elem_fila *next_car;

};

void insert(struct car c, struct elem_fila **root);
void printList(struct elem_fila *root);
