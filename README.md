# DbFiller——批量数据构造器
# For example:
```

$db = new DbFiller('127.0.0.1',3306,'root','root');

//select table
$conn = $db->connect('hichat');

$fds = array('iUserId'=>array('type'=>'int','min'=>10,'max'=>1000),'sNickName'=>array('type'=>'string','length'=>10));

//build sql
$db->build($fds,'tbUser');

//增加10条数据
$db->run(10);
```
