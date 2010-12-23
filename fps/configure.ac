#/*****************************************************************************
#/ @(#) $Id: configure.ac,v 1.2 2003/12/05 17:13:17 virzak Exp $ 
#/ 
#/  Description: 
#/      This file generates the ./configure script with autoconf
#/
#/  =
#/
#/*****************************************************************************

# Process this file with autoconf to produce a configure script.
AC_INIT([FPSPadonak], [0.01], [virzak@rogers.com])
AC_COPYRIGHT([Pizdite skol'ko hotite, vse ravno eto huynya!])
AC_CONFIG_SRCDIR([resource.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX([icc g++ CC])
#AC_PROG_CC([icc gcc cc])


# Checks for libraries.
AC_CHECK_LIB([GL], [glVertex3f])
AC_CHECK_LIB([GLU], [gluPerspective])
AC_CHECK_LIB([Xxf86vm], [XF86VidModeQueryVersion])
AC_CHECK_LIB([jpeg], [jpeg_read_header])
AC_CHECK_LIB([stdc++], [main],, AC_MSG_ERROR(c++ not found))


dnl AC_PREREQ_LIBTOOL(1.4.0, grutil_lib="libXv.a", grutil_lib="libXv.so")
grutil_lib="libgrutil.a"
AC_ARG_WITH(grutil-path,[  --with-grutil-path=path     Where $grutil_lib is installed],
            grutil_path="$withval" LDFLAGS="$LDFLAGS -L$grutil_path/lib" 
            CXXFLAGS="$CXXFLAGS -I$grutil_path/include", 
          grutil_path="/usr/lib")
            
AC_CHECK_LIB([grutil], [grutilFunctionForConfigSrcipt], 
  [ AC_MSG_CHECKING(for $grutil_lib location)
    if test -f "$grutil_path/lib/$grutil_lib"; then
      AC_MSG_RESULT(found in $grutil_path)
      GRUTIL_LIB="-L$grutil_path/lib -lgrutil"
      LIBS="$LIBS $GRUTIL_LIB"
      AC_DEFINE(HAVE_GRUTIL,1,[Define this if you have libgrutil installed])
      ac_have_grutil="yes"
        
      if test x$grutil_lib = "xlibgrutil.a" ; then
        AC_DEFINE(HAVE_GRUTIL_STATIC,1,[Define this if you have libgrutil.a])
      fi              
      
    else
      AC_MSG_ERROR(grutil needed huy)
    fi ]

,AC_MSG_ERROR(grutil needed grutil_path $grutil_path LDFLAGS $LDFLAGS),
[$X_LIBS $X_PRE_LIBS $X_EXTRA_LIBS])

if test x$ac_have_grutil = "xyes"; then
  EXTRA_X_LIBS="-L$grutil_path $GRUTIL_LIB"
  EXTRA_X_CFLAGS=""
fi
AC_SUBST(GRUTIL_LIB)
AC_SUBST(EXTRA_X_LIBS)
AC_SUBST(EXTRA_X_CFLAGS)



AC_ARG_WITH(patch-path,[  --with-patch-path=path     Where $patch_lib is installed],
            patch_path="$withval" LDFLAGS="$LDFLAGS -L$patch_path/lib" 
            CXXFLAGS="$CXXFLAGS -I$patch_path/include", patch_path="/usr/lib")
            
AC_CHECK_LIB([patch], [patchFunctionForConfigSrcipt], 
,AC_MSG_ERROR(patch needed))


AC_ARG_WITH(subdiv-path,[  --with-subdiv-path=path     Where $subdiv_lib is installed],
            subdiv_path="$withval" LDFLAGS="$LDFLAGS -L$subdiv_path/lib" 
            CXXFLAGS="$CXXFLAGS -I$subdiv_path/include", subdiv_path="/usr/lib")
            
AC_CHECK_LIB([subdiv], [subdivFunctionForConfigSrcipt], 
,AC_MSG_ERROR(subdiv needed))





#AC_MSG_ERROR([APR-util not found.  Please read the documentation.])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h memory.h stdlib.h string.h sys/timeb.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([ftime memset sqrt])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 test/Makefile
                 test/output_test/Makefile])
AC_OUTPUT


#/
#/ @(#) $Log: configure.ac,v $
#/ @(#) Revision 1.2  2003/12/05 17:13:17  virzak
#/ @(#) added patch, util
#/ @(#)
#/ @(#) Revision 1.1  2003/09/04 23:47:30  virzak
#/ @(#) Initia revision
#/ @(#)
#/


