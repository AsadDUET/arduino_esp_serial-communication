//The program is used for only Specific number of GSM Based automation System …
//===final code FOR SMS SENDER NUMBER IDENTIFY
#include<EEPROM.h>
#include<SoftwareSerial.h>
SoftwareSerial myserial(12, 11);
#include<LiquidCrystal.h>
const int rs = 37, en = 35, d4 = 33, d5 = 31, d6 = 29, d7 = 27;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 53
#define RST_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN);
#include <Servo.h>


#include <ArduinoJson.h>
DynamicJsonDocument doc(256);
DynamicJsonDocument snd(256);


Servo myservo;
int pos = 0;
void laser();
void card();
void member();
int out = 10;
#define out_pin_low digitalRead(laser1)==HIGH
#define out_pin_hi digitalRead(laser2)==HIGH

int system_fault = A13, GREEN_1 = A12, RED_1 = A10, GREEN_2 = A15, RED_2 = A14, buzzer = A8;
int button1 = A9, button2 = A11;
int laser1 = 39, laser2 = 41;
int temp_sensor = A7, flame_sensor = A6, gas_sensor = A5; ///////Sensor INPUT
int fan_1 = 26, fan_2 = 28, fan_3 = 30, pump = 32;//(relay pin(32 to 36)/////Relay OUTPUT(30,36,26,34,32,28)
void relay_check();

int e1 = 2, n1 = 3, n2 = 4, n3 = 7, n4 = 6, e2 = 5;
/////////////////////////////////////////
void Conveyar_forward(int second_pwm);////
void Conveyar_stop(int second_pwm);///////
///////////////////////////////////////////motor driver for gate & conveyar
void door_forward(int first_pwm);///////
void door_backward(int first_pwm);//////
void door_stop(int first_pwm);//////////
/////////////////////////////////////////
void door_open();
void door_closed();

int count = 0; int adc_value; float v_in; float temp; float total_t; int temptr;
int flame_level = 0;
int value = 0, sensorThres = 120;

int valve1_condition = 0;
char RcvdMsg[200] = "";
int RcvdCheck = 0;
int RcvdConf = 0;
int index = 0;
int RcvdEnd = 0;
char MsgMob[15];
char MsgTxt[55] = "";
int MsgLength = 0;
String number = "";
String mess = "";
char num;
char msg;
String g_msg_temperature = "Temperature: ";
//#define machine_on digitalWrite(convear_motor,LOW)//LOW
//#define machine_off digitalWrite(convear_motor,HIGH)//HIGH

#define machine_on Conveyar_forward(120)
#define machine_off Conveyar_stop(0)

void read_temp_sensor();
void read_flame_sensor();
void read_gas_sensor();
void display_temp_data();
void display_flame_data();
void display_gas_data();
void send_msg(String num, String msg);
void send_msg1(String num, String msg);
void call(String num);
void Response();
void RecSMS();
void machine_condi();

void Config() // This function is configuring our SIM900 module i.e. sending the initial AT commands
{
  delay(1000);
  myserial.print("ATE0\r");
  Response();
  myserial.print("AT\r");
  Response();
  myserial.print("AT+CMGF=1\r");
  Response();
  myserial.print("AT+CNMI=1,2,0,0,0\r");
  Response();
}

int m, f;
int product_count = 0;
int store;
espSerial = Serial2;
void setup()
{
  espSerial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  lcd.begin(20, 4);
  delay(100);
  lcd.setCursor(0, 0);
  lcd.print("GSM Checking...");
  myserial.begin(9600);
  Serial.begin(9600);
  Config();
  ///////////////////////////OUTPUT/////////////////////////////////
  pinMode(fan_1, OUTPUT); // relay _ 1
  pinMode(fan_2, OUTPUT); // relay _ 2
  pinMode(fan_3, OUTPUT); // relay _ 3
  pinMode(pump, OUTPUT); // relay _ 4

  pinMode(e1, OUTPUT); //as output for 293D
  pinMode(n1, OUTPUT);
  pinMode(n2, OUTPUT);
  pinMode(n3, OUTPUT);
  pinMode(n4, OUTPUT);
  pinMode(e2, OUTPUT);
  ///////////////////////////pannel board////////////////////////////
  pinMode(GREEN_1, OUTPUT);                     //pannel board _ 1
  pinMode(RED_1, OUTPUT);                       //pannel board _ 2
  pinMode(GREEN_2, OUTPUT);                     //pannel board _ 3
  pinMode(RED_2, OUTPUT);                       //pannel board _ 4
  pinMode(buzzer, OUTPUT);                      //pannel board _ 5
  ///////////////////////////INPUT///////////////////////////////////
  pinMode(temp_sensor, INPUT);
  pinMode(flame_sensor, INPUT);
  pinMode(gas_sensor, INPUT);
  pinMode(system_fault, INPUT);                //pannel board _ 6
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(laser1, INPUT);
  pinMode(laser2, INPUT);
  pinMode(out, INPUT_PULLUP);

  digitalWrite(fan_1, HIGH);
  digitalWrite(fan_2, HIGH);
  digitalWrite(fan_3, HIGH);
  digitalWrite(pump, HIGH);

  lcd.setCursor(0, 0);
  lcd.print("LED & Alarm Checking");
  digitalWrite(GREEN_1, HIGH);
  digitalWrite(GREEN_2, HIGH);
  digitalWrite(RED_1, HIGH);
  digitalWrite(RED_2, HIGH);
  digitalWrite(buzzer, HIGH);
  delay(500);
  digitalWrite(GREEN_1, LOW);
  digitalWrite(GREEN_2, LOW);
  digitalWrite(RED_1, LOW);
  digitalWrite(RED_2, LOW);
  digitalWrite(buzzer, LOW);
  delay(100);
  lcd.clear();

  //   lcd.setCursor(0,0);
  //   lcd.print("Relay Checking......");
  //   relay_check();
  //   delay(500);

  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.begin(20, 4);
  delay(1000);
  myservo.attach(6);
  myservo.write(pos);

  read_temp_sensor(); read_flame_sensor(); read_gas_sensor();
  delay(100);
  lcd.clear();
  machine_condi();
  //call("01813644633");
  //send_msg("01868901459","System Ready");
}

void loop()
{
  if (espSerial.available())
  {
    deserializeJson(doc, espSerial);
    String mm = doc["m"];
    if (mm == "1")
      m = 1;
    if (mm == "2")
      m = 2;
    if (mm == "0")
      m = 0;
    String ff = doc["f"];
    if (ff == "0")
      f = 0;
    if (ff == "1")
      f = 1;
    if (ff == "2")
      f = 2;
    if (ff == "3")
      f = 3;
    if (ff == "4")
      f = 4;


  }
  //        if(Serial.available())******************************************serial
  //        {
  //
  //        }

  //RecSMS();
  read_temp_sensor(); read_flame_sensor(); read_gas_sensor();
  display_temp_data(); display_flame_data(); display_gas_data();
  laser();
  card();

  if (digitalRead(system_fault) == LOW) //MANUAL ON SWITCH
  {
    if (digitalRead(button1) == LOW)
    {
      m = 0;
      machine_on;
      digitalWrite(GREEN_1, HIGH);
      EEPROM.write(0, 1);
      machine_condi();
    }

    if (digitalRead(button2) == LOW)
    {
      m = 0;
      machine_off;
      digitalWrite(GREEN_1, LOW);
      EEPROM.write(0, 0);
      machine_condi();
    }
    // RecSMS();
  }



  if (m != 0) ////***************************************firebase(MOTOR)
  {
    if (m = 1)
    {
      machine_on;
      digitalWrite(GREEN_1, HIGH);
      EEPROM.write(0, 1);
      machine_condi();
    }
    if (m = 2)
    {
      machine_off;
      digitalWrite(GREEN_1, LOW);
      EEPROM.write(0, 0);
      machine_condi();
    }
    RecSMS();
  }

  lcd.setCursor(10, 3);
  lcd.print("Member:");
  lcd.setCursor(17, 3);
  lcd.print("  ");
  lcd.setCursor(17, 3);
  lcd.print(store);
  Serial.print("Member: ");
  Serial.print(store);/////*******************************member counting
  Serial.print("   ");
  snd["type"] = "ok";
  snd["person"] = store;

  lcd.setCursor(0, 0);
  lcd.print("Temperature: ");
  lcd.print(temptr);
  lcd.print("'c");
  Serial.print("temp: ");
  Serial.print(temptr);/////************************************temperature
  Serial.print("   ");
  snd["temp"] = temptr;

  Serial.print("flame level: ");
  Serial.print(flame_level);/////************************************flame
  Serial.print("   ");
  snd["flame"] = flame_level;

  Serial.print("gas value: ");
  Serial.print(value);/////*********************************************gas
  Serial.print("   ");
  snd["gas"] = value;

  lcd.setCursor(0, 3);
  lcd.print("P.C: ");
  lcd.print(product_count);
  Serial.print("product_count: ");
  Serial.println(product_count);/////***************************product counting
  Serial.print("   ");
  snd["prod"] = product_count;
  serializeJson(snd, espSerial);
}



//===========================================//
////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////ALL SUB-FUNCTION///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void read_temp_sensor()
{
  int i;
  total_t = 0;
  for (i = 1; i <= 500; i++)
  {
    adc_value = analogRead(temp_sensor);
    delay(1);
    v_in = (adc_value * 5.0) / 1023.0;
    temp = v_in / 0.01;
    total_t = total_t + temp;
  }
  temptr = total_t / 500;
  //lcd.clear();
}
void display_temp_data()
{
  if (f != 0) ////***************************************firebase(Fan)
  {
    if (f = 1)
    {
      digitalWrite(fan_1, HIGH); //HIGH
      digitalWrite(fan_2, HIGH); //HIGH
      digitalWrite(fan_3, HIGH); //HIGH
      digitalWrite(pump, HIGH); //HIGH
      digitalWrite(GREEN_2, HIGH);
      digitalWrite(buzzer, LOW);
      digitalWrite(RED_2, LOW);
    }
    else if (f = 2)
    {
      digitalWrite(fan_1, LOW); //LOW
      digitalWrite(fan_2, HIGH); //HIGH
      digitalWrite(fan_3, HIGH); //HIGH
      digitalWrite(pump, HIGH); //HIGH
      digitalWrite(GREEN_2, HIGH);
      digitalWrite(buzzer, LOW);
      digitalWrite(RED_2, LOW);
    }
    else if (f = 3)
    {
      digitalWrite(fan_1, LOW); //LOW
      digitalWrite(fan_2, LOW); //LOW
      digitalWrite(fan_3, HIGH); //HIGH
      digitalWrite(pump, HIGH); //HIGH
      digitalWrite(GREEN_2, HIGH);
      digitalWrite(buzzer, LOW);
      digitalWrite(RED_2, LOW);
    }
    else if (f = 4)
    {
      digitalWrite(fan_1, LOW); //LOW
      digitalWrite(fan_2, LOW); //LOW
      digitalWrite(fan_3, LOW); //LOW
      digitalWrite(pump, HIGH); //HIGH
      digitalWrite(RED_2, HIGH);
      digitalWrite(buzzer, LOW);
    }
  }
  else
  {
    if (temptr <= 35)
    {
      digitalWrite(fan_1, HIGH); //HIGH
      digitalWrite(fan_2, HIGH); //HIGH
      digitalWrite(fan_3, HIGH); //HIGH
      digitalWrite(pump, HIGH); //HIGH
      digitalWrite(GREEN_2, HIGH);
      digitalWrite(buzzer, LOW);
      digitalWrite(RED_2, LOW);
    }
    else if (temptr > 35 && temptr <= 45)
    {
      digitalWrite(fan_1, LOW); //LOW
      digitalWrite(fan_2, HIGH); //HIGH
      digitalWrite(fan_3, HIGH); //HIGH
      digitalWrite(pump, HIGH); //HIGH
      digitalWrite(GREEN_2, HIGH);
      digitalWrite(buzzer, LOW);
      digitalWrite(RED_2, LOW);
    }
    else if (temptr > 45 && temptr <= 55)
    {
      digitalWrite(fan_1, LOW); //LOW
      digitalWrite(fan_2, LOW); //LOW
      digitalWrite(fan_3, HIGH); //HIGH
      digitalWrite(pump, HIGH); //HIGH
      digitalWrite(GREEN_2, HIGH);
      digitalWrite(buzzer, LOW);
      digitalWrite(RED_2, LOW);
    }
    else if (temptr > 55 && temptr <= 65)
    {
      digitalWrite(fan_1, LOW); //LOW
      digitalWrite(fan_2, LOW); //LOW
      digitalWrite(fan_3, LOW); //LOW
      digitalWrite(pump, HIGH); //HIGH
      digitalWrite(RED_2, HIGH);
      digitalWrite(buzzer, LOW);
    }
    else
    {
      digitalWrite(pump, LOW); //LOW
      digitalWrite(buzzer, HIGH);
      send_msg("01868901459", "System Over heated & System fail. Take Action.");
      delay(3000);
      while (temptr > 65)
      {
        digitalWrite(fan_1, HIGH); //HIGH
        digitalWrite(fan_2, HIGH); //HIGH
        digitalWrite(fan_3, HIGH); //HIGH
        digitalWrite(buzzer, HIGH);
        digitalWrite(RED_2, HIGH);
      }
    }
  }
}
void read_flame_sensor()
{
  flame_level = analogRead ( flame_sensor ) ;
}

