<?php

//STEP1 CONNECT THE DB
//mysql_connect(servername,username,password);
// servername	可选。规定要连接的服务器。默认是 "localhost:3306"。
// username	可选。规定登录所使用的用户名。默认值是拥有服务器进程的用户的名称。
// password	可选。规定登录所用的密码。默认是 ""。


// 用户名　 : SAE_MYSQL_USER
// 密　　码 : SAE_MYSQL_PASS
// 主库域名 : SAE_MYSQL_HOST_M
// 从库域名 : SAE_MYSQL_HOST_S
// 端　　口 : SAE_MYSQL_PORT
// 数据库名 : SAE_MYSQL_DB

$dbstr = SAE_MYSQL_HOST_M.":".SAE_MYSQL_PORT ;

$con = mysql_connect($dbstr,   SAE_MYSQL_USER,    SAE_MYSQL_PASS);
if (!$con)
  {
  die('Could not connect: ' . mysql_error());
  }

mysql_select_db(SAE_MYSQL_DB);//選擇數據庫
  
$temp = $_GET['t'];//GET方法為URL參數傳遞
//$cf   = $_GET['cf'];
$now  = date("Y-m-d h:i:s");

mysql_query("INSERT INTO cxp_temprature (temp, datetime) VALUES ('$temp', '$now') ");

echo "Update successfully!";
?>
