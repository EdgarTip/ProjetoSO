
#define SIZE 50
struct config_fich_struct{
  int time_units_per_second;
  int lap_distance;
  int number_of_laps;
  int number_of_teams;
  int T_Avaria;
  int T_Box_Min;
  int T_Box_Max;
  int fuel_capacity;
};

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
