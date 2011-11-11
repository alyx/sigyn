dnl
dnl  To generate the configure script, call:
dnl    aclocal -I m4
dnl    autoconf
dnl

AC_PREREQ(2.59)

dnl Specify name, version and contact e-mail of the package.
AC_INIT(sigyn, 1.0.0-beta, bugs@ethrik.net)

AC_CONFIG_AUX_DIR(autoconf)

AC_CONFIG_HEADER(include/config.h)
AC_PREFIX_DEFAULT(~/sigyn)


AC_SUBST([PACKAGE], [AC_PACKAGE_TARNAME])
AC_SUBST([VERSION], [AC_PACKAGE_VERSION])

AC_DEFINE_UNQUOTED(VERSION, "$VERSION", [Version number of package])
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE", [Name of package])
dnl Specifies where your sources are.
AC_CONFIG_SRCDIR(src)

dnl If your package uses C, you want a C compiler and a C preprocessor
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL

AC_MSG_CHECKING(how to generate dependancy info)
if test "$SunWorkShop" = "yes"; then
    AC_MSG_RESULT(Sun WorkShop/Forte using $CC -xM)
    MKDEP="$CC -xM"
fi
if test "$HPUX" = "yes"; then
    AC_MSG_RESULT(HP-UX C Compiler using makedepend)
    MKDEP="makedepend"
fi
if test "x$MKDEP" = "x"; then
    AC_MSG_RESULT(assuming $CC -MM)
    MKDEP="$CC -MM"
fi

AC_SUBST(MKDEP)

AC_CHECK_FUNCS([strdup gettimeofday umask mmap getrlimit fork getpid execve])
AC_CHECK_FUNC(socket,, AC_CHECK_LIB(socket, socket))
AC_CHECK_FUNC(gethostbyname,, AC_CHECK_LIB(nsl, gethostbyname))


MOWGLI_CFLAGS="-I../libmowgli-2/src/libmowgli -I../../libmowgli-2/src/libmowgli -I../../../libmowgli-2/src/libmowgli"
MOWGLI_LIBS="-L../libmowgli-2/src/libmowgli -L../../libmowgli-2/src/libmowgli -L../../../libmowgli-2/src/libmowgli -lmowgli-2"
LIBMOWGLI="libmowgli-2"
RPATH="-Wl,-rpath,${libdir}"
AC_CONFIG_SUBDIRS([libmowgli-2])

BUILDROOT=`pwd`
AC_SUBST(BUILDROOT)
dnl It will automatically set the required LIB_* and PLUGIN_* variables.
dnl See m4/buildsys.m4 for more details.
dnl Uncomment the following line if you want to build a shared library.
dnl BUILDSYS_SHARED_LIB

dnl If your program uses plugins and should run on win32 as well, you need to
dnl uncomment the following line
dnl BUILDSYS_PROG_IMPLIB

dnl We need this in order to run some initialization needed by the buildsys
BUILDSYS_INIT
dnl You need to call this to generate the .deps files required by the buildsys.
BUILDSYS_TOUCH_DEPS

dnl Set the name of your package here. It will be used in the default include
dnl paths for example. (Can be changed, see buildsys.mk.in for details.)
AC_SUBST(PACKAGE, name)

dnl If you don't need any extra variables
dnl If you need any extra variables, they go to extra.mk.
dnl If you use extra variables, comment the line above and uncomment the line
dnl below.
AC_CONFIG_FILES([buildsys.mk extra.mk])

dnl Generate config.h if we did some defines.
dnl _CONFIG_HEADERS(config.h)

dnl Write out configure script
AC_OUTPUT
