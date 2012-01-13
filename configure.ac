dnl Process this file with autoconf to produce a configure script.
AC_INIT([DB-Hub],0.452,,dbhub)
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER(config.h)
AM_SANITY_CHECK

CFLAGS="$CFLAGS -D_GNU_SOURCE"

dnl Define debug feature
AC_MSG_CHECKING([whether to include full debugging])
AC_ARG_ENABLE(debug,
	AC_HELP_STRING([--enable-debug],
   		  [Enable extended ebugging features]),
	[case "${enableval}" in
		yes) debug=yes;;
		no) debug=no;;
		*) AC_MSG_ERROR([Specified value ${enableval} for --enable-debug is invalid]);;
	 esac],
	[debug=no]
)
AC_MSG_RESULT($debug)

dnl Check if we want switchuser
AC_MSG_CHECKING([whether to include switch_user support])
AC_ARG_ENABLE(switch_user,
	AC_HELP_STRING([--enable-switch_user],
		[Enable User-Switching support]),
	[case "${enableval}" in
		yes) want_switchuser=yes;;
		no) want_switchuser=no;;
		*) AC_MSG_ERROR([Specified value ${enableval} for --enable-switch_user is invalid]);;
	 esac],
	[want_switchuser=no]
)
AC_MSG_RESULT($want_switchuser)

dnl Check if we want perl support
AC_MSG_CHECKING([whether to include perl support])
AC_ARG_ENABLE(perl,
	AC_HELP_STRING([--enable-perl], [Enable perl scripting support]),
	[case "${enableval}" in
		yes) want_perl=yes;;
		no) want_perl=no;;
		*) AC_MSG_ERROR([Specified value ${enableval} for --enable-perl is invalid]);;
	 esac],
	[want_perl="no"]
)
AC_MSG_RESULT($want_perl)

AC_MSG_CHECKING([whether to include NLS])
AC_ARG_ENABLE(nls,
	AC_HELP_STRING([--disable-nls],
   		  [Disable NLS (National language support)]),
	[case "${enableval}" in
		yes) want_nls=yes;;
		no) want_nls=no;;
		*) AC_MSG_ERROR([Specified value ${enableval} for --disable-nls is invalid]);;
	 esac],
	[want_nls="yes"]
)
AC_MSG_RESULT($want_nls)

AC_PROG_CC
AC_HEADER_STDC

dnl Checking FCNTL Header
AC_CHECK_HEADERS([fcntl.h],,
	AC_MSG_ERROR([Mandatory Header missing])
)
AC_CHECK_LIB(crypt, crypt)

if test "$debug" = "yes"; then
	AC_DEFINE([DEBUG],,[We want full debugging support])
	CFLAGS="$CFLAGS -g -Wall -Wextra"
	AC_CHECK_HEADERS([execinfo.h],[LDFLAGS="$LDFLAGS -rdynamic";HAVE_EXEC="yes"],[HAVE_EXEC="no"])
fi
AM_CONDITIONAL([BDEBUG],[test "$HAVE_EXEC" = "yes"])


if test "$want_switchuser" = "yes"; then
	HAVE_CAP="true"
	MISSING_CAP_HEADERS=""
	AC_CHECK_HEADERS([sys/capability.h sys/prctl.h pwd.h grp.h],,HAVE_CAP="false"; MISSING_CAP_HEADERS="$ac_header $MISSING_CAP_HEADERS")
	AC_CHECK_LIB([cap],[cap_init],,
		LIBCAP_MISSING="true"
		HAVE_CAP="false")
			
	if test "$HAVE_CAP" = "true"; then
		AC_DEFINE([SWITCH_USER], [], [Switch user if started as root])
	else
		AC_MSG_WARN([Stuff for Switch-User missing])
	fi
fi


