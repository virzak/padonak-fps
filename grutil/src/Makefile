UTILOBJS=util.o
RANLIB=ranlib

CXX=g++
CXXFLAGS=-I../include -g -Wall -Werror -O3

libgrutil.a: ${UTILOBJS}
	ar rc ../lib/$@ ${UTILOBJS}
	${RANLIB} ../lib/$@

clean:
	-rm -f core *.o *~


