exec: RaceSimulator.o RaceManager.o ReadConfig.o TeamManager.o BreakDownManager.o MultipleProcessActions.o
	gcc RaceSimulator.o RaceManager.o ReadConfig.o TeamManager.o BreakDownManager.o MultipleProcessActions.o -lpthread -D_REENTRANT -Wall -o exec

RaceSimulator.o: RaceSimulator.c
	gcc -c RaceSimulator.c

RaceManager.o: RaceManager.c
	gcc -c RaceManager.c

ReadConfig.o: ReadConfig.c
	gcc -c ReadConfig.c

TeamManager.o: TeamManager.c
	gcc -c TeamManager.c

BreakDownManager.o: BreakDownManager.c
	gcc -c BreakDownManager.c

MultipleProcessActions.o: MultipleProcessActions.c
	gcc -c MultipleProcessActions.c

clean:
	rm *.o exec
