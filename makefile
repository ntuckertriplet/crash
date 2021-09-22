CFLAGS= -g -I. -Og -Wall
DEPS = list.h signal.h process.h
LIBS = -lpthread
EXEC_FILE = 308sh

all: $(EXEC_FILE)

%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) -o $@ $< 

$(EXEC_FILE): main.o list.o signal.o process.o
	$(CC) $^ $(CFLAGS) -o $@ $(LIBS)

clean:
	rm -f *.o $(EXEC_FILE)