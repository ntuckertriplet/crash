CFLAGS= -g -I. -Og -Wall
DEPS = list.h signalhandler.h process.h printlogo.h
LIBS = -lpthread
EXEC_FILE = 308sh

all: $(EXEC_FILE)

%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) -o $@ $< 

$(EXEC_FILE): main.o list.o signalhandler.o process.o
	$(CC) $^ $(CFLAGS) -o $@ $(LIBS)

clean:
	rm -f *.o $(EXEC_FILE)