#include <SPI.h>

#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
//#include <util.h>

#include <DHT.h>
#include <SoftwareSerial.h>

byte MAC[]={0x90,0xA2,0xDA,0x0D,0xCA,0xCC}; 
byte IPADDR[]={169,254,251,109};
byte GATEWAY[]={192,168,1,1};
byte SUBNET[]={255,255,0,0};
byte PHPSVR[]={169,254,251,107};  //replace with your web server ip address
EthernetClient client;

DHT dht;
SoftwareSerial s(7,8);  //rx, tx
unsigned int sensor, mq;
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup(){
  Serial.begin(9600);
  s.begin(38400);
  dht.setup(2);
  Ethernet.begin(MAC,IPADDR,GATEWAY,SUBNET);
}

void contactWebServer(float temp, float hum, unsigned int ppm){
  Serial.println("Sedang menyambungkan ke server web untuk mengirim data sensor...");
  if(client.connect(PHPSVR, 85)){
    if(ppm>500){
      ppm=500;
    }
    Serial.println("Tersambung di server php");
    client.print("GET /tugasakhir/ethernet.php?ppm=");client.print(ppm);client.print("&temp=");client.print(temp);client.print("&hum=");client.print(hum);client.println(" HTTP/1.1");
    client.println("Host: 169.254.251.107:85");
    client.println("Connection: close");
    client.println();
    client.stop();
  }
  else{
    Serial.println("Gagal menyambungkan ke server web");
  }
}

unsigned int bacaNilaiMQ135(void){
  s.write(0x41);     //perintah baca nilai sensor (dt-sense mq135)
  delayMicroseconds(10);
  sensor = s.read();
  sensor=sensor*256 + s.read();
  return sensor;
}

void loop(){
  delay(5); 
  float suhu=dht.getTemperature();
  float lembab=dht.getHumidity(); 
  
  delay(dht.getMinimumSamplingPeriod());
  Serial.print(lembab);Serial.print("%");
  Serial.print("\t");
//  Serial.print(suhu*1.8+32);Serial.print("* F");  
//  Serial.print("\t");
  Serial.print(suhu);Serial.print("* C");  
//  Serial.print("\t");
//  Serial.print(suhu+ 273.15 );Serial.println("* K"); 
  Serial.print("\n");
  
  sensor = bacaNilaiMQ135();
  
  Serial.print(sensor/1000 % 10 + 0x30);Serial.print("\t");
  Serial.print(sensor/100 % 10 + 0x30);Serial.print("\t");
  Serial.print(sensor/10 % 10 + 0x30);Serial.print("\t");
  Serial.print(sensor % 10 + 0x30);Serial.print("\t");
  Serial.println(sensor);Serial.print("\n");
   
  mq=sensor;
  contactWebServer(suhu, lembab, mq);
  
//  delay(1000); 
}


