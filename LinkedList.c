
#include "LinkedList.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


//Inserts elements to the list
void insert(struct car c, struct elem_fila **root){

  struct elem_fila *aux, *next, *previous;

  aux = (struct elem_fila *) malloc(sizeof(struct elem_fila));

  //Something went wrong
  if (aux == NULL) {
    return;
  }
  aux->car_info = c;
  aux->next_car = NULL;

  //If the root is null
  if (*root == NULL){
    *root = aux;

  }
  //If the root is not null
  else{
      previous = *root;
      next = (*root)->next_car;

          while (next != NULL) {
              previous = next;
              next = next->next_car;
          }
          previous->next_car = aux;
          aux->next_car =NULL;
      }
}


void printList(struct elem_fila *root){
  struct elem_fila *current;

  current = root;

  while(current != NULL ){
    printf("Numero carro: %d, Velocidade: %d, Consumption: %d, Reliability:%d\n", current->car_info.car_number,
                                                                                current->car_info.speed,
                                                                                current->car_info.consumption,
                                                                                current->car_info.reliability);
   current = current->next_car;

  }
  return;
}
