# $Id: Makefile,v 1.5 2014-05-27 15:55:33-07 - - $

GPP        = g++ -g -O0 -Wall -Wextra -std=gnu++11

DEPFILE    = Makefile.dep
HEADERS    = sockets.h signal_action.h cix_protocol.h logstream.h
CPPLIBS    = sockets.cpp signal_action.cpp cix_protocol.cpp 
CPPSRCS    = ${CPPLIBS} cix-daemon.cpp  cix-server.cpp cix-client.cpp
LIBOBJS    = ${CPPLIBS:.cpp=.o}
CLIENTOBJS = cix-client.o ${LIBOBJS}
SERVEROBJS = cix-server.o ${LIBOBJS}
DAEMONOBJS = cix-daemon.o ${LIBOBJS}
OBJECTS    = ${CLIENTOBJS} ${SERVEROBJS} ${DAEMONOBJS} ${CLIENTOBJS}
EXECBINS   =  cix-server cix-daemon cix-client
LISTING    = Listing.ps
SOURCES    = ${HEADERS} ${CPPSRCS} Makefile
CLASS       = cmps109-wm.u14
PROJECT     = asg5

all: ${DEPFILE} ${EXECBINS}

cix-client: ${CLIENTOBJS}
	${GPP} -o $@ ${CLIENTOBJS}

cix-server: ${SERVEROBJS}
	${GPP} -o $@ ${SERVEROBJS}

cix-daemon: ${DAEMONOBJS}
	${GPP} -o $@ ${DAEMONOBJS}

%.o: %.cpp
	${GPP} -c $<

ci:
	- checksource ${SOURCES}
	- cid + ${SOURCES}

lis: all ${SOURCES} ${DEPFILE}
	mkpspdf ${LISTING} ${SOURCES} ${DEPFILE}

clean:
	- rm  ${OBJECTS}

spotless: clean
	- rm ${EXECBINS}

dep:
	- rm ${DEPFILE}
	make --no-print-directory ${DEPFILE}

${DEPFILE}:
	${GPP} -MM ${CPPSRCS} >${DEPFILE}

again: ${SOURCES}
	make --no-print-directory spotless ci all lis

include ${DEPFILE}

submit : ${SOURCES}
	- checksource ${SOURCES}
	submit ${CLASS} ${PROJECT} ${SOURCES}