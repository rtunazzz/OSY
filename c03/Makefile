CC        = gcc
CFLAGS    = -Wall -pedantic -ansi 
LIBS      = -lpthread  
CXX       = g++
CXXFLAGS  = -Wall -pedantic

TARGETS   = prodcons_cv_01 condvar_pt_01 condvar_01    

all: $(TARGETS)

prodcons_cv_01: prodcons_cv_01.cpp
	$(CXX) -std=c++11 $(CXXFLAGS) -o $@ $< $(LIBS)

condvar_pt_01: condvar_pt_01.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

condvar_01: condvar_01.cpp
	$(CXX) -std=c++11 $(CXXFLAGS) -o $@ $< $(LIBS)

clean:
	\rm -f $(TARGETS) *~ core

