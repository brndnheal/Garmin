CC=gcc
CXX=g++
RM=rm -f

SRCS=gar.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: gar

gar: $(OBJS)
	    $(CXX) -Wall -o gar $(OBJS) 


clean:
	    $(RM) $(OBJS)

distclean: clean
	    $(RM) gar