if test "$want_perl" = "yes";then
	HAVE_PERL="true"
	AC_CHECK_PROG(HAVE_PERL_INT, perl, yes, no)
	if test "$HAVE_PERL_INT" = "yes"; then
		AC_MSG_CHECKING([Perl C Flags])
		perl_ccflags=`perl -MExtUtils::Embed -e ccopts`
		AC_MSG_RESULT([$perl_ccflags])
		for item in $(perl -MExtUtils::Embed -e ldopts); do
			if test "${item: -12}" = "DynaLoader.a"; then
				perl_loader=$item
			fi
			if test "${item:0:1}" = "-"; then
				perl_ld="$perl_ld $item"
			else
				perl_ldd="$perl_ldd $item"
			fi
		done
		AC_MSG_CHECKING([Perl LD FLags])
		AC_MSG_RESULT([$perl_ld])
		AC_MSG_CHECKING([Perl LD Objs])
		AC_MSG_RESULT([$perl_ldd])
		CPPFLAGS="$CPPFLAGS $perl_ccflags"
		LIBS="$LIBS $perl_ld"
		MISSING_PERL_HEADERS=""
		AC_CHECK_HEADERS([EXTERN.h perl.h],,HAVE_PERL="false"; MISSING_PERL_HEADERS="$ac_header $MISSING_PERL_HEADERS",[[#if HAVE_EXTERN_H
		#include <EXTERN.h>
		#endif
		]])
		AC_CHECK_LIB([perl],[perl_run],,LIBPERL_MISSING="true";HAVE_PERL="false")
	else
		HAVE_PERL="false"
	fi
	if test "$HAVE_PERL" = "true" ; then
		AC_DEFINE([HAVE_PERL],[],[Define we got perl at hand])
	else
		AC_MSG_WARN([Stuff for Perl-Scripting missing])
	fi
fi

AM_CONDITIONAL([BPERL],[test "$HAVE_PERL" = "true"])

if test "$want_nls" = "yes"; then
	AC_DEFINE(HAVE_LANG, [], [Define HAVE_LANG])
fi

AM_CONDITIONAL([BNLS],[test "$want_nls" = "yes"])

AC_OUTPUT(Makefile src/Makefile)

echo -e  "\n\e@<:@1m-------------------------------------------------\e@<:@0m"
echo -en "\e@<:@1m Full Debugging is turned: "
if test "$debug" = "yes" ; then
	echo -e "\e@<:@32mon \e@<:@0m"
else
	echo -e "\e@<:@31moff\e@<:@0m"
fi
echo -en "\e@<:@1m Switch-User support requested: "
if test "$want_switchuser" = "yes"; then
	echo -e "\e@<:@32myes\e@<:@0m"
else
	echo -e "\e@<:@31mno \e@<:@0m"
fi
if test "$want_switchuser" = "yes"; then
	if test "$LIBCAP_MISSING" = "true"; then
		echo -e "\e@<:@31m    But libcap seems to be missing\e@<:@0m"
	fi
	if test "$MISSING_CAP_HEADERS" != ""; then
		echo -e "\e@<:@31m    But ${MISSING_CAP_HEADERS}header(s) missing\e@<:@0m"
	fi
	echo -en "\e@<:@1m Switch-User is turned: "
	if test "$HAVE_CAP" = "true" ; then
		echo -e "\e@<:@32mon \e@<:@0m"
	else
		echo -e "\e@<:@31moff\e@<:@0m"
	fi
fi
echo -en "\e@<:@1m Perl support requested: "
if test "$want_perl" = "yes"; then
	echo -e "\e@<:@32myes\e@<:@0m"
	if test "$HAVE_PERL_INT" = "no" ; then
		echo -e "\e@<:@31m    But there's no perl interpreter\e@<:@0m"
	fi
	if test "$MISSING_PERL_HEADERS" != ""; then
		echo -e "\e@<:@31m    But ${MISSING_PERL_HEADERS}header(s) missing\e@<:@0m"
	fi
	if test "$LIBPERL_MISSING" = "true"; then
		echo -e "\e@<:@31m    But libperl seems to be missing\e@<:@0m"
	fi
	if test "$HAVE_DYNLOAD" = "no"; then
		echo -e "\e@<:@31m    But DynaLoader.a seems to be missing\e@<:@0m"
	fi
	echo -en "\e@<:@1m Perl support is turned: "
	if test "$HAVE_PERL" = "true" ; then
		echo -e "\e@<:@32mon \e@<:@0m"
	else
		echo -e "\e@<:@31moff\e@<:@0m"
	fi
else
	echo -e "\e@<:@31mno \e@<:@0m"
fi
echo -en "\e@<:@1m NLS support is turned: "
if test "$want_nls" = "yes" ; then
	echo -e "\e@<:@32mon \e@<:@0m"
else
	echo -e "\e@<:@31moff\e@<:@0m"
fi

echo -e  "\e@<:@1m-------------------------------------------------\e@<:@0m"
echo ""
echo "*** Type \"gmake\" to build dbhub ***"
echo "*** If \"gmake\" does not work, try \"make\" ***"
echo ""
