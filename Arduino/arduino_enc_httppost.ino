

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
char website[]              PROGMEM = "2.ruiba2012.sinaapp.com";
char urlBuf[]               PROGMEM = "/cxpupdate.php?";

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
  Serial.begin(115200);
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
}