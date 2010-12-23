PATCHOBJS=patch.o
RANLIB=ranlib

CXX=g++
CXXFLAGS=-I../include -I../../grutil/include -g -Wall -Werror -O3

libpatch.a: ${PATCHOBJS}
	ar rc $@ ${PATCHOBJS} 
	${RANLIB} $@

clean:
	-rm -f core *.o *~