void display_flame_data()
{
  if (flame_level < 600)
  {
    while (1)
    {
      if (flame_level < 600)
      {
        digitalWrite(buzzer, HIGH);
        digitalWrite(RED_2, HIGH);
        lcd.setCursor(0, 1);
        lcd.print("flame detected      ");
        send_msg("01868901459", "System flame detected....");
        delay(2000);
      }
      break;
    }
  }
  else
  {
    lcd.setCursor(0, 1);
    lcd.print("No flame detected   ");
    digitalWrite(GREEN_2, HIGH);
    digitalWrite(buzzer, LOW);
    digitalWrite(RED_2, LOW);
  }
}
void read_gas_sensor()
{
  value = analogRead(gas_sensor);
}
void display_gas_data()
{
  if (value > 35)
  {
    while (1)
    {
      if (value > 35)
      {
        digitalWrite(RED_2, HIGH);
        lcd.setCursor(0, 2);
        lcd.print("Gas Leakage Alert.!!");
        send_msg("01868901459", "System Gas Leakaged....");
        tone(buzzer, 1000, 200);
        delay(100);
      }
      break;
    }
  }
  else
  {
    digitalWrite(GREEN_2, HIGH);
    digitalWrite(RED_2, LOW);
    lcd.setCursor(0, 2);
    lcd.print(">>>>>Gas Normal<<<<<");
    noTone(buzzer);
  }
}
///////////////////////////////////////////AT Command////////////////////////////////////////
void Response() // Get the Response of each AT Command
{
  int count = 0;
  Serial.println();
  while (1)
  {
    if (myserial.available())
    {
      char data = myserial.read();
      if (data == 'K') {
        Serial.println("OK");
        break;
      }
      if (data == 'R') {
        Serial.println("GSM Not Working");
        break;
      }
    }
    count++;
    delay(10);
    if (count == 1000) {
      Serial.println("GSM not Found");
      break;
    }

  }
}
void RecSMS() // Receiving the SMS and extracting the Sender Mobile number & Message Text
{
  if (myserial.available())
  {
    char data = myserial.read();
    if (data == '+') {
      RcvdCheck = 1;
    }
    if ((data == 'C') && (RcvdCheck == 1)) {
      RcvdCheck = 2;
    }
    if ((data == 'M') && (RcvdCheck == 2)) {
      RcvdCheck = 3;
    }
    if ((data == 'T') && (RcvdCheck == 3)) {
      RcvdCheck = 4;
    }
    if (RcvdCheck == 4) {
      RcvdConf = 1;
      RcvdCheck = 0;
    }

    if (RcvdConf == 1)
    {
      if (data == '\n') {
        RcvdEnd++;
      }
      if (RcvdEnd == 3) {
        RcvdEnd = 0;
      }
      RcvdMsg[index] = data;

      index++;
      if (RcvdEnd == 2) {
        RcvdConf = 0;
        MsgLength = index - 2;
        index = 0;
      }
      if (RcvdConf == 0)
      {
        for (int x = 4; x < 18; x++)
        {
          MsgMob[x - 4] = RcvdMsg[x];
          num = RcvdMsg[x];
          number = number + num;
        }
        for (int x = 47; x < MsgLength; x++)
        {
          MsgTxt[x - 46] = RcvdMsg[x];
          msg = RcvdMsg[x];
          mess = mess + msg;
        }
        //****Resetting all the variables////////
        if (number == "+8801868901459")
        {
          if (mess == "TEMPERATURE")
          {
            send_msg1("01868901459", g_msg_temperature);
          }
          if (mess == "MACHINEON")
          {
            while (digitalRead(system_fault) == HIGH)
            { int k;
              while (k > 0)
              {
                send_msg("01868901459", "system_fault");
                k = 10;
                break;
              }
              digitalWrite(RED_1, HIGH);
            }
            digitalWrite(RED_1, LOW);
            //if(digitalRead(button1)==LOW)
            {
              machine_on;
              digitalWrite(GREEN_1, HIGH);
              send_msg("01868901459", "machine_running");
              EEPROM.write(0, 1);
              machine_condi();
            }
          }
          if (mess == "MACHINEOFF")
          {
            while (digitalRead(system_fault) == HIGH)
            { int k;
              while (k > 0)
              {
                send_msg("01868901459", "system_fault");
                k = 10;
                break;
              }
              digitalWrite(RED_1, HIGH);
            }
            digitalWrite(RED_1, LOW);
            //if(digitalRead(button2)==LOW)
            {
              machine_off;
              digitalWrite(GREEN_1, LOW);
              send_msg("01868901459", "machine_stop");
              EEPROM.write(0, 0);
              machine_condi();
            }
          }
        }
        RcvdCheck = 0;
        RcvdConf = 0;
        index = 0;
        RcvdEnd = 0;
        MsgMob[15];
        MsgTxt[55];
        MsgLength = 0;
        number = "";
        mess = "";
        myserial.flush();
      }
    }
  }
}

