dnl AM_PATH_ORBIT2([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for ORBit2, and define ORBIT_CFLAGS and ORBIT_LIBS
dnl
AC_DEFUN([AM_PATH_ORBIT2],
[dnl 
dnl Get the cflags and libraries from the orbit2-config script
dnl
AC_ARG_WITH(orbit-prefix,[  --with-orbit-prefix=PFX   Prefix where ORBIT is installed (optional)],
            orbit_config_prefix="$withval", orbit_config_prefix="")
AC_ARG_WITH(orbit-exec-prefix,[  --with-orbit-exec-prefix=PFX Exec prefix where ORBIT is installed (optional)],
            orbit_config_exec_prefix="$withval", orbit_config_exec_prefix="")
AC_ARG_ENABLE(orbittest, [  --disable-orbittest       Do not try to compile and run a test ORBIT program],
		    , enable_orbittest=yes)

  for module in . $4
  do
      case "$module" in
         server) 
             orbit_config_args="$orbit_config_args server"
	 ;;
         client) 
             orbit_config_args="$orbit_config_args client"
         ;;
      esac
  done
  if test x$orbit_config_args = x ; then
	orbit_config_args=server
  fi

  if test x$orbit_config_exec_prefix != x ; then
     orbit_config_args="$orbit_config_args --exec-prefix=$orbit_config_exec_prefix"
     if test x${ORBIT_CONFIG+set} != xset ; then
        ORBIT_CONFIG=$orbit_config_exec_prefix/bin/orbit2-config
     fi
  fi
  if test x$orbit_config_prefix != x ; then
     orbit_config_args="$orbit_config_args --prefix=$orbit_config_prefix"
     if test x${ORBIT_CONFIG+set} != xset ; then
        ORBIT_CONFIG=$orbit_config_prefix/bin/orbit2-config
     fi
  fi

  AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  AC_PATH_PROG(ORBIT_CONFIG, orbit2-config, no)
  min_orbit_version=ifelse([$1], , 2.3.0, $1)
  AC_MSG_CHECKING(for ORBit - version >= $min_orbit_version)
  no_orbit=""
  if test "$ORBIT_CONFIG" = "no" ; then
    if test "$PKG_CONFIG" = "no" ; then
      no_orbit=yes
    else
      ORBIT_CFLAGS=`$PKG_CONFIG --cflags ORBit-2.0`
      ORBIT_LIBS=`$PKG_CONFIG --libs ORBit-2.0`
    fi
  else
    ORBIT_CFLAGS=`$ORBIT_CONFIG $orbit_config_args --cflags`
    ORBIT_LIBS=`$ORBIT_CONFIG $orbit_config_args --libs`
  fi

  if test "x$no_orbit" = x ; then
    ORBIT_VERSION=`$PKG_CONFIG --modversion ORBit-2.0`

    orbit_config_major_version=`echo $ORBIT_VERSION | \
	   sed -e 's,[[^0-9.]],,g' -e 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    orbit_config_minor_version=`echo $ORBIT_VERSION | \
	   sed -e 's,[[^0-9.]],,g' -e 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    orbit_config_micro_version=`echo $ORBIT_VERSION | \
	   sed -e 's,[[^0-9.]],,g' -e 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_orbittest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $ORBIT_CFLAGS"
      LIBS="$ORBIT_LIBS $LIBS"
dnl
dnl Now check if the installed ORBIT is sufficiently new. (Also sanity
dnl checks the results of orbit2-config to some extent
dnl
      rm -f conf.orbittest
      AC_TRY_RUN([
#include <orbit/orbit.h>
#include <stdio.h>
#include <stdlib.h>

int 
main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.orbittest");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = g_strdup("$min_orbit_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_orbit_version");
     exit(1);
   }

  if ((orbit_major_version != $orbit_config_major_version) ||
      (orbit_minor_version != $orbit_config_minor_version) ||
      (orbit_micro_version != $orbit_config_micro_version))
    {
      printf("\n*** 'pkg-config --version ORBit-2.0' returned %d.%d.%d, but ORBit (%d.%d.%d)\n", 
             $orbit_config_major_version, $orbit_config_minor_version, $orbit_config_micro_version,
             orbit_major_version, orbit_minor_version, orbit_micro_version);
      printf ("*** was found! If orbit2-config was correct, then it is best\n");
      printf ("*** to remove the old version of ORBit. You may also be able to fix the error\n");
      printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
      printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
      printf("*** required on your system.\n");
      printf("*** If orbit2-config was wrong, set the environment variable ORBIT_CONFIG\n");
      printf("*** to point to the correct copy of orbit2-config, and remove the file config.cache\n");
      printf("*** before re-running configure\n");
    } 
#if defined (ORBIT_MAJOR_VERSION) && defined (ORBIT_MINOR_VERSION) && defined (ORBIT_MICRO_VERSION)
  else if ((orbit_major_version != ORBIT_MAJOR_VERSION) ||
	   (orbit_minor_version != ORBIT_MINOR_VERSION) ||
           (orbit_micro_version != ORBIT_MICRO_VERSION))
    {
      printf("*** ORBit header files (version %d.%d.%d) do not match\n",
	     ORBIT_MAJOR_VERSION, ORBIT_MINOR_VERSION, ORBIT_MICRO_VERSION);
      printf("*** library (version %d.%d.%d)\n",
	     orbit_major_version, orbit_minor_version, orbit_micro_version);
    }
#endif /* defined (ORBIT_MAJOR_VERSION) ... */
  else
    {
      if ((orbit_major_version > major) ||
        ((orbit_major_version == major) && (orbit_minor_version > minor)) ||
        ((orbit_major_version == major) && (orbit_minor_version == minor) && (orbit_micro_version >= micro)))
      {
        return 0;
       }
     else
      {
        printf("\n*** An old version of ORBit (%d.%d.%d) was found.\n",
               orbit_major_version, orbit_minor_version, orbit_micro_version);
        printf("*** You need a version of ORBit newer than %d.%d.%d. The latest version of\n",
	       major, minor, micro);
        printf("*** ORBit is always available from ftp://ftp.orbit.org.\n");
        printf("***\n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the orbit2-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of ORBit, but you can also set the ORBIT_CONFIG environment to point to the\n");
        printf("*** correct copy of orbit2-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
      }
    }
  return 1;
}
],, no_orbit=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_orbit" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$ORBIT_CONFIG" = "no" ; then
       echo "*** The orbit2-config script installed by ORBIT could not be found"
       echo "*** If ORBit was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the ORBIT_CONFIG environment variable to the"
       echo "*** full path to orbit2-config."
     else
       if test -f conf.orbittest ; then
        :
       else
          echo "*** Could not run ORBIT test program, checking why..."
          CFLAGS="$CFLAGS $ORBIT_CFLAGS"
          LIBS="$LIBS $ORBIT_LIBS"
          AC_TRY_LINK([
#include <orbit/orbit.h>
#include <stdio.h>
],      [ return ((orbit_major_version) || (orbit_minor_version) || (orbit_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding ORBit or finding the wrong"
          echo "*** version of ORBit. If it is not finding ORBit, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"
          echo "***"
          echo "*** If you have a RedHat 5.0 system, you should remove the ORBit package that"
          echo "*** came with the system with the command"
          echo "***"
          echo "***    rpm --erase --nodeps orbit orbit-devel" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means ORBIT was incorrectly installed"
          echo "*** or that you have moved ORBit since it was installed. In the latter case, you"
          echo "*** may want to edit the orbit2-config script: $ORBIT_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     ORBIT_CFLAGS=""
     ORBIT_LIBS=""
     ifelse([$3], , :, [$3])
  fi

  AC_PATH_PROG(ORBIT_IDL, orbit-idl-2, ifelse([$3], , :, [$3]))
  AC_SUBST(ORBIT_CFLAGS)
  AC_SUBST(ORBIT_LIBS)
  AC_SUBST(ORBIT_IDL)
  rm -f conf.orbittest
])
