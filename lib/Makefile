CFLAGS = -g -Wall
OBJECTS = nettraf.o

all: libnettraf.a

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

libnettraf.a: $(OBJECTS)
	rm -f libnettraf.a
	ar cru libnettraf.a $(OBJECTS)
	ranlib libnettraf.a

clean:
	rm -f *.o *~ *.bak libnettraf.a
