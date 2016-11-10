/// **** INCLUDES **** ///
#include <Si4703_Breakout.h>
#include <Wire.h>
#include <Nextion.h>

/// **** CONSTANTES **** ////
static const uint16_t  FAIL = 0;
static const uint16_t  SUCCESS = 1;
static const int  SI4703 = 0x10; //
static const uint16_t  I2C_FAIL_MAX = 10; 
static const uint16_t  SEEK_DOWN = 0; 
static const uint16_t  SEEK_UP = 1;
//Define the register names
static const uint16_t  DEVICEID = 0x00;
static const uint16_t  CHIPID = 0x01;
static const uint16_t  POWERCFG = 0x02;
static const uint16_t  CHANNEL = 0x03;
static const uint16_t  SYSCONFIG1 = 0x04;            ///  Constantes définissants les registres ///
static const uint16_t  SYSCONFIG2 = 0x05;
static const uint16_t  STATUSRSSI = 0x0A;
static const uint16_t  READCHAN = 0x0B;
static const uint16_t  RDSA = 0x0C;
static const uint16_t  RDSB = 0x0D;
static const uint16_t  RDSC = 0x0E;
static const uint16_t  RDSD = 0x0F;
//Register 0x02 - POWERCFG
static const uint16_t  SMUTE = 15;
static const uint16_t  DMUTE = 14;
static const uint16_t  MONO = 13;
static const uint16_t  SKMODE = 10;
static const uint16_t  SEEKUP = 9;
static const uint16_t  SEEK = 8;
//Register 0x03 - CHANNEL
static const uint16_t  TUNE = 15;
//Register 0x04 - SYSCONFIG1
static const uint16_t  RDS = 12;
static const uint16_t  DE = 11;
//Register 0x05 - SYSCONFIG2
static const uint16_t  SPACE1 = 5;
static const uint16_t  SPACE0 = 4;
//Register 0x0A - STATUSRSSI
static const uint16_t  RDSR = 15;
static const uint16_t  STC = 14;
static const uint16_t  SFBL = 13;
static const uint16_t  AFCRL = 12;
static const uint16_t  RDSS = 11;
static const uint16_t  STEREO = 8;

/// **** VARIABLES **** ///

//uint16_t//
uint16_t si4703_registers[16];

//int//
int resetPin = 2;
int SDIO = A4;
int SCLK = A5;
int Region = 0;
int channel;
int volume = 7;
int completedCount = 0;
int rdsCount = 0;

//String//
String lect;
String data = "";
String seeku = "101180255255255";
String seekd = "101160255255255";
String vol = "101190255255255";
String one = "101130255255255";   //1033 Fun radio nancy
String two = "101140255255255";   // 887 France culture
String three = "101150255255255"; // 917 France musique
String mute = "1011100255255255"; 
String search = "101170255255255";
String on = "1011110255255255";
String valid = "1022255255255";
String buffActuelle = "";

//Si4703_Breakout ( radio )//
Si4703_Breakout radio(resetPin, SDIO, SCLK, Region);

//bool//
bool isMute = false;
bool isOn = false;
bool data_receive = false;
bool completed[] = {false, false, false, false};

//Elements Nextion//
NexText t0 = NexText(1, 1, "t0");
NexText g0 = NexText(1, 12, "g0");
NexSlider h0 = NexSlider(1, 9, "h0");

//char//
char text[8];
char *rds;
char rdsBuffer[10];

void setup()
{
  Serial.begin(9600);
  radio.powerOn();
  radio.setVolume(0);
  
}

