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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_dbfiller.h"
#include <mysql.h>
#include <time.h>
#include <errno.h>
/* If you declare any globals in php_dbfiller.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(dbfiller)
*/
#define PHP_MY_RES_NAME "mysql连接文件描述符"
#define PHP_MY_RESULT_NAME "mysql结果集合文件描述符"
/* True global resources - no need for thread safety here */
static int le_dbfiller;
static int le_sample_descriptor,le_result_des;
zend_class_entry * dbfiller_ce;

typedef struct _php_mysql_conn {
	MYSQL conn;
	int active_result_id;
} php_mysql_conn;

PHP_METHOD(dbfiller, connect) {
        php_mysql_conn *mysql;
        zval *host, *user, *pwd, *port;
        mysql = (php_mysql_conn *) emalloc(sizeof(php_mysql_conn));
	mysql->active_result_id = 0;
        if(mysql_init(&mysql->conn) == NULL){
		php_printf("mysql init error!");
		mysql_close(&mysql->conn);
		RETURN_NULL();
	}
        host  = zend_read_property(dbfiller_ce, getThis(), "host", sizeof("host")-1, 0 TSRMLS_DC);
        user  = zend_read_property(dbfiller_ce, getThis(), "user", sizeof("user")-1, 0 TSRMLS_DC);
        pwd   = zend_read_property(dbfiller_ce, getThis(), "pwd", sizeof("pwd")-1, 0 TSRMLS_DC);
        port  = zend_read_property(dbfiller_ce, getThis(), "port", sizeof("port")-1, 0 TSRMLS_DC);
    

	char *arg = NULL;
	int arg_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		php_printf("Please select a db.\n");
		return;
	}

        if (mysql_real_connect(&mysql->conn,  Z_STRVAL_P(host),Z_STRVAL_P(user), Z_STRVAL_P(pwd),arg,Z_STRVAL_P(port), NULL, 128) == NULL)
	{
		php_printf("Connection fails.\n");
		mysql_close(&mysql->conn);
		return;
	}
        ZEND_REGISTER_RESOURCE(return_value,mysql,le_sample_descriptor);
     
}

PHP_METHOD(dbfiller,build) {
        php_mysql_conn *mysql;
	char sql[] = "show tables";
        MYSQL_RES *mysql_result;
	zval **arg;
        if (zend_get_parameters_ex(1, &arg) == FAILURE) {
                WRONG_PARAM_COUNT;
		return;

	}
        ZEND_FETCH_RESOURCE(mysql,php_mysql_conn*,arg,-1,PHP_MY_RES_NAME,le_sample_descriptor);
        mysql_real_query(&mysql->conn,sql,strlen(sql));
        
        mysql_result = mysql_store_result(&mysql->conn);
        

        int i,j = mysql_num_fields(mysql_result);
        MYSQL_ROW row;
	while((row = mysql_fetch_row(mysql_result))){
		for(i = 0; i < j; i++)       
		printf("%s\t", row[i]); 
		printf("\n");        
	}
}
/*定义构造方法*/
PHP_METHOD(dbfiller,DbFiller)
{
    zval *host, *port ,*user,*pass;
    zend_class_entry *ce;
    ce = Z_OBJCE_P(getThis());
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzzz", &host, &port,&user,&pass) == FAILURE )
    {
        printf("Error\n");
        RETURN_NULL();
    }
    zend_update_property(ce, getThis(), "host", sizeof("host")-1, host TSRMLS_CC);
    zend_update_property(ce, getThis(), "port", sizeof("port")-1, port TSRMLS_CC);
    zend_update_property(ce, getThis(), "user", sizeof("user")-1, user TSRMLS_CC);
    zend_update_property(ce, getThis(), "pwd", sizeof("pwd")-1, pass TSRMLS_CC);
     
}
/*
 * Every user visible function must have an entry in dbfiller_functions[].
 */
const zend_function_entry dbfiller_functions[] = {
	PHP_ME(dbfiller, connect, 	NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(dbfiller, DbFiller, 	NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(dbfiller, build, 	NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
        {NULL, NULL, NULL}
};
/* }}} */

/* {{{ dbfiller_module_entry
 */
zend_module_entry dbfiller_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"dbfiller",
	dbfiller_functions,
	PHP_MINIT(dbfiller),
	PHP_MSHUTDOWN(dbfiller),
	PHP_RINIT(dbfiller),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(dbfiller),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(dbfiller),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_DBFILLER_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_DBFILLER
ZEND_GET_MODULE(dbfiller)
#endif


/*
 {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(dbfiller)
{
        zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "DbFiller", dbfiller_functions);
	dbfiller_ce = zend_register_internal_class_ex(&ce,  NULL, NULL TSRMLS_CC);
	zend_declare_property_null(dbfiller_ce, ZEND_STRL("host"),    ZEND_ACC_PRIVATE TSRMLS_CC);
        zend_declare_property_null(dbfiller_ce, ZEND_STRL("port"),    ZEND_ACC_PRIVATE TSRMLS_CC);
        zend_declare_property_null(dbfiller_ce, ZEND_STRL("user"),    ZEND_ACC_PRIVATE TSRMLS_CC);
        zend_declare_property_null(dbfiller_ce, ZEND_STRL("pwd"),    ZEND_ACC_PRIVATE TSRMLS_CC);
        zend_declare_property_null(dbfiller_ce, ZEND_STRL("conn"),    ZEND_ACC_PRIVATE TSRMLS_CC);
	le_sample_descriptor = zend_register_list_destructors_ex(NULL, NULL, PHP_MY_RES_NAME,module_number);
	le_result_des = zend_register_list_destructors_ex(NULL, NULL, PHP_MY_RESULT_NAME,module_number);
	return SUCCESS;
}

/*
 {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(dbfiller)
{
	return SUCCESS;
}

/*
 {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(dbfiller)
{
	return SUCCESS;
}

/* 
{{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(dbfiller)
{
	return SUCCESS;
}

/* 
{{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(dbfiller)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "dbfiller support", "enabled");
	php_info_print_table_end();

}


/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_dbfiller_compiled(string arg)
   Return a string to confirm that the module is compiled in */
/*PHP_FUNCTION(confirm_dbfiller_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "dbfiller", arg);
	RETURN_STRINGL(strg, len, 0);
}*/
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
