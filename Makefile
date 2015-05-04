INCLUDE_DIRS =
LIB_DIRS =
CC=g++

CDEFS=
CFLAGS= -O0 -g -std=c++11 $(INCLUDE_DIRS) $(CDEFS)
LIBS= -lrt
CPPLIBS= -L/usr/lib -lopencv_core -lopencv_flann -lopencv_video

HFILES=gait
CFILES=
CPPFILES=gait Joint

SRCS= ${HFILES} ${CFILES}
CPPOBJS= ${CPPFILES:.cpp=.o}

all:  gait

clean:
	-rm -f *.o *.d
	-rm -f gait

distclean:
	-rm -f *.o *.d

gait: gait.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o `pkg-config --libs opencv` $(CPPLIBS)

depend:

.c.o:
	$(CC) $(CFLAGS) -c $<

.cpp.o:
	$(CC) $(CFLAGS) -c $<