void loop(){
  
  if (Serial.available()){
    data_receive = true;
    while ( Serial.available() ){     // Si il y a des données à lire, je les lis.//
      lect = String(Serial.read());
      data = data + lect;
      delay(2); 
    }
  }  
  if (data_receive){                 
    
    if (data == seeku ){
      channel = radio.seekUp();
      toBuffer( channel, text );
      t0.setText(text);
      push();
      data = "";
    }
  
    else if ( data == on ){          // Si j'ai lu des données, je regarde quelle bouton à été appuyé
      if ( isOn ){                   // Search , SeekUp, SeekDown ...
        radio.setVolume(0);
        t0.setText(" OFF ");
        isOn = false;
        data="";
      }
      else{
        radio.setVolume(volume);
        radio.setChannel(1033);
        t0.setText("103.3");
        isOn = true;
        data="";
      }
    }
   
    else if ( data == seekd ){
      channel = radio.seekDown();
      toBuffer( channel, text);
      t0.setText(text);
      push();
      data = "";
      
    } 
         
    else if ( data == vol ){
      uint32_t number = 0;
      h0.getValue(&number);
      radio.setVolume(number);
      volume = number;
      data="";
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
      t0.setText(text);
      push();
      data = "";
      
    }
    
    else if ( data == two ){
      channel = 887; // France Culture 
      radio.setChannel(channel);
      toBuffer( channel, text);
      t0.setText(text);
      push();
      data = "";
      
    }
    
    else if ( data == three ){
      channel = 917; // France Musique 
      radio.setChannel(channel);
      toBuffer( channel, text);
      t0.setText(text);
      push();
      data = "";
      
    }
  
    else if ( data == search ){
      data="";
      char buffert[100] = {0};
      while ( Serial.available() < 1 ){
      }
      delay(20);
      while ( Serial.available() ){
        data+=Serial.read();
      }
      t0.getText(buffert , sizeof(buffert));
      channel = int(10*String(buffert).toFloat());
      radio.setChannel(channel);
      push();
      data="";    
    }
  data_receive = false;
  }


  
  if( completedCount < 4 ) { //&& isRds == false
    rdsCount ++;
    if(rdsCount > 20000){
      memset(completed , false , sizeof(completed));             // Je lis le RDS
      completedCount = 0;
      rdsCount = 0;
      memset(rdsBuffer, 0 , sizeof(rdsBuffer));
      
    }
    readRegisters();
    if(si4703_registers[STATUSRSSI] & (1<<RDSR)){

      uint16_t b = si4703_registers[RDSB];
      int index = b & 0x03;
      byte type_trame=(si4703_registers[RDSB] & 0xF000) >> 12;
      if (! completed[index] && type_trame==0x00)
      {
        completed[index] = true;
        completedCount ++;
        char Dh = (si4703_registers[RDSD] & 0xFF00) >> 8;
        char Dl = (si4703_registers[RDSD] & 0x00FF);
        rdsBuffer[index * 2] = Dh;
        rdsBuffer[index * 2 +1] = Dl;
      }
      delay(40); //Wait for the RDS bit to clear
    }
    else {
      delay(30); //From AN230, using the polling method 40ms should be sufficient amount of time between checks
    }
  }
  if (completedCount == 4){
 //   isRds = true; 
    rdsBuffer[10]='/0';                                 //Si le RDS est complet et différent du précédent, je l'écrit.
    if (String(rdsBuffer) != buffActuelle){
      g0.setText(rdsBuffer);
      buffActuelle = String(rdsBuffer);
    }
    memset(completed , false , sizeof(completed));
    memset(rdsBuffer, 0 , sizeof(rdsBuffer));
    completedCount = 0;
  }
}


void toBuffer(int chan, char tex[8]){
      String chanstr = String(chan);
      int taille = sizeof(chanstr);       //Fonction de mise en forme de donées
      chanstr.toCharArray(tex,8);         // int --> buffer
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

boolean readRegisters(){

  //Si4703 begins reading from register upper register of 0x0A and reads to 0x0F, then loops to 0x00.
  Wire.requestFrom(SI4703, 32); //We want to read the entire register set from 0x0A to 0x09 = 32 bytes.

  long endTime = millis() + 1000;  
  while(Wire.available() < 32 && millis() < endTime) ; //Wait for 16 words/32 bytes to come back from slave I2C device

  //Remember, register 0x0A comes in first so we have to shuffle the array around a bit
  for(int x = 0x0A ; ; x++) { //Read in these 32 bytes
    if(x == 0x10) x = 0; //Loop back to zero
    si4703_registers[x] = Wire.read() << 8;
    si4703_registers[x] |= Wire.read();
    if(x == 0x09) break; //We're done!
  }
  if (millis() >= endTime) {
  return 0;
  }
  return 1;
}

void push(){
  memset(completed , false , sizeof(completed));
  completedCount = 0;
  memset(rdsBuffer, 0 , sizeof(rdsBuffer));
  g0.setText(" - - - ");
}

