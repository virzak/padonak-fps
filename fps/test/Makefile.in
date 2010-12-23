#/*****************************************************************************
#/ @(#) $Id: Makefile.in,v 1.1 2003/09/04 02:41:30 virzak Exp $ 
#/ 
#/  Description: 
#/      This is a test-level Makefile
#/
#/  =
#/
#/*****************************************************************************


SUBDIRS = output_test
all clean distclean:
	@set fnord $(MAKEFLAGS); amf=$$2; \
	target=`echo $@ | sed s/-recursive//`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  echo "Making $$target in $$subdir"; \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$target) \
	   || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
	done; \
	test -z "$$fail"


#/
#/ @(#) $Log: Makefile.in,v $
#/ @(#) Revision 1.1  2003/09/04 02:41:30  virzak
#/ @(#) Added test level Makefile
#/ @(#)
#/


