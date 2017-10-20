# DbFiller——批量数据构造器

 
# 如何安装？
 
## 编译
```
make clean

#/usr/local/php/bin/phpize

#./configure --with-php-config=/usr/local/php/bin/php-config
```
## 编辑Makefile文件：

大概在30几行，修改INCLUDES，添加“-L/usr/local/mysql/lib -I/usr/local/mysql/include”

```
EXTRA_LIBS =
INCLUDES = -I/usr/local/php/include/php -L/usr/local/mysql/lib -I/usr/local/mysql/include -lmysqlclient  -I/usr/local/php/include/php/main -I/usr/local/php/include/php/TSRM -I/usr/local/php/include/php/Zend -I/usr/local/php/include/php/ext -I/usr/local/php/include/php/ext/date/lib
LFLAGS =
LDFLAGS =
```
 
## 开始安装
```
make && make install

```

# 如何使用？:
```

$db = new DbFiller('127.0.0.1',3306,'root','root');

//选择数据库
$conn = $db->connect('hichat');

$fds = array('iUserId'=>array('type'=>'int','min'=>10,'max'=>1000),'sNickName'=>array('type'=>'string','length'=>10));

//传入字段描述数组和数据表名，sql语句预编译
$db->build($fds,'tbUser');

//增加10条数据
$db->run(10);
```
