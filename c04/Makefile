CC        = gcc
CFLAGS    = -Wall -pedantic -ansi 
LIBS      = -lrt  
CXX       = g++
CXXFLAGS  = -Wall -pedantic

TARGETS   = print_pid system fork fork_exec

#fork-exec sigquit pipe1 pipe2 pipe3

all: $(TARGETS)

print_pid: print_pid.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

system: system.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

fork: fork.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

fork_exec: fork_exec.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	\rm -f $(TARGETS) *~ core

