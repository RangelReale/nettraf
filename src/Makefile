CFLAGS = -g -Wall -DVERSION=\"1.3\" #-DDEBUG
# enable the next line if you want to enable TCP WRAPPERS
# CFLAGS += -DUSE_LIBWRAP
OBJECTS = nettrafd.o connection.o errrpt.c ../lib/libnettraf.a
INCLUDES = -I../lib
LIBS = -lpthread
# enable the next line if you want to enable TCP WRAPPERS
# LIBS += -lwrap
VERSION = 1.4

all: nettrafd

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

nettrafd: $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o nettrafd $(OBJECTS) $(LIBS)

clean:
	rm -f *.o *~ *.bak nettrafd
