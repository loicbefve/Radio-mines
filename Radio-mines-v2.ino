
#include <Si4703_Breakout.h>
#include <Wire.h>
#include <Nextion.h>

int resetPin = 2;
int SDIO = A4;
int SCLK = A5;
int Region = 0;
Si4703_Breakout radio(resetPin, SDIO, SCLK, Region);
int channel = 887;
int volume = 7;
char rdsBuffer[10];

//Page 4
String data = "";
String seeku = "101410255255255";
String seekd = "101490255255255";
String volup = "101440255255255";
String voldown = "101430255255255";
String one = "1014110255255255"; //1033 Fun radio nancy
String two = "1014120255255255"; // 887 France culture
String three = "1014130255255255";// 917 France musique
String four = "1014140255255255"; // 1055 Europe1
String five = "1014150255255255"; //1009 Skyrock
String mute = "101450255255255"; 
String search = "101420255255255";


//Page 1
String valid = "1011160255255255";

char butext[5];
String butextstr;
bool isMute = false;

NexText t1 = NexText(4, 11, "t1");
NexText t2 = NexText(4, 16, "g0");
NexText t3 = NexText(1, 13, "t1");
char text[8];
char *rds;

void setup()
{
  Serial.begin(9600);
  radio.powerOn();
  radio.setVolume(7);
}

void loop(){
  
    radio.readPS(rdsBuffer);
    t2.setText(rdsBuffer); 
  
  if (data == seeku ){
    channel = radio.seekUp();
    toBuffer( channel, text );
    t1.setText(text);

    data = "";
  }
 
  else if ( data == seekd ){
    channel = radio.seekDown();
    toBuffer( channel, text);
    t1.setText(text);
    data = "";
    
  } 
    
  else if ( data == volup ){
    volume ++;
    if (volume == 16) volume = 15;
    radio.setVolume(volume);
    data = "";
    
  }
   
  else if ( data == voldown ){
    volume --;
    if (volume < 0) volume = 0;
    radio.setVolume(volume);
    data = "";
    
  }
    
  else if  ( data == mute ){
    if (!isMute){
      radio.setVolume(0);
      isMute = !isMute;
      data = "";
      
    }
    else {
      radio.setVolume(volume);
      isMute = !isMute;
      data = "";
      
    }
  }
  
  else if ( data == one ){
    channel = 1033; // Fun radio 
    radio.setChannel(channel);
    toBuffer( channel, text);
    t1.setText(text);
    data = "";
    
  }
  
  else if ( data == two ){
    channel = 887; // France Culture 
    radio.setChannel(channel);
    toBuffer( channel, text);
    t1.setText(text);
    data = "";
    
  }
  
  else if ( data == three ){
    channel = 917; // France Musique 
    radio.setChannel(channel);
    toBuffer( channel, text);
    t1.setText(text);
    data = "";
    
  }
  
  else if ( data == four ){
    channel = 1055; // Europe1 
    radio.setChannel(channel);
    toBuffer( channel, text);
    t1.setText(text);
    data = "";
    
  }
  
  else if ( data == five ){
    channel = 1009; // Skyrock 
    radio.setChannel(channel);
    toBuffer( channel, text);
    t1.setText(text);
    data = "";
    
  }

  else if ( data == search ){
    while ( Serial.available() == 0 ){ 
    }
    while ( Serial.available() ){
        Serial.read();
    }
    butextstr = t3.getText(butext , 5);
    channel = butextstr.toFloat();
    radio.setChannel(channel);    
    data = "";
  }

  else {
    data = "";
  }
  
}




//    else if (ch == 'a')
//    {
//      channel = 930; // Rock FM
//      radio.setChannel(channel);
//      displayInfo();
//    }
//    else if (ch == 'b')
//    {
//      channel = 974; // BBC R4
//      radio.setChannel(channel);
//      displayInfo();
//    }
//    else if (ch == 'r')
//    {
//      Serial.println("RDS listening");
//      radio.readRDS(rdsBuffer, 15000);
//      Serial.print("RDS heard:");
//      Serial.println(rdsBuffer);      
//    }
//  }
//}
//
//void displayInfo()
//{
//   Serial.print("Channel:"); Serial.print(channel); 
//   Serial.print(" Volume:"); Serial.println(volume); 
//}

void toBuffer(int chan, char tex[8]){
      String chanstr = String(chan);
      int taille = sizeof(chanstr);
      chanstr.toCharArray(tex,8);
      if ( chan < 1000 ){
        tex[3]=tex[2];
        tex[2]='.';
        tex[4]='\0';
      }
      else {
        tex[4]=tex[3];
        tex[3]='.';
        tex[5]='\0';
      }
}



