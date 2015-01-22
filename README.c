# Arduino-ENC28J60-Web-MySQL
Arduino-ENC28J60-MySQL-through-WEBbrowse


[size=150][b]第一部分 网站建立[/b][/size]

首先我们需要一套个人的网站系统，包括数据库与主机
[list]1一套可以正常运行的数据库系统， 此例使用mySQL
2.一个HTTP的服务器，此例使用PHP程序[/list]
以上两套系统可以使用phpstudy来架localhost，也可以使用新浪有SAE来架免费的网站博客之类的。

在某个目录下添加一个php文件，此文件通过网页调用后可以更新数据库数据。
[list]1. 新建根目录下文件名为  ./fpdz_update.php
2. 访问URL为http://localhost/fpdz_update.php?temp=23.5  [/list]
目的是通过这个URL可以更新数据库

在数据库内我们建表 TEMP，有两个变量，温度值，及相应时间
[list]1.temp
2.datetime[/list]

在文件fpdz_update.php中内容为： 【注意配置数据库的连接参数】
[code]<?php

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

mysql_select_db(SAE_MYSQL_DB);//选择数据库
  
$temp = $_GET['t'];//简单的GET方法处理温度参数
//$cf   = $_GET['cf'];
$now  = date("Y-m-d h:i:s");

mysql_query("INSERT INTO TEMP (temp, datetime) VALUES ('$temp', '$now') ");

echo "Update successfully!";
?>
[/code]

这样我们就可以通过HTTP访问URL来更新数据库里的温度值。如
[code]http://localhost/fpdz_update.php?temp=24.55[/code]

[size=150][b]第二部分 数据上传[/b][/size]
1.Arduino板 使用UNO为例
2.网络板使用 本店的ENC28J60
3.Arduino的库使用 ethercard  [url]https://github.com/jcw/ethercard[/url]
4.温度传感器使用18B20,调用18B20的库文件
5.调用ethercard.browseUrl函数进行数据更新。

6.串口输出可得
[code]>>  
Read temp:t=25.25C
>>> REQ
<<< Reply 2252 ms
HTTP/1.0 200 OK
Server: nginx/1.4.4
Date: Tue, 13 Jan 2015 14:16:04 GMT
Content-Type: text/html
X-Powered-By: PHP/5.3.29
Set-Cookie: saeut=143.89.188.4.1421158564648235; path=/; max-age=311040000
X-Cache: MISS from ustlnx17.ust.hk
X-Cache-Lookup: MISS from ustlnx17.ust.hk:8080
Via: 10.67.15.48, 1.0 ustlnx17.ust.hk (squid/3.1.20-20120626-r10456)
Connection: close
Update successfully!
[/code]

源代码ino如下：

[code]

#include "DS18B20_S.h"
//DS18B20(byte pin)  连接管脚
//传感器设定为10位模式，每次转换时间<187.5ms,如果需要12位模式，请修改库文件of ds.set(0x7F);
DS18B20_S  ds(5);//pin5
float read18B20temp() 
{ 
  ds.start();
  //开始测量（所有传感器）
  /////////判断转换是否完成
  ///只有在三根线连接模式下有效,两根线模式下请不要使用
  //  while(!ds.ready()){
  //    delay(10);
  //  }  
  ////////////
  delay(200);//2根线连接模式
  
  float a=ds.get();

  if(a>200)
  { //CRC 校验错误
  return 0;
  }
  else
  {  
    return a;
  }
  
  //下面的不重要
  //   void set(byte n);//set(0x1F) 9位模式 delay(94) ;
                             //0x3F 10位模式 delay(188);
                             //0x5F 11位模式 delay(375);
                             //0x7F 12 位模式 delay(750);
}

char tempstr[]="t=12.34";

float lasttemp=12.34;
void getTempStr()
{
  float temp = read18B20temp();

  if(temp == 0) 
    temp = lasttemp;
  else
    lasttemp = temp;


  uint16_t Tc_100 = temp*100;
  uint8_t whole, fract;
  whole = Tc_100/100 ;  // separate off the whole and fractional portions
  fract = Tc_100 % 100;

  sprintf(tempstr,"t=%d.%d",whole,fract); 
}


#include <EtherCard.h>
 
#define REQUEST_RATE 60000 // milliseconds
// ethernet interface mac address
static byte mymac[] ={ 0x00,0x1E,0x37,0xD9,0x62,0x58}; //hkust 


 // remote website name
char website[]              PROGMEM = "localhost";
char urlBuf[]               PROGMEM = "/fpdz_update.php?";

byte Ethernet::buffer[500];   // a very small tcp/ip buffer is enough here
static long timer;
 
// called when the client request is complete
static void my_result_cb (byte status, word off, word len)
{
  Serial.print("<<< Reply ");
  Serial.print(millis() - timer);
  Serial.println(" ms");
  Serial.println((const char*) Ethernet::buffer + off);
}
 
void setup () 
{
  Serial.begin(57600);
  Serial.println(F("\n[http-post]"));

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println(F("Failed to access Ethernet controller"));

  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));
 

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  

  if (!ether.dnsLookup(website))
    Serial.println("DNS failed");
    
  ether.printIp("SRV: ", ether.hisip);
 
  timer = - REQUEST_RATE; // start timing out right away


}
 

void loop () 
{
  ether.packetLoop(ether.packetReceive());
 
  if (millis() > timer + REQUEST_RATE)
  {
    timer = millis();
    Serial.print("\n>>>  Read temp:");
    getTempStr();
    Serial.print(tempstr);
    Serial.println("C");

    Serial.println(">>> REQ");

    if (!ether.dnsLookup(website))
      Serial.println("DNS failed");
  
    ether.browseUrl( urlBuf, tempstr, website, my_result_cb);
  }
}[/code]


[size=150][b]第三部分 网页显示[/b][/size]

本部分是就是网页站将数据读出，整理成图表
待完


[size=150][b]后记[/b][/size]
可以直接读写数据库。回头研究更新
