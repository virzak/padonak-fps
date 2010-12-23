CXX=g++
CXXFLAGS=-O2 -g -Wall -I../../grutil/include -I../ui_generic -I../include
  ifeq (${CXX},icc)
    CXXFLAGS+=-wd981
  endif


target=subdiv
LIBS=   -lGL -lGLU -lglut -lgrutil -lsubdiv -L../../grutil/lib -L../lib
        
OBJS=engine_sd.o ../ui_generic/trackball.o 

${target}: ${OBJS} ../lib/libsubdiv.a
	$(CXX) $(CXXFLAGS) -o $@ ${OBJS} $(GRLIBS) $(LIBS)

clean:
	-rm -f core ${OBJS} *~ ${MAIN_TCL_HEADER}

Clean: clean
	-rm -f ${target}

rebuild: Clean ${target}

