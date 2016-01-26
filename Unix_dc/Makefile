# $Id: Makefile,v 1.11 2014-07-02 20:01:17-07 - - $
COMPILECPP  = g++ -g -O0 -Wall -Wextra -std=gnu++11
MAKEDEPCPP  = g++ -MM

CPPHEADER   = bigint.h   scanner.h   debug.h   util.h   iterstack.h
CPPSOURCE   = bigint.cpp scanner.cpp debug.cpp util.cpp main.cpp
EXECBIN     = ydc
OBJECTS     = ${CPPSOURCE:.cpp=.o}

${EXECBIN} : ${OBJECTS}
	${COMPILECPP} -o $@ ${OBJECTS}

%.o : %.cpp
	${COMPILECPP} -c $<

clean : 
	rm -f *.o ydc
