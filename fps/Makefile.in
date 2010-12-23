#/*****************************************************************************
#/ @(#) $Id: Makefile.in,v 1.8 2003/10/21 08:45:48 virzak Exp $ 
#/ 
#/  Description: 
#/      This is a top-level Makefile
#/
#/  =
#/
#/*****************************************************************************


SUBDIRS = src test
all clean distclean:
	@set fnord $(MAKEFLAGS); amf=$$2; \
	target=`echo $@ | sed s/-recursive//`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  echo "Making $$target in $$subdir"; \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$target) \
	   || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
	done; \
	test -z "$$fail"


        
ifeq ($(shell uname), Linux)
  FPS_BIN_NAME_MACHINE=-linux
  PATH_TO_EXECUTABLE=src/fps
endif

ifeq ($(shell uname), CYGWIN_NT-5.1)
  FPS_BIN_NAME_MACHINE=-win
  PATH_TO_EXECUTABLE=Release/fps.exe
endif


#Use: make all bin-release CONF=release
FPS_BIN_NAME=fps-bin${FPS_BIN_NAME_MACHINE}
FPS_BIN_TEMP_DIR=${FPS_BIN_NAME}

fps-bin bin-release: 
	@echo Removing old archives
	@rm -rf ${FPS_BIN_TEMP_DIR} ${FPS_BIN_NAME}.tar.gz \
	  ${FPS_BIN_NAME}.tar.bz2 ${FPS_BIN_NAME}.zip
	@echo Creating a temporary directory
	@mkdir ${FPS_BIN_TEMP_DIR}
	@echo Copying data into the temporary directory
	@cp -rp data ${FPS_BIN_TEMP_DIR}
	@cp ${PATH_TO_EXECUTABLE} ${FPS_BIN_TEMP_DIR}
	@mkdir ${FPS_BIN_TEMP_DIR}/docs
	@cp THANKS TODO FEATURES AUTHORS ${FPS_BIN_TEMP_DIR}/docs/
	@cp README ${FPS_BIN_TEMP_DIR}/
	@find ${FPS_BIN_TEMP_DIR} -name "CVS" -type d -print | xargs rm -rf
	@find ${FPS_BIN_TEMP_DIR} -name ".xvpics" -type d -print | xargs rm -rf
	@find ${FPS_BIN_TEMP_DIR} -name ".cvsignore" -type f -print | \
	  xargs rm -rf
	@echo Preparing ${FPS_BIN_NAME}.tar.gz
	@tar -czf ${FPS_BIN_NAME}.tar.gz ${FPS_BIN_TEMP_DIR}
	@echo Preparing ${FPS_BIN_NAME}.tar.bz2
	@tar -cjf ${FPS_BIN_NAME}.tar.bz2 ${FPS_BIN_TEMP_DIR}
	@echo Preparing ${FPS_BIN_NAME}.zip
	@zip -rq ${FPS_BIN_NAME}.zip ${FPS_BIN_TEMP_DIR}
	@echo Removing a temporary directory
	@rm -rf ${FPS_BIN_TEMP_DIR}



        
#/
#/ @(#) $Log: Makefile.in,v $
#/ @(#) Revision 1.8  2003/10/21 08:45:48  virzak
#/ @(#) README will now be written into the root directory
#/ @(#)
#/ @(#) Revision 1.7  2003/10/10 00:53:19  virzak
#/ @(#) Fixed a bug
#/ @(#)
#/ @(#) Revision 1.6  2003/10/10 00:52:33  virzak
#/ @(#) Added README
#/ @(#)
#/ @(#) Revision 1.5  2003/10/10 00:49:41  virzak
#/ @(#) Added docs directory
#/ @(#)
#/ @(#) Revision 1.4  2003/09/13 00:41:49  virzak
#/ @(#) Fixed for windows
#/ @(#)
#/ @(#) Revision 1.3  2003/09/13 00:09:33  virzak
#/ @(#) Added binary archives build
#/ @(#)
#/ @(#) Revision 1.2  2003/09/04 01:07:26  virzak
#/ @(#) Added distclean target
#/ @(#)
#/ @(#) Revision 1.1  2003/08/22 00:07:32  virzak
#/ @(#) Top Level Makefile
#/ @(#)
#/

