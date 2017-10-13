// Copyright (C) 
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

/// @filename      dbfiller.c
/// @Synopsis      
/// @author LornenLei, nangfq@foxmail.com
/// @version 1.03
/// @date 2017-04-09

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_dbfiller.h"
#include <mysql.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
/* 
If you declare any globals in php_dbfiller.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(dbfiller)
 */
#define PHP_MY_RES_NAME "mysql连接文件描述符"
#define PHP_MY_RESULT_NAME "mysql结果集合文件描述符"
/* True global resources - no need for thread safety here */
static int le_dbfiller;
static int le_sample_descriptor,le_result_des;
zend_class_entry * dbfiller_ce;
int dbfiller_globals_id;
zend_dbfiller_globals dbfiller_globals;
kval* php_next_val(HashTable *arrht);
// ============================================================================
/// @Synopsis      连接数据库
///
/// @Param         dbfiller
/// @Param         connect
// ============================================================================
PHP_METHOD(dbfiller, connect) {
    php_mysql_conn *mysql;
    zval *host, *user, *pwd, *port,*value;
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
    MAKE_STD_ZVAL(value);
    ZEND_REGISTER_RESOURCE(value,mysql,le_sample_descriptor);
    zend_update_property(dbfiller_ce, getThis(), "conn", sizeof("conn")-1,value TSRMLS_CC);

}


// ============================================================================
/// @Synopsis  产生随机字符串
///
/// @Param int  n  字符串长度
///
/// return  char*
// ============================================================================
char *getRandomStr(int n){
    int i;
    time_t t; 
    /* 输出 0 到 93 之间的 n 个随机数 */
    time_t seconds;
    char *str = (char*)emalloc(n+1);
    for( i = 0 ; i < n ; i++ ) {  
        int randval = rand()+ time(NULL); 
        randval = randval<0 ? -randval : randval;
        str[i] =  randval% 94+33;
    }   
    str[n] = '\0';
    return str;
}


// ============================================================================
/// @Synopsis      生成随机数字，取值范围为min~max
///
/// @Param         min
/// @Param         max
///
/// \return 
// ============================================================================
char *getRandomInt(int min,int max){
    srand((unsigned)time(NULL));
    int  i = rand();
    char *str= (char*)emalloc(25);
    //    itoa(i,str,10);
    sprintf(str,"%d",i);
    return str;
}
/// @Synopsis      从二级数组解析出字段类型和数据生成策略
///
/// @Param         arrht
/// @Param         key
/// @Param         keylen
// ============================================================================
void php_foreach_hash_inner(HashTable *arrht,char *key,uint keylen)
{

    zend_hash_internal_pointer_reset(arrht);
    if(zend_hash_has_more_elements(arrht) == SUCCESS)
    {       
        char *key;
        uint keylen;
        ulong idx;
        int type;
        zval **ppzval, tmpcopy;

        type = zend_hash_get_current_key_ex(arrht, &key, &keylen,&idx, 0, NULL);
        if (zend_hash_get_current_data(arrht, (void**)&ppzval) == FAILURE)
        {
            return;
        }

        tmpcopy = **ppzval;
        zval_copy_ctor(&tmpcopy);
        INIT_PZVAL(&tmpcopy);

        if (type != HASH_KEY_IS_STRING)
        {
            php_printf("二级数组键必须是字符串：%d\n",type);
        }
        PHPWRITE(key, keylen);

        zend_uchar vtype = Z_TYPE_P(&tmpcopy); 
        if(vtype == IS_ARRAY || vtype == IS_OBJECT || vtype == IS_RESOURCE)
        {	php_printf("二级数组值必须是字符串或者数字");
            return;
        }

        convert_to_string(&tmpcopy);
        char * tmpchar = Z_STRVAL(tmpcopy); 
        PHPWRITE(tmpchar, Z_STRLEN(tmpcopy));
        if(strcmp(key,"type") == 0){
            int length = 20;
            php_printf("sizeof:",sizeof(type_param_entry));
            type_param_entry *tp = (type_param_entry *)emalloc(sizeof(type_param_entry));
            if(strcmp(tmpchar,"string") == 0){
                kval *next = (kval*)php_next_val(arrht);
                if(next != NULL && strcmp(next->key,"length")){
                    length = atoi(next->val);
                }      
                tp->type = tmpchar;
                tp->length = length;
            }else if(strcmp(tmpchar,"int") == 0){
                kval *min_p = (kval*)php_next_val(arrht);
                kval *max_p = (kval*)php_next_val(arrht);
                tp->type =  tmpchar;
                tp->min  = atoi(min_p->val);
                tp->max  = atoi(max_p->val);
            }else{
                php_printf("目前仅支持string和int类型");
                return;
            }
            DBFILLER_G(tpa_p)[DBFILLER_G(tpa_len)++] = tp;

        }else{
            php_printf("%s的第一个参数必须是type",key);
            return;
        }

        zval_dtor(&tmpcopy);
    }
}
kval* php_next_val(HashTable *arrht)
{
    zend_hash_move_forward(arrht);
    if(zend_hash_has_more_elements(arrht) == SUCCESS)
    { 

        char *key;
        uint keylen;
        ulong idx;
        int type;
        zval **ppzval, tmpcopy;
        kval *kvp;
        type = zend_hash_get_current_key_ex(arrht, &key, &keylen,&idx, 0, NULL);
        if (zend_hash_get_current_data(arrht, (void**)&ppzval) == FAILURE)
        {
            return NULL;
        }

        tmpcopy = **ppzval;
        zval_copy_ctor(&tmpcopy);
        INIT_PZVAL(&tmpcopy);

        if (type != HASH_KEY_IS_STRING)
        {
            php_printf("数组键必须是字符串：%d\n",type);
            return NULL;
        }

        zend_uchar vtype = Z_TYPE_P(&tmpcopy); 
        if(vtype == IS_ARRAY || vtype == IS_OBJECT || vtype == IS_RESOURCE)
        {	php_printf("值必须是字符串或者数字");
            return NULL;
        }

        convert_to_string(&tmpcopy);
        char *tmpchar = Z_STRVAL(tmpcopy); 
        kvp = (kval *)emalloc(sizeof(kval));
        kvp->key = key;
        kvp->val = tmpchar;
        zval_dtor(&tmpcopy);
        return kvp;
    }else{
        return NULL;
    }
}
/*
 *遍历数组
 */
