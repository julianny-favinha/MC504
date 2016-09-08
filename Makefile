CC = gcc
CFLAGS = -pthread -Wall -g -l pthread

PROGRAMS = sushi \

all: $(PROGRAMS)

clean:
	rm -f *.o *~ $(PROGRAMS)
