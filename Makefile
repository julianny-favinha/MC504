CC = gcc
CFLAGS = -pthread -Wall -g -l pthread

PROGRAMS = sushi color_test \

all: $(PROGRAMS)

clean:
	rm -f *.o *~ $(PROGRAMS)
