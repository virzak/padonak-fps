PATCHOBJS=subdiv.o polyhedron.o
RANLIB=ranlib

CXX=g++
CXXFLAGS=-I../include -I../../grutil/include -g -Wall -Werror -O3

libsubdiv.a: ${PATCHOBJS}
	ar rc $@ ${PATCHOBJS} 
	${RANLIB} $@
	cp $@ ../lib

clean:
	-rm -f core *.o *~


