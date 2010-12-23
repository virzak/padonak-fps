# rules for OCTANES (using gmake)
#UTIL_DIR=/home/virzak/fps



RANLIB=ranlib


OBJS=gr.o grsh.o togl.o ../ui_generic/trackball.o ../ui_generic/pic.o 

ifeq ($(shell uname), IRIX64) 
CC = cc

SUPPLIED_STUFF = /u/gr/cs488
SUPPLIED_ARCH  = /u/gr/cs488/irix4d

CFLAGS= -I${SUPPLIED_STUFF}/include   \
        -g -O3

LIBS=   -rpath ${SUPPLIED_ARCH}/lib \
        -ltk8.0 -ltcl8.0 \
	-lGL -lGLU -lX11 -lXext -lXmu -lm -L${SUPPLIED_ARCH}/lib -lpng 
endif

# rules for LINUX 
ifeq ($(shell uname), Linux)
CXX=g++
CXXFLAGS=-I/usr/include/tcl8.3 -I../include -I../../grutil/include \
	-I../ui_generic/ -g -Wall -Werror -O3

  ifeq (${CXX}, icc)
    CXXFLAGS += -wd981,383
  endif

LIBS= 	\
	-L/usr/lib -ltcl8.3 \
	-L/usr/lib -ltk8.3 \
	-lGL -lGLU \
	-L/usr/X11R6/lib -lX11 -lXext -lXmu -lm -lpng
endif

all : sp


togl.o: togl.cpp
	${CXX} -I/usr/include/tcl8.3 -w -g -O3 -c -o $@ $<


# define how to make the shell 
sp:  ${OBJS}
	$(CXX) -o sp ${OBJS} $(GRLIBS) $(LIBS) -lgrutil -lpatch \
	-L../../grutil/lib -L../lib

clean:
	-rm -f core *.o *~

Clean: clean
	-rm -f sp

rebuild: Clean sp