void php_foreach_hash(HashTable *arrht)
{

    int flag = 0;
    strcat(DBFILLER_G(beforesql) , "(");
    for(zend_hash_internal_pointer_reset(arrht);zend_hash_has_more_elements(arrht) == SUCCESS;zend_hash_move_forward(arrht))
    {
        char *key;
        uint keylen;
        ulong idx;
        int type;
        zval **ppzval, tmpcopy;

        type = zend_hash_get_current_key_ex(arrht, &key, &keylen,&idx, 0, NULL);
        if (zend_hash_get_current_data(arrht, (void**)&ppzval) == FAILURE)
        {
            continue;
        }

        tmpcopy = **ppzval;
        zval_copy_ctor(&tmpcopy);
        INIT_PZVAL(&tmpcopy);

        if (type == HASH_KEY_IS_STRING)
        {
            PHPWRITE(key, keylen);
        } else {
            php_printf("%ld", idx);

        }
        if(Z_TYPE_P(&tmpcopy) != IS_ARRAY){
            php_printf("参数有误");
            return;	
        }
        char *field;
        if(flag ==0)
        {
            field = (char *)emalloc(sizeof(key));
            sprintf(field,"%s",key);
        }
        else
        {

            field = (char *)emalloc(sizeof(key)+1);
            sprintf(field,",%s",key);
        }
        strcat(DBFILLER_G(beforesql),field);
        php_foreach_hash_inner(tmpcopy.value.ht,key,keylen);
        if(flag == 0)
        {
            flag = 1;
        }
        zval_dtor(&tmpcopy);
    }
    strcat(DBFILLER_G(beforesql) , ")");
}

/*
 *构造sql语句
 */
PHP_METHOD(dbfiller,build) {
    zval *arr;
    char *table;
    int table_len;
    HashTable *arrht;
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"as",&arr,&table,&table_len) == FAILURE){
        php_printf("Please input a array and a table name\n");
        RETURN_NULL();
    }
    strcat( DBFILLER_G(beforesql),table);
    arrht = arr->value.ht;
    php_foreach_hash(arrht);
}


PHP_METHOD(dbfiller,run) {
    int num = 1;
    if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"l",&num) == FAILURE){
        php_printf("Default only once!\n");

    }
    int i, j;
    for(i=0;i<num;i++){
        int len = DBFILLER_G(tpa_len);
        for(j=0;j<len;j++){
            type_param_entry *tp = DBFILLER_G(tpa_p)[j];
            char *insert_val;
            if(strcmp(tp->type,"string") == 0){
                insert_val = getRandomStr(tp->length ); 
            }else if(strcmp(tp->type,"int") == 0){
                insert_val = getRandomInt(tp->min,tp->max);
            }else{
                php_printf("目前仅支持string和int类型");
                return;
            }
            char *tmpsplit = (char*)emalloc(strlen(insert_val)+5);
            if(j==0){
                sprintf(tmpsplit,"('%s'",insert_val);
            }else if(j == (DBFILLER_G(tpa_len) -1)){
                if(i != (num-1)){
                    sprintf(tmpsplit,",'%s'),",insert_val);
                }else{
                    sprintf(tmpsplit,",'%s')",insert_val);
                }
            }else{
                sprintf(tmpsplit,",'%s'",insert_val);
            }
            strcat(DBFILLER_G(aftersql),tmpsplit);
        }
    }
    php_printf("sql:%s%s\n",DBFILLER_G(beforesql),DBFILLER_G(aftersql));
    RETURN_NULL();
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
        PHP_ME(dbfiller, run, 	NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
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
    PHP_RINIT(dbfiller),		
    PHP_RSHUTDOWN(dbfiller),
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
static void php_dbfiller_globals_ctor(zend_dbfiller_globals *dbfiller_globals TSRMLS_DC)
{


    dbfiller_globals->tpa_len = 0;
    memcpy(dbfiller_globals->beforesql,"INSERT INTO ",12);
    memcpy(dbfiller_globals->aftersql,"VALUES ",7);
}

static void php_dbfiller_globals_dtor(zend_dbfiller_globals *dbfiller_globals TSRMLS_DC)
{
}

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

#ifdef ZTS
    ts_allocate_id(&dbfiller_globals_id,
            sizeof(zend_dbfiller_globals),
            (ts_allocate_ctor)php_dbfiller_globals_ctor,
            (ts_allocate_dtor)php_dbfiller_globals_dtor);
#else
    php_dbfiller_globals_ctor(&dbfiller_globals TSRMLS_CC);
#endif
    return SUCCESS;
}

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


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