void send_msg1(String num, String msg)
{
  myserial.print("AT+CMGS=\"");
  myserial.print(num);
  myserial.print("\"");
  myserial.write(13);
  myserial.write(10);//enter
  delay(100);
  myserial.print(msg + String(temptr) + "*");
  myserial.write(26);//ctrl+Z
  Serial.print(msg);
  Serial.println(" ...Message request complete");
  delay(1000);
}

void send_msg(String num, String msg)
{
  myserial.print("AT+CMGS=\"");
  myserial.print(num);
  myserial.print("\"");
  myserial.write(13);
  myserial.write(10);//enter
  delay(100);
  myserial.print(msg);
  myserial.write(26);//ctrl+Z
  Serial.print(msg);
  Serial.println(" ...Message request complete");
  delay(10);
}

void call(String num)
{
  myserial.print("ATD");
  myserial.print(num);
  myserial.print(";");
  myserial.write(13);
  myserial.write(10);

  Serial.print(num);
  Serial.println(" ...Called request complete");
}

void machine_condi()//******************************send firebase data
{
  if (EEPROM.read(0) == 1)
  {
    machine_on;
  }
  else if (EEPROM.read(0) == 0)
  {
    machine_off;
  }
}

void card()
{
  if (digitalRead(out) == LOW)
  {
    delay(50);
    if (digitalRead(out) == LOW)
    {
      digitalWrite(buzzer, HIGH);
      store = store - 1;
      if (store < 1)
        store = 0;
      delay(100);
      digitalWrite(buzzer, LOW);
      door_open();
      door_stop(0);
      delay(2000);
      door_closed();
      door_stop(0);
    }
  }
  //*********************************************************************************************//
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  for (byte i = 0; i < 1; i++)
  {
    ////////////////////////////////// CARD NO - 01 ////////////////////////////////////////////Card UID_1: 18 160 230 26
    if (mfrc522.uid.uidByte[0] == 18 && mfrc522.uid.uidByte[1] == 160 && mfrc522.uid.uidByte[2] == 230 && mfrc522.uid.uidByte[3] == 26)
    {
      digitalWrite(buzzer, HIGH);
      Serial.println("CARD - 1 Pressed,");
      store = store + 1;
      delay(100);
      digitalWrite(buzzer, LOW);
      door_open();
      door_stop(0);
      delay(2000);
      door_closed();
      door_stop(0);
    }
    ///////////////////////////////////// CARD NO - 02 ////////////////////////////////////////////Card UID_2: 22 196 08 240
    if (mfrc522.uid.uidByte[0] == 22 && mfrc522.uid.uidByte[1] == 196 && mfrc522.uid.uidByte[2] == 8 && mfrc522.uid.uidByte[3] == 240)
    {
      digitalWrite(buzzer, HIGH);
      Serial.println("CARD - 2 Pressed,");
      store = store + 1;
      delay(100);
      digitalWrite(buzzer, LOW);
      door_open();
      door_stop(0);
      delay(2000);
      door_closed();
      door_stop(0);
    }
    /////////////////////////////////////// CARD NO - 03 //////////////////////////////////////////Card UID_3: 73 223 04 100
    if (mfrc522.uid.uidByte[0] == 73 && mfrc522.uid.uidByte[1] == 223 && mfrc522.uid.uidByte[2] == 4 && mfrc522.uid.uidByte[3] == 100)
    {
      digitalWrite(buzzer, HIGH);;
      Serial.println("CARD - 3 Pressed,");
      store = store + 1;
      delay(100);
      digitalWrite(buzzer, LOW);
      door_open();
      door_stop(0);
      delay(2000);
      door_closed();
      door_stop(0);
    }
    ///////////////////////////////////// CARD NO - 04 ////////////////////////////////////////////Card UID_4: 98 76 79 115
    if (mfrc522.uid.uidByte[0] == 98 && mfrc522.uid.uidByte[1] == 76 && mfrc522.uid.uidByte[2] == 79 && mfrc522.uid.uidByte[3] == 115)
    {
      digitalWrite(buzzer, HIGH);
      Serial.println("CARD - 4 Pressed,");
      store = store + 1;
      delay(100);
      digitalWrite(buzzer, LOW);
      door_open();
      door_stop(0);
      delay(2000);
      door_closed();
      door_stop(0);
    }
  }
}

