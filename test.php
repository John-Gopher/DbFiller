<?php 

$db = new DbFiller('127.0.0.1',3306,'root','root');
$conn = $db->connect('hichat');
$fds = array('iUserId'=>array('type'=>'int','min'=>10,'max'=>1000),'sNickName'=>array('type'=>'string','length'=>10));
//$db->build('tbuser',$fds));
//$fds = array('a'=>'1111','b'=>'222222',3=>'hehhh');
$db->build($fds,'tbUser');
//增加10条数据
$db->run(2);
var_dump($conn);

