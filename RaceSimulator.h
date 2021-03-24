
#define SIZE 50

struct config_fich_struct{
  int time_units_per_second;
  int lap_distance;
  int number_of_laps;
  int number_of_teams;
  int number_of_cars;
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

struct team{
  char team_name[SIZE];
  struct car cars[];
};
