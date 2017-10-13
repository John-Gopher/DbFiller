/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author:                                                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */
#include <mysql.h>
#ifndef PHP_DBFILLER_H
#define PHP_DBFILLER_H

extern zend_module_entry dbfiller_module_entry;
#define phpext_dbfiller_ptr &dbfiller_module_entry

#define PHP_DBFILLER_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_DBFILLER_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_DBFILLER_API __attribute__ ((visibility("default")))
#else
#	define PHP_DBFILLER_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(dbfiller);
PHP_MSHUTDOWN_FUNCTION(dbfiller);
PHP_RINIT_FUNCTION(dbfiller);
PHP_RSHUTDOWN_FUNCTION(dbfiller);
PHP_MINFO_FUNCTION(dbfiller);

//PHP_FUNCTION(confirm_dbfiller_compiled);	/* For testing, remove later. */

/* 
   Declare any global variables you may need between the BEGIN
   and END macros here:     
 */

typedef struct _php_mysql_conn {
    MYSQL conn;
    int active_result_id;
} php_mysql_conn;

typedef struct _kval {
    char *key;
    char *val;
}kval;

typedef struct _type_param_entry{
    char *type;
    int length;
    int min;
    int max;
}type_param_entry;


ZEND_BEGIN_MODULE_GLOBALS(dbfiller)
    type_param_entry *tpa_p[20];
    int tpa_len;
    char beforesql[1024];
    char aftersql[1024*1023];
ZEND_END_MODULE_GLOBALS(dbfiller)

    /* In every utility function you add that needs to use variables 
       in php_dbfiller_globals, call TSRMLS_FETCH(); after declaring other 
       variables used by that function, or better yet, pass in TSRMLS_CC
       after the last function argument and declare your utility function
       with TSRMLS_DC after the last declared argument.  Always refer to
       the globals in your function as DBFILLER_G(variable).  You are 
       encouraged to rename these macros something shorter, see
       examples in any other php module directory.
     */

#ifdef ZTS
#define DBFILLER_G(v) TSRMG(dbfiller_globals_id, zend_dbfiller_globals *, v)
#else
#define DBFILLER_G(v) (dbfiller_globals.v)
#endif

#endif	/* PHP_DBFILLER_H */


    /*
     * Local variables:
     * tab-width: 4
     * c-basic-offset: 4
     * End:
     * vim600: noet sw=4 ts=4 fdm=marker
     * vim<600: noet sw=4 ts=4
     */