void door_open()
{
  Serial.println("Door Opened");
  door_forward(150);
  delay(700);
}
void door_closed()
{
  Serial.println("Door Closed");
  door_backward(150);
  delay(700);
}
void door_forward(int first_pwm)
{
  digitalWrite(n1, LOW);
  digitalWrite(n2, HIGH);
  analogWrite(e1, first_pwm);
}
void door_stop(int first_pwm)
{
  digitalWrite(n1, LOW);
  digitalWrite(n2, LOW);
  analogWrite(e1, first_pwm);
}
void door_backward(int first_pwm)
{
  digitalWrite(n1, HIGH);
  digitalWrite(n2, LOW);
  analogWrite(e1, first_pwm);
}
///////////////////////////////////////////////
void Conveyar_forward(int second_pwm)
{
  digitalWrite(n3, HIGH);
  digitalWrite(n4, LOW);
  analogWrite(e2, second_pwm);
}                              //////////////////////Conveyar///////////////
void Conveyar_stop(int second_pwm)
{
  digitalWrite(n3, LOW);
  digitalWrite(n4, LOW);
  analogWrite(e2, second_pwm);
}
/////////////////////////////////////////////
void relay_check()
{
  digitalWrite(fan_1, LOW); //LOW=HIGH
  delay(2000);
  digitalWrite(fan_1, HIGH);
  digitalWrite(fan_2, LOW);
  delay(2000);
  digitalWrite(fan_2, HIGH);
  digitalWrite(fan_3, LOW);
  delay(2000);
  digitalWrite(fan_3, HIGH);
  digitalWrite(pump, LOW);
  delay(2000);
  digitalWrite(pump, HIGH);
}

void laser()
{
  if (digitalRead(laser1) == HIGH)
  {
    if ((digitalRead(laser2) == HIGH) && (digitalRead(laser1) == HIGH))
    {
      digitalWrite(buzzer, HIGH);
      delay(100);
      product_count = product_count + 1;
      digitalWrite(buzzer, LOW);
      myservo.write(0);
      delay(1);
      while (digitalRead(laser2) == HIGH)
      {
        delay(5);
      }
    }
  }
  else
  {
    myservo.write(0);
    delay(1);
  }
}

// now the code is ended……………
