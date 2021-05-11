CC        = gcc
CFLAGS    = -Wall -pedantic #-ansi 
LIBS      = -lpthread
CXX       = g++
CXXFLAGS  = -Wall -pedantic

SRCP      = sum_raceconds_pt    # Sum with race condition
SMP       = sum_mutex_pt        # Sum synchronised by mutex
PSP       = prodcons_sem_pt     # Producer-consumer synchronized by semaphores

SRC       = sum_raceconds       # Sum with race condition
SM        = sum_mutex           # Sum synchronised by mutex
SEM        = sum_sem           # Sum synchronised by semaphores
PS        = prodcons_sem        # Producer-consumer synchronized by semaphores
M         = matrix              # Power of matrix synchronized by bariers

TARGETS   = $(SRCP) $(SMP) $(PSP) $(SRC) $(SM) $(SEM) $(PS) $(M) 

# Rules
all: $(TARGETS)

# Compile & link C programs
% : %.c
	$(CC) $(CFLAGS)  $< $(LIBS) -o $@

# Compile & link C++ programs
% : %.cpp
	$(CXX) -std=c++11 $(CXXFLAGS) $< $(LIBS) -o $@ 

# Delete all binary files and core file
clean:
	\rm -f $(TARGETS) *~ core

