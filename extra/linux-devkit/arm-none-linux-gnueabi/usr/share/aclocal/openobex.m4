AC_DEFUN([AM_PATH_OPENOBEX], [
	if (test "${prefix}" = "NONE"); then
		openobex_prefix=${ac_default_prefix}
	else
		openobex_prefix=${prefix}
	fi

	AC_ARG_WITH(openobex, AC_HELP_STRING([--with-openobex=DIR], [OpenOBEX library is installed in DIR]), [
		if (test "${withval}" != "yes"); then
			openobex_prefix=${withval}
		fi
	])

	ac_save_CPPFLAGS=$CPPFLAGS
	ac_save_LDFLAGS=$LDFLAGS

	OPENOBEX_CFLAGS=""
	test -d "${openobex_prefix}/include" && OPENOBEX_CFLAGS="$OPENOBEX_CFLAGS -I${openobex_prefix}/include"

	CPPFLAGS="$CPPFLAGS $OPENOBEX_CFLAGS"
	AC_CHECK_HEADER(openobex/obex.h,, AC_MSG_ERROR(OpenOBEX header files not found))

	OPENOBEX_LIBS=""
	if (test "${ac_default_prefix}" = "${openobex_prefix}"); then
		test -d "${libdir}" && OPENOBEX_LIBS="$OPENOBEX_LIBS -L${libdir}"
	else
		test -d "${openobex_prefix}/lib64" && OPENOBEX_LIBS="$OPENOBEX_LIBS -L${openobex_prefix}/lib64"
		test -d "${openobex_prefix}/lib" && OPENOBEX_LIBS="$OPENOBEX_LIBS -L${openobex_prefix}/lib"
	fi

	LDFLAGS="$LDFLAGS $OPENOBEX_LIBS"
	AC_CHECK_LIB(openobex, OBEX_Init, OPENOBEX_LIBS="$OPENOBEX_LIBS -lopenobex", AC_MSG_ERROR(OpenOBEX library not found))

	CPPFLAGS=$ac_save_CPPFLAGS
	LDFLAGS=$ac_save_LDFLAGS

	AC_SUBST(OPENOBEX_CFLAGS)
	AC_SUBST(OPENOBEX_LIBS)
])
