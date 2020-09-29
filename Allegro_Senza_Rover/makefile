MAIN = main
#---------------------------------------------------
# CC will be the compiler to use
#---------------------------------------------------
CC = gcc
#---------------------------------------------------
# CFLAGS will be the options passed to the compiler
#---------------------------------------------------
CFLAGS = -Wall -pthread -lrt 
#---------------------------------------------------
# Dependencies
#---------------------------------------------------
$(MAIN): $(MAIN).o ptask.o graphics.o utility.o BtComunication.o
	$(CC) $(CFLAGS) -o avoidance $(MAIN).o ptask.o utility.o BtComunication.o graphics.o `allegro-config --libs` -lm

$(MAIN).o: $(MAIN).c
	$(CC) -c $(MAIN).c

ptask.o: ptask.c
	$(CC) -c ptask.c


graphics.o: graphics.c
	$(CC) -c graphics.c

utility.o: utility.c
	$(CC) -c utility.c
BtComunication.o: BtComunication.c
	$(CC) -c BtComunication.c
clean:
	rm *.o avoidance
