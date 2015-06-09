CFLAGS = -g -Wall
OBJECTS = nttest.o ../libnettraf.a
INCLUDES = -I../

all: nttest

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

nttest: $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o nttest $(OBJECTS)

clean:
	rm -f *.o *~ *.bak nttest
