# Process this file with autoconf to produce a configure script
AC_PREREQ([2.64])
AC_INIT([motion], esyscmd(['./version.sh']), [],
        [motion], [https://github.com/dmiter/motion])
AC_CONFIG_SRCDIR([motion.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_MKDIR_P
AC_PROG_INSTALL

# The following macro makes it possible to use features of Posix that are
# extensions to C, as well as platform extensions not defined by Posix.
AC_USE_SYSTEM_EXTENSIONS

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_TIME
AC_HEADER_DIRENT
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([stdio.h fcntl.h time.h signal.h syslog.h limits.h stddef.h])
AC_CHECK_HEADERS([sys/time.h sys/ioctl.h sys/mman.h sys/param.h sys/socket.h])
AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h termios.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIGNAL
AC_TYPE_MODE_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_INT64_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MMAP
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([select socket gethostbyname gettimeofday localtime_r])
AC_CHECK_FUNCS([dup2 getcwd mkdir munmap putenv setenv regcomp uname])
AC_CHECK_FUNCS([strcasecmp strchr strdup strerror strncasecmp strstr])
AC_CHECK_FUNCS([memchr memmove memset])
AC_CHECK_FUNCS([get_current_dir_name])

TEMP_CFLAGS="$CFLAGS"
TEMP_LIBS="$LIBS"
TEMP_LDFLAGS="$LDFLAGS"

#
# Check to include static objects after some time
#
AC_ARG_ENABLE([static-obj],
    [AS_HELP_STRING([--enable-static-obj],
        [Define the amount of time the static object will be visible in seconds.])],
    [STATIC_OBJ="$enableval"],
    [STATIC_OBJ="no"])

if test "$STATIC_OBJ" != "no"; then
    if test "$STATIC_OBJ" = "yes"; then
        STATIC_OBJ="10"
    fi
    AC_DEFINE_UNQUOTED([ACCEPT_STATIC_OBJECT_TIME], $STATIC_OBJ,
        [Define the amount of time the static object will be visible in seconds.])
fi

#
# Check for V4L support
#
AC_ARG_WITH([v4l],
    [AS_HELP_STRING([--without-v4l],
        [Exclude using v4l (video4linux) subsystem.
         Makes Motion so it only supports network cameras.])],
    [V4L="$withval"],
    [V4L="yes"])

V4L2="no"
VIDEO_OBJ="video_common.o"
if test "$V4L" = "yes"; then
    # Check if v4l is available
    AC_CHECK_HEADERS([linux/videodev.h],
        [V4L="yes"], [V4L="no"])

    AC_MSG_CHECKING([for V4L support])
    if test "$V4L" = "yes"; then
        AC_MSG_RESULT([yes])
        AC_DEFINE([HAVE_V4L], 1, [Define to 1 if you have v4l support.])
        VIDEO_OBJ="$VIDEO_OBJ video.o"

        # Check if v4l2 is available
        AC_CHECK_TYPE([struct v4l2_buffer],
            [V4L2="yes"], [V4L2="no"],
            [#include <sys/time.h>
             #include <linux/videodev.h>])
    else
        AC_MSG_RESULT([no])
    fi

    # <linux/videodev.h> doesn't include <linux/videodev2.h>
    AC_CHECK_HEADERS([linux/videodev2.h],
        [V4L2_OLD="yes"], [V4L2_OLD="no"],
        [#include <asm/types.h>])

    AC_MSG_CHECKING([for V4L2 support])
    if test "$V4L2" = "yes" -o "$V4L2_OLD" = "yes"; then
        AC_MSG_RESULT([yes])
        V4L2="yes"
    else
        AC_MSG_RESULT([no])
    fi

    if test "$V4L2" = "yes"; then
        AC_DEFINE([HAVE_V4L2], 1, [Define to 1 if you have v4l2 support.])
        VIDEO_OBJ="$VIDEO_OBJ video2.o"
    fi
fi
AC_SUBST(VIDEO_OBJ)

#
# Check for sdl library
#
AC_ARG_WITH([sdl],
    [AS_HELP_STRING([--without-sdl],
        [Compile without sdl support to get stream in SDL window.])],
    [SDL="$withval"],
    [SDL="no"])

if test "$SDL" != "no"; then
    AC_MSG_CHECKING([for SDL])
    CONFIG_SDL='sdl-config'
    if test -z "`($CONFIG_SDL --version) 2>/dev/null`"; then
        AC_MSG_RESULT([no])
        if test "$SDL" = "yes"; then
            AC_MSG_WARN([
****************************************************
* sdl-config could not be found. Please install it *
* and remove the --with-sdl configure argument.    *
* libSDL can be found at http://www.libsdl.org     *
****************************************************
])
        fi
    else
        AC_MSG_RESULT([yes])
        SDL="yes"
        TEMP_LIBS="$TEMP_LIBS `$CONFIG_SDL --libs`"
        TEMP_CFLAGS="$TEMP_CFLAGS `$CONFIG_SDL --cflags`"
        AC_DEFINE([HAVE_SDL], 1, [Define to 1 if you have SDL support.])
        SDL_OBJ="sdl.o"
        AC_SUBST(SDL_OBJ)
    fi
fi

#
# Check for the libjpeg-turbo library
#
AC_ARG_WITH([jpeg-turbo],
    [AS_HELP_STRING([--with-jpeg-turbo=<path>],
        [Specify the prefix for the install path for
         jpeg-turbo for optimized jpeg handling (optional).])],
    [JPEG_TURBO="$withval"],
    [JPEG_TURBO="no"])

JPEG_SUPPORT="no"
JPEG_TURBO_SUPPORT="no"
if test "$JPEG_TURBO" != "no"; then
    AC_CHECK_FILE([$JPEG_TURBO/lib/libjpeg.a], [
        JPEG_TURBO_SUPPORT="yes"
        saved_LIBS="$LIBS"
        saved_CFLAGS="$CFLAGS"
        LIBS="$LIBS -L$JPEG_TURBO/lib -ljpeg"
        CFLAGS="$CFLAGS -I$JPEG_TURBO/include"
        AC_CHECK_LIB([jpeg], [jpeg_start_compress], [
            JPEG_SUPPORT="yes"
            TEMP_LIBS="$TEMP_LIBS -L$JPEG_TURBO/lib -ljpeg"
            TEMP_CFLAGS="$TEMP_CFLAGS -I$JPEG_TURBO/include"],
            [AC_MSG_ERROR([Bad jpeg library])],[])
        LIBS="$saved_LIBS"
        CFLAGS="$saved_CFLAGS"
    ], [])
fi

#
# Check for the special mmx accelerated jpeg library
#
AC_ARG_WITH([jpeg-mmx],
    [AS_HELP_STRING([--with-jpeg-mmx=<path>],
        [Specify the prefix for the install path for
         jpeg-mmx for optimized jpeg handling (optional).
         If this is not specified motion will try to find
         the library /usr/lib/libjpeg-mmx.a /usr/local/lib/libjpeg-mmx.a])],
    [JPEG_MMX="$withval"],
    [JPEG_MMX="no"])

#
# --without-jpeg-mmx or with-jpeg-mmx=no
#
JPEG_MMX_SUPPORT="no"
if test "$JPEG_MMX" != "no" && test "$JPEG_SUPPORT" != "yes"; then
    LOOK_AT="/usr/lib64 /usr/lib /usr/local/lib"
    if test "$JPEG_MMX" != "yes"; then
        LOOK_AT="$JPEG_MMX"
    fi
    for dir in $LOOK_AT; do
        # AUTODETECT STATIC LIB
        AC_CHECK_FILE([$dir/libjpeg-mmx.a], [
            JPEG_MMX_SUPPORT="yes"
            JPEG_MMX="$dir"

            saved_LIBS="$LIBS"
            saved_CFLAGS="$CFLAGS"
            LIBS="$LIBS -L$JPEG_MMX"
            CFLAGS="$CFLAGS -I$JPEG_MMX"
            AC_CHECK_LIB([jpeg-mmx], [jpeg_start_compress], [
                JPEG_SUPPORT="yes"
                TEMP_LIBS="$TEMP_LIBS -L$JPEG_MMX -ljpeg-mmx"
                TEMP_CFLAGS="$TEMP_CFLAGS -I$JPEG_MMX"],
                [AC_MSG_ERROR([Bad jpeg-mmx library])],[])
            LIBS="$saved_LIBS"
            CFLAGS="$saved_CFLAGS"
            break
        ], [])
    done
fi

#
# Look for _a_ jpeg lib that will work.
#
if test "$JPEG_SUPPORT" != "yes"; then
    # Checks for libraries
    AC_CHECK_LIB([jpeg], [jpeg_set_defaults], [
        JPEG_SUPPORT="yes"
        TEMP_LIBS="$TEMP_LIBS -ljpeg"],
        [AC_MSG_ERROR([You do not have libjpeg installed])],[])
fi

#
# Check for libavcodec and libavformat from ffmpeg
#
FFMPEG_OBJ=""
FFMPEG_SUPPORT="no"
AC_ARG_WITH([ffmpeg],
    [AS_HELP_STRING([--with-ffmpeg=<path>],
        [Specify the prefix for the install path for
         libavcodec/libavformat (part of ffmpeg) be able to
         encode mpeg movies realtime.
         If this is not specified motion will try to find
         the libraries in /usr and /usr/local.])],
    [FFMPEG_DIR="$withval"],
    [FFMPEG_DIR="yes"])

#
# --with-ffmpeg=<path> or nothing
#
if test "$FFMPEG_DIR" != "no"; then
    LOOK_AT="/usr/lib64 /usr/lib /usr/local/lib"
    if test "$FFMPEG_DIR" != "yes"; then
        LOOK_AT="$FFMPEG_DIR/lib $FFMPEG_DIR"
    fi
    for dir in $LOOK_AT; do
        # AUTODETECT STATIC LIB
        AC_CHECK_FILE([$dir/libavcodec.a], [
            AC_CHECK_FILE([$dir/libavformat.a], [
                FFMPEG_SUPPORT="yes"
                FFMPEG_LIB="$dir"
                FFMPEG_DIR="$(dirname $dir)"
                break
            ], [])
        ], [])
        # AUTODETECT SHARED LIB
        AC_CHECK_FILE([$dir/libavcodec.so], [
            AC_CHECK_FILE([$dir/libavformat.so], [
                FFMPEG_SUPPORT="yes"
                FFMPEG_LIB="$dir"
                FFMPEG_DIR="$(dirname $dir)"
                break
            ], [])
        ], [])
    done
    AC_MSG_CHECKING([for ffmpeg libs])
    if test "$FFMPEG_SUPPORT" = "yes"; then
        AC_MSG_RESULT([$FFMPEG_LIB])
    else
        AC_MSG_RESULT([no])
        AC_MSG_WARN([
**********************************************
* libavcodec.a or libavcodec.so or           *
* libavformat.a or libavformat.so not found: *
*    ALL FFMPEG FEATURES DISABLED            *
*                                            *
* Please read the Motion Guide for help:     *
* http://motion.sourceforge.net              *
**********************************************
])
    fi
fi

#
# ffmpeg headers custom location
#
AC_ARG_WITH([ffmpeg_headers],
    [AS_HELP_STRING([--with-ffmpeg-headers=<path>],
        [Specify the prefix for ffmpeg headers.])],
    [FFMPEG_HEADERS_DIR="$withval"],
    [FFMPEG_HEADERS_DIR="yes"])

#
# Now check for ffmpeg headers (avformat.h) if ffmpeg libs were found
#
if test "$FFMPEG_SUPPORT" = "yes"; then
    FFMPEG_SUPPORT="no"
    if test "$FFMPEG_HEADERS_DIR" != "yes"; then
        FFMPEG_DIR="$FFMPEG_HEADERS_DIR"
    fi
    LOOK_AT="$FFMPEG_DIR/include $FFMPEG_DIR/include/ffmpeg $FFMPEG_DIR"
    for dir in $LOOK_AT; do
        # AUTODETECT avformat.h
        AC_CHECK_FILE([$dir/avformat.h], [
            FFMPEG_SUPPORT="yes"
            FFMPEG_CFLAGS="-I$dir"
            break
        ], [])
        # AUTODETECT libavformat/avformat.h
        AC_CHECK_FILE([$dir/libavformat/avformat.h], [
            FFMPEG_SUPPORT="yes"
            FFMPEG_CFLAGS="-I$dir"
            AC_DEFINE([FFMPEG_NEW_INCLUDES], 1, [Define to 1 if you have the <libavformat/avformat.h> header file.])
            AVFORMAT="-I$dir/libavformat"
            break
        ], [])
    done
    # If ffmpeg libs and headers have been found
    if test "$FFMPEG_SUPPORT" = "yes"; then
        TEMP_LIBS="$TEMP_LIBS -L$FFMPEG_LIB -lavformat -lavcodec -lavutil -lz"
        TEMP_CFLAGS="$TEMP_CFLAGS $FFMPEG_CFLAGS"
        AC_DEFINE([HAVE_FFMPEG], 1, [Define to 1 if you have the ffmpeg.])

        FFMPEG_OBJ="ffmpeg.o"
        AC_SUBST(FFMPEG_OBJ)

        AC_MSG_CHECKING([for file_protocol is defined in ffmpeg])
        saved_LIBS=$LIBS
        saved_CFLAGS=$CFLAGS
        LIBS="$TEMP_LIBS"
        CFLAGS="$FFMPEG_CFLAGS $AVFORMAT"
        AC_COMPILE_IFELSE([
            AC_LANG_SOURCE([
                #include <avformat.h>
                URLProtocol test_file_protocol;
                int main(void) {
                    test_file_protocol.url_read = file_protocol.url_read;
                    return 0;
                }
            ])],
            [AC_MSG_RESULT([yes])],
            [AC_MSG_RESULT([no])
             AC_DEFINE([HAVE_FFMPEG_NEW], 1, [Define to 1 if 'file_protocol' is *NOT* defined in the <avformat.h> header file.])])
        LIBS=$saved_LIBS
        CFLAGS=$saved_CFLAGS
    else
        AC_MSG_WARN([
**********************************************
*       avformat.h not found:                *
*    ALL FFMPEG FEATURES DISABLED            *
*                                            *
* Please read the Motion Guide for help:     *
* http://motion.sourceforge.net              *
**********************************************
])
    fi
fi

#
# Check SQLITE3
#
SQLITE3_SUPPORT="no"
AC_ARG_WITH([sqlite3],
    [AS_HELP_STRING([--without-sqlite3],
        [Disable sqlite3 support in motion.])],
    [SQLITE3="$withval"],
    [SQLITE3="yes"])

if test "$SQLITE3" != "no"; then
    saved_LIBS=$LIBS
    saved_CFLAGS=$CFLAGS
    LIBS="$LIBS -lsqlite3"
    AC_CHECK_LIB([sqlite3], [sqlite3_open], [
        AC_MSG_RESULT([yes])
        SQLITE3_SUPPORT="yes"
        TEMP_LIBS="$TEMP_LIBS -lsqlite3"
        AC_DEFINE([HAVE_SQLITE3], 1, [Define to 1 if you have SQLITE3 support.])],
        [],[])
    LIBS=$saved_LIBS
    CFLAGS=$saved_CFLAGS
fi

#
# Check Mysql
#
MYSQL_SUPPORT="no"
AC_ARG_WITH([mysql],
    [AS_HELP_STRING([--without-mysql],
        [Disable mysql support in motion.])],
    [MYSQL="$withval"],
    [MYSQL="yes"])

AC_ARG_WITH([mysql-lib],
    [AS_HELP_STRING([--with-mysql-lib=<path>],
        [Normally, configure will scan all possible default
         installation paths for mysql libs. When it fails, use
         this command to tell configure where mysql libs
         installation root directory is.])],
    [MYSQL_LIBS="$withval"],
    [MYSQL_LIBS="yes"])

AC_ARG_WITH([mysql-include],
    [AS_HELP_STRING([--with-mysql-include=<path>],
        [Normally, configure will scan all possible default
         installation paths for mysql include. When it fails, use
         this command to tell configure where mysql include
         installation root directory is.])],
    [MYSQL_HEADERS="$withval"],
    [MYSQL_HEADERS="yes"])

if test "$MYSQL" != "no"; then
    # ******* Search mysql headers *******
    LOOK_AT="/usr/include /usr/local/include /usr/mysql /usr/local/mysql /usr/local/mysql/include /opt /opt/mysql"
    if test "$MYSQL_HEADERS" != "yes"; then
        LOOK_AT="$MYSQL_HEADERS"
    fi
    MYSQL_HEADERS="no"
    for dir in $LOOK_AT; do
        # AUTODETECT mysql.h
        AC_CHECK_FILE([$dir/mysql.h], [
            MYSQL_HEADERS="yes"
            MYSQL_INCDIR="$dir"
            break
        ], [])
        # AUTODETECT mysql/mysql.h
        AC_CHECK_FILE([$dir/mysql/mysql.h], [
            MYSQL_HEADERS="yes"
            MYSQL_INCDIR="$dir/mysql"
            break
        ], [])
        # AUTODETECT mysql/include/mysql.h
        AC_CHECK_FILE([$dir/mysql/include/mysql.h], [
            MYSQL_HEADERS="yes"
            MYSQL_INCDIR="$dir/mysql/include"
            break
        ], [])
    done
    if test "$MYSQL_HEADERS" = "yes"; then
        MYSQL_SUPPORT="no"
        # ******* Search mysql libs *********
        LOOK_AT="/usr/lib64 /usr/lib /usr/local/lib /usr/mysql /usr/local/mysql /usr/local/mysql/lib /opt /opt/mysql /usr/lib/x86_64-linux-gnu"
        if test "$MYSQL_LIBS" != "yes"; then
            LOOK_AT="$MYSQL_LIBS"
        fi
        for dir in $LOOK_AT; do
            # AUTODETECT STATIC LIB
            AC_CHECK_FILE([$dir/libmysqlclient.a], [
                MYSQL_SUPPORT="yes"
                MYSQL_LIBDIR="$dir"
                break
            ], [])
            # AUTODETECT SHARED LIB
            AC_CHECK_FILE([$dir/libmysqlclient.so], [
                MYSQL_SUPPORT="yes"
                MYSQL_LIBDIR="$dir"
                break
            ], [])
        done
        AC_MSG_CHECKING([for mysql libs])
        if test "$MYSQL_SUPPORT" = "yes"; then
            AC_MSG_RESULT([$MYSQL_LIBDIR])
            saved_LIBS=$LIBS
            saved_CFLAGS=$CFLAGS
            LIBS="-L$MYSQL_LIBDIR -lmysqlclient -lz"
            CFLAGS="-I$MYSQL_INCDIR"
            AC_CHECK_LIB([mysqlclient], [mysql_init], [
                TEMP_LIBS="$TEMP_LIBS -L$MYSQL_LIBDIR -lmysqlclient -lz"
                TEMP_CFLAGS="$TEMP_CFLAGS -I$MYSQL_INCDIR"
                AC_DEFINE([HAVE_MYSQL], 1, [Define to 1 if you have MySQL support.])],
                [AC_MSG_ERROR([MySQL support can't build without MySQL libraries])],[])
            CFLAGS=$saved_CFLAGS
            LIBS=$saved_LIBS
        else
            AC_MSG_RESULT([no])
        fi
    # end mysql-include, mysql-libs
    fi
# end Mysql detection
fi

#
# Check PostgreSQL
#
PGSQL_SUPPORT="no"
AC_ARG_WITH([pgsql],
    [AS_HELP_STRING([--without-pgsql],
        [Disable PostgreSQL support in motion.])],
    [PGSQL="$withval"],
    [PGSQL="yes"])

AC_ARG_WITH([pgsql-lib],
    [AS_HELP_STRING([--with-pgsql-lib=<path>],
        [Normally, configure will scan all possible default
         installation paths for pgsql libs. When it fails, use
         this command to tell configure where pgsql libs
         installation root directory is.])],
    [PGSQL_LIBS="$withval"],
    [PGSQL_LIBS="yes"])

AC_ARG_WITH([pgsql-include],
    [AS_HELP_STRING([--with-pgsql-include=<path>],
        [Normally, configure will scan all possible default
         installation paths for pgsql include. When it fails, use
         this command to tell configure where pgsql include
         installation root directory is.])],
    [PGSQL_HEADERS="$withval"],
    [PGSQL_HEADERS="yes"])

if test "$PGSQL" != "no"; then
    # ******* Search pgsql headers *******
    LOOK_AT="/usr /usr/local /usr/local/pgsql $PHP_PGSQL"
    if test "$PGSQL_HEADERS" != "yes"; then
        LOOK_AT="$PGSQL_HEADERS"
    fi
    PGSQL_HEADERS="no"
    for dir in $LOOK_AT; do
        # Manual detection for <withval>
        AC_CHECK_FILE([$dir/libpq-fe.h], [
            PGSQL_HEADERS="yes"
            PGSQL_DIR="$(dirname $dir)"
            PGSQL_INCDIR="$dir"
            break
        ], [])
        # AUTODETECT libpq-fe.h
        AC_CHECK_FILE([$dir/include/libpq-fe.h], [
            PGSQL_HEADERS="yes"
            PGSQL_DIR="$dir"
            PGSQL_INCDIR="$dir/include"
            break
        ], [])
        # AUTODETECT pgsql/libpq-fe.h
        AC_CHECK_FILE([$dir/include/pgsql/libpq-fe.h], [
            PGSQL_HEADERS="yes"
            PGSQL_DIR="$dir"
            PGSQL_INCDIR="$dir/include/pgsql"
            break
        ], [])
        # AUTODETECT postgresql/libpq-fe.h
        AC_CHECK_FILE([$dir/include/postgresql/libpq-fe.h], [
            PGSQL_HEADERS="yes"
            PGSQL_DIR="$dir"
            PGSQL_INCDIR="$dir/include/postgresql"
            break
        ], [])
    done
    if test "$PGSQL_HEADERS" = "yes"; then
        # ******* Search pgsql libs *********
        LOOK_AT="/usr/lib64 /usr/lib /usr/local/lib $PGSQL_DIR/lib/pgsql $PGSQL_DIR/lib/postgresql $PGSQL_DIR/lib"
        if test "$PGSQL_LIBS" != "yes"; then
            LOOK_AT="$PGSQL_LIBS"
        fi
        for dir in $LOOK_AT; do
            # AUTODETECT STATIC LIB
            AC_CHECK_FILE([$dir/libpq.a], [
                PGSQL_SUPPORT="yes"
                PGSQL_LIBDIR="$dir"
                break
            ], [])
            # AUTODETECT SHARED LIB
            AC_CHECK_FILE([$dir/libpq.so], [
                PGSQL_SUPPORT="yes"
                PGSQL_LIBDIR="$dir"
                break
            ], [])
        done
        AC_MSG_CHECKING([for pgsql libs])
        if test "$PGSQL_SUPPORT" = "yes"; then
            AC_MSG_RESULT([$PGSQL_LIBDIR])
            PGSQL_INCLUDE="-I$PGSQL_INCDIR"
            saved_LIBS=$LIBS
            saved_CFLAGS=$CFLAGS
            LIBS="-L$PGSQL_LIBDIR -lpq"
            CFLAGS="-I$PGSQL_INCDIR"
            AC_CHECK_LIB([pq], [PQconnectStart], [
                TEMP_LIBS="$TEMP_LIBS -L$PGSQL_LIBDIR -lpq"
                TEMP_CFLAGS="$TEMP_CFLAGS -I$PGSQL_INCDIR"
                AC_DEFINE([HAVE_PGSQL], 1, [Define to 1 if you have PostgreSQL support.])],
                [AC_MSG_ERROR([PostgreSQL support can't build without PostgreSQL libraries])],[])
            LIBS=$saved_LIBS
            CFLAGS=$saved_CFLAGS
        else
            AC_MSG_RESULT([no])
        fi
    # end pgsql-include, pgsql-libs
    fi
# end PostgreSQL detection
fi

AC_ARG_WITH([developer-flags],
    [AS_HELP_STRING([--with-developer-flags],
        [Causes practically all of the possible gcc
         warning flags to be set. This may produce
         a large amount of warnings.])],
    [DEVELOPER_FLAGS="$withval"],
    [DEVELOPER_FLAGS="no"])

if test "$DEVELOPER_FLAGS" = "yes"; then
    TEMP_CFLAGS="$TEMP_CFLAGS -W -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Waggregate-return -Wstrict-prototypes -Wmissing-prototypes -Wnested-externs -Winline -Wredundant-decls -Wno-long-long -ggdb -g3"
fi

AC_MSG_CHECKING([for bswap instruction])
AC_LINK_IFELSE([
    AC_LANG_PROGRAM([ ],
        [unsigned int __x=0;
        register unsigned int __v;
        __asm("bswap %0" : "=r" (__v) : "0" (__x));])
    ], [
        AC_MSG_RESULT([yes])
        AC_DEFINE([HAVE_BSWAP], 1, [Define to 1 if you have 'bswap' instruction.])
    ], [
        AC_MSG_RESULT([no])
    ])

CFLAGS="$TEMP_CFLAGS $UNAME_DEFS"
LIBS="$TEMP_LIBS -lm -lpthread"
LDFLAGS="$TEMP_LDFLAGS"

AC_CHECK_FUNC([avformat_alloc_context],
    AC_DEFINE([have_avformat_alloc_context], 1,
        [Define to 1 if you 'have avformat_alloc_context' support.]))
AC_CHECK_FUNC([av_avformat_alloc_context],
    AC_DEFINE([have_av_avformat_alloc_context], 1,
        [Define to 1 if you have 'av_avformat_alloc_context' support.]))
AC_CHECK_FUNC([av_register_protocol2],
    AC_DEFINE([have_av_register_protocol2], 1,
        [Define to 1 if you have 'av_register_protocol2' support.]))
AC_CHECK_FUNC([av_register_protocol],
    AC_DEFINE([have_av_register_protocol], 1,
        [Define to 1 if you have 'av_register_protocol' support.]))

#
# Add the right exec path for rc scripts
#
if test "x$prefix" = "xNONE"; then
    BIN_PATH="$ac_default_prefix"
    if test $exec_prefix = "NONE"; then
        BIN_PATH="$BIN_PATH/bin"
    else
        BIN_PATH="$BIN_PATH/$bindir"
    fi
else
    if test "x$exec_prefix" = "xNONE"; then
        BIN_PATH="$prefix/bin"
    else
        BIN_PATH="$prefix/$bindir"
    fi
fi
AC_SUBST(BIN_PATH)

AC_CONFIG_FILES([
thread1.conf
thread2.conf
thread3.conf
thread4.conf
motion-dist.conf
motion.init-Debian
motion.init-Fedora
motion.spec
Makefile
])
AC_OUTPUT

echo
echo "*******************************************************"
echo "* Configure status"
echo "* $PACKAGE_NAME $PACKAGE_VERSION"
echo "*******************************************************"
echo
echo "V4L support:         $V4L"
echo "V4L2 support:        $V4L2"
echo "SDL support:         $SDL"
if test "$JPEG_TURBO_SUPPORT" = "yes"; then
    echo "jpeg support:        turbo"
elif test "$JPEG_MMX_SUPPORT" = "yes"; then
    echo "jpeg support:        mmx"
else
    echo "jpeg support:        $JPEG_SUPPORT"
fi
echo "FFmpeg support:      $FFMPEG_SUPPORT"
echo "SQLite3 support:     $SQLITE3_SUPPORT"
echo "MYSQL support:       $MYSQL_SUPPORT"
echo "PostgreSQL support:  $PGSQL_SUPPORT"
echo
echo "CFLAGS:  $CFLAGS"
echo "LIBS:    $LIBS"
echo "LDFLAGS: $LDFLAGS"
echo
echo "Install prefix: $prefix"
echo
