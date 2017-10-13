dnl $Id$
dnl config.m4 for extension dbfiller

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(dbfiller, for dbfiller support,
dnl Make sure that the comment is aligned:
dnl [  --with-dbfiller             Include dbfiller support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(dbfiller, whether to enable dbfiller support,
Make sure that the comment is aligned:
[  --enable-dbfiller           Enable dbfiller support])

if test "$PHP_DBFILLER" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-dbfiller -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/dbfiller.h"  # you most likely want to change this
  dnl if test -r $PHP_DBFILLER/$SEARCH_FOR; then # path given as parameter
  dnl   DBFILLER_DIR=$PHP_DBFILLER
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for dbfiller files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       DBFILLER_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$DBFILLER_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the dbfiller distribution])
  dnl fi

  dnl # --with-dbfiller -> add include path
  dnl PHP_ADD_INCLUDE($DBFILLER_DIR/include)

  dnl # --with-dbfiller -> check for lib and symbol presence
  dnl LIBNAME=dbfiller # you may want to change this
  dnl LIBSYMBOL=dbfiller # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $DBFILLER_DIR/lib, DBFILLER_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_DBFILLERLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong dbfiller lib version or lib not found])
  dnl ],[
  dnl   -L$DBFILLER_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(DBFILLER_SHARED_LIBADD)

  PHP_NEW_EXTENSION(dbfiller, dbfiller.c, $ext_shared)
fi
