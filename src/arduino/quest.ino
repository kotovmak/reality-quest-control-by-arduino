#include <SPI.h>
#include <Ethernet.h>


byte mac[] = {
  0xDE, 0xAD, 0xBE, 0x88, 0xF6, 0xE5
};
// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(192, 168, 0, 22);
IPAddress gateway(192, 168, 0, 1);

IPAddress subnet(255, 255, 0, 0);
// fill in your Domain Name Server address here:
IPAddress myDns(1, 1, 1, 1);
// initialize the library instance:
EthernetClient clients[4];
EthernetServer server(12321);

char ardsend[20];
//char compsend[20];
char inchar =-1;
//int index = 0;


/*-------------------VARS----------------------*/
uint8_t result;
String LastStrLan;
#define S_SIZE 18
boolean s[S_SIZE];
boolean start, finish_win, finish_lose, reset = false;
boolean sended_start, sended_finish,p08 = false;
boolean reseted = true;
#define ST_SIZE 15
boolean step[ST_SIZE];
#define UST_SIZE 6
boolean u_step[UST_SIZE];
boolean u_end = false;
#define PL_SIZE 10
boolean play[PL_SIZE];
int pin_reset = A0;
int pin_power = A1;
int pin_work = A2;
int pin_error = A3;
#define P_SIZE 18
int p[P_SIZE] = {A8, A9, A10, A11, A12, A13, A14, A15, 40, 42, 44, 46, 38,39, A5, 5, 4, 3};
#define R_SIZE 16
int r[R_SIZE] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37};
#define T_SIZE 18
unsigned long timer[T_SIZE],rInt;
int ii;
boolean go;
/*------------------------------------------------*/

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  Ethernet.begin(mac, ip, myDns, gateway, subnet);
  server.begin();
  //Serial.println("ip serv: 192.168.0.5");
  //Serial.println("port server:   12320");
  Serial.print("ip ard:   ");
  Serial.println(Ethernet.localIP());
  Serial.println("port ard:      12321");

  pinMode(pin_power,INPUT_PULLUP);
  pinMode(pin_reset,INPUT_PULLUP);

  for (int i=0; i<P_SIZE;i++) {
    if (i>13) {
    pinMode(p[i], INPUT);
      digitalWrite(p[i], LOW);
    } else {
    pinMode(p[i], INPUT_PULLUP);
    digitalWrite(p[i], HIGH);
    }
   }
  digitalWrite(pin_power, HIGH);
  digitalWrite(pin_reset, HIGH);

  for (int i=0; i<R_SIZE;i++) {
    pinMode(r[i], OUTPUT);
  }
  pinMode(pin_error, OUTPUT);
  pinMode(pin_work, OUTPUT);

  InitReset();

  for (int i=0; i<P_SIZE;i++) {
    //Serial.print(String("p"+String(i)+" "));
    //Serial.println(digitalRead(p[i]));
  }
  Serial.println("ready");
}

void InitReset()
{
  ii=0;
  rInt = 0;
  p08 = false;
  go = false;

  for (int i=0; i<R_SIZE;i++) {
    if (i==6)
    digitalWrite(r[i], LOW);
    else
    digitalWrite(r[i], HIGH);
  }

  digitalWrite(pin_work, LOW);

  for (int i=0; i<S_SIZE;i++) {
    s[i] = false;
  }

  start = finish_win = finish_lose = false;
  sended_start = sended_finish = false;

  for (int i=0; i<ST_SIZE;i++) {
    step[i] = false;
  }
  for (int i=0; i<T_SIZE;i++) {
    timer[i] = 0;
  }
  for (int i=0; i<UST_SIZE;i++) {
    u_step[i] = false;
  }
  for (int i=0; i<PL_SIZE;i++) {
    play[i] = false;
  }
  for (int i=0; i<T_SIZE;i++) {
    timer[i] = 0;
  }
  reseted = true;
}

void loop() {

  EthernetClient client = server.available();
  if (ChangeStatePin(pin_power, !start))
    start =  !GetStatePin(pin_power, start);

  if (ChangeStatePin(pin_reset, !reset))
    reset = !GetStatePin(pin_reset, reset);

  if (client) {
    boolean newClient = true;
    for (byte i = 0; i < 4; i++) {
      //check whether this client refers to the same socket as one of the existing instances:
      if (clients[i] == client) {
        newClient = false;
        break;
      }
    }

    if (newClient) {
      //check which of the existing clients can be overridden:
      for (byte i = 0; i < 4; i++) {
        if (!clients[i] && clients[i] != client) {
          clients[i] = client;
          // clear out the input buffer:
          client.flush();
          Serial.println("We have a new client");
          client.print("Hello, client number: ");
          client.print(i);
          client.println();
          break;
        }
      }
    }

    if (client.available() > 0) {
      // read the bytes incoming from the client:
      String ardsend;
      while (client.available() > 0) {
        char thisChar = client.read();
        ardsend += thisChar;
      }
      if (ardsend.indexOf("ping")!=-1) {
        client.println("pong");
        Serial.print(ardsend);
      } else if (ardsend.length()>0) {
        GetLanVar(ardsend);
        Serial.print(ardsend);
      }
    }
  }
  for (byte i = 0; i < 4; i++) {
    if (!(clients[i].connected())) {
      // client.stop() invalidates the internal socket-descriptor, so next use of == will allways return false;
      clients[i].stop();
    }
  }


  if (reset && !reseted) {
    Serial1.println("R");
    InitReset();
  }

  if (timer[0])
  if (millis() - timer[0] > 500) {
    digitalWrite(pin_work, !digitalRead(pin_work));
    timer[0] = millis();
  }

  for (int i=0; i<P_SIZE;i++) {
    if (i>13) {
      if (ChangeStatePin(p[i], s[i])) {
        if (!timer[6]) s[i] =  GetStatePin(p[i], s[i]);
        if (i==16 || i==15) timer[6]=millis();
         Serial.println(p[i]);
        Serial.println(s[i]);
        delay(50);


      }
    } else {
      if (ChangeStatePin(p[i], !s[i])) {
        s[i] =  !GetStatePin(p[i], s[i]);
        Serial.println(p[i]);
        Serial.println(s[i]);
        delay(50);
      }
    }
    if (timer[6]) if (millis() - timer[6] > 2000) timer[6]=false;
  }

  if (start && reseted) {
     reseted = false;
     u_end = false;
     sended_finish = false;
     step[0] = true;
     //play[0] = true;
     timer[0] = millis();
     timer[3] = millis();
     rInt = random(360000,720000);
     Serial1.println("S");
     digitalWrite(r[4], LOW);
     digitalWrite(r[7], LOW);
    digitalWrite(r[2], LOW);
  }

 if (u_step[0] && digitalRead(r[9]) == LOW) {
    digitalWrite(r[9], HIGH);
    u_step[0] = false;
  }
  if (u_step[0] || (s[0] && step[0] && !step[1]))
  {
       step[1] = true;
       u_step[0] = false;
       goplay("P01.wma");
       digitalWrite(r[9], LOW);
  }

  if (u_step[1] && digitalRead(r[10]) == LOW) {
    digitalWrite(r[10], HIGH);
    u_step[1] = false;
  }
  if (u_step[1] || (s[1] && step[1] && !step[2]))
  {
       step[2] = true;
       u_step[1] = false;
       timer[8] = millis();
       goplay("P02.wma");
  }
  if (timer[8]) if (millis() - timer[8] > 500) {
     digitalWrite(r[10], LOW);
     digitalWrite(r[9], HIGH);
      timer[8]=false;
  }

   if (u_step[2] && digitalRead(r[1]) == LOW) {
    digitalWrite(r[1], HIGH);
    u_step[2] = false;
  }
  if (u_step[2] || (s[2] && s[3] && !s[4] && !s[5] && !s[6] && step[2] && !step[3]))
  {
    digitalWrite(r[1], LOW);
    //Serial.println("s3");
    step[3] = true;
    u_step[2] = false;
    digitalWrite(r[11], HIGH);
    digitalWrite(r[12], HIGH);
    digitalWrite(r[13], HIGH);
    digitalWrite(r[14], HIGH);
    digitalWrite(r[15], LOW);
  }
  if (!step[3]) {
    if (s[2]) digitalWrite(r[11], LOW); else digitalWrite(r[11], HIGH);
    if (s[3]) digitalWrite(r[12], LOW); else digitalWrite(r[12], HIGH);
    if (s[4]) digitalWrite(r[13], LOW); else digitalWrite(r[13], HIGH);
    if (s[5]) digitalWrite(r[14], LOW); else digitalWrite(r[14], HIGH);
    if (s[6]) digitalWrite(r[15], LOW); else digitalWrite(r[15], HIGH);
  }
 if (u_step[3] && digitalRead(r[3]) == LOW) {
    digitalWrite(r[3], HIGH);
    u_step[3] = false;
  }
  if (u_step[3] || (s[8] && step[3] && !step[4]))
  {
    step[4] = true;
    u_step[3] = false;
    goplay("P10.wma");
    digitalWrite(r[3], LOW);
  }

  if (u_step[4] && digitalRead(r[2]) == LOW) {
    //digitalWrite(r[2], HIGH);
    digitalWrite(r[6], LOW);
    u_step[4] = false;
  }
  if (u_step[4] || (!s[9] && step[4] && !step[5]))
  {
    step[5] = true;
    u_step[4] = false;
    timer[1] = millis();
    goplay("P05.wmv");

    digitalWrite(r[7], HIGH); //room light off
  }
  if (timer[1])
  if (millis() - timer[1] > 7500) { //7,5 second
    digitalWrite(r[6], HIGH); //projector off
    digitalWrite(r[7], LOW); //light on
    timer[1] = 0;
  }

  if (u_step[5] && digitalRead(r[0]) == LOW) {
    digitalWrite(r[0], HIGH);
    digitalWrite(r[5], HIGH);
    u_step[5] = false;
  }
  if (u_step[5] || (!s[10] && step[5] && !step[6]))
  {
    step[6] = true;
    u_step[5] = false;
    timer[4] = millis();
    goplay("P06.wma");
    digitalWrite(r[7], HIGH);
    digitalWrite(r[5], LOW);
    digitalWrite(r[0], LOW);
  }
  if (timer[4])
  if (millis() - timer[4] > 12000) {
    digitalWrite(r[5], HIGH);
    digitalWrite(r[0], HIGH);
    timer[5] = millis();
    timer[4] = 0;
  }
  if (timer[5])
  if (millis() - timer[5] > 3000) {
     digitalWrite(r[7], LOW); // room light on
    timer[5] = 0;
  }

 if (s[7] && step[6] && !step[12])
  {
    step[12] = true;
    goplay("P14.wma");
    finish_win = true;
    digitalWrite(r[8], LOW); // light on
  }
  if (!s[12] && step[3] && !step[7])
  {
    step[7] = true;
    goplay("P07.wma");
  }

  if (!s[11] && step[2] && !step[11])
  {
    step[11] = true;
    goplay("P15.wma");
  }

  // s[14] - XD-RF-5V radio buton 1

  if (s[14] && !go){
      if (!timer[9]) {
        timer[9] = millis();
        goplay("P08.wma");
      }
      s[14] = false;
  }

  if (timer[9]) if (millis() - timer[9] > 500) {
    timer[9]=false;
    go=true;
  }

  if ((s[14] || (ii%2!=0 && ii>0)) && digitalRead(r[4]) == HIGH) {
      digitalWrite(r[4], LOW);
      s[14] = false;
      ii++;
      timer[7] = millis();
    }
  if ((s[14]) && digitalRead(r[4]) == LOW) {
    if (!p08) {
      p08=true;
    }
     digitalWrite(r[4], HIGH);
     s[14] = false;
     ii++;
  }
  if (timer[7])
  if (millis() - timer[7] > 2000) {
    timer[7] = false;
    ii=0;
  }

  // s[15] - XD-RF-5V radio buton 2
  if ((s[15]) && !timer[2]) {
    s[15] = false;
    timer[2] = millis();
    if (!step[7]) goplay("P09.wma");
    if (step[7] && !step[5]) goplay("P10.wma");
    if (step[5]) goplay("P13.wma");
  }
  if (timer[2])
  if (millis() - timer[2] > 2000) {
    timer[2] = 0;
  }

  // s[16]  - XD-RF-5V radio buton 3
  // r[7] - room light channel 3
  // r[8] - light on exit
  if ((s[16]) && digitalRead(r[7]) == LOW) {
    digitalWrite(r[7], HIGH);
//    digitalWrite(r[8], HIGH);
    digitalWrite(r[15], HIGH);
    s[16] = false;
  }
  if ((s[16]) && digitalRead(r[7]) == HIGH) {
    digitalWrite(r[7], LOW);
    digitalWrite(r[15], LOW);
    s[16] = false;
  }

  if (timer[3])
  if (millis() - timer[3] > rInt) {
    if (timer[1]) timer[3] = millis();
    else {
      goplay("P11.wmv");
    digitalWrite(r[2], HIGH);
      delay(100);
    digitalWrite(r[2], LOW);
      timer[3] = millis();
      rInt = random(360000,720000);
    }
  }

  // s[17] - XD-RF-5V radio buton 4
  if (s[17])
  {
    Serial1.println("Q");
    s[17] = false;
  }

  if (u_end)
  {
    u_end = false;
    s[17] = false;
    goplay("P12.wma");
    finish_lose = true;
    Serial1.println("R");
  }

  for (int i=0; i<PL_SIZE;i++) {
    if (play[i])
    {
      String track = "track"+String(i)+".mp3";
      play[i] = false;
      delay(500);
    }
  }

  SetDatLan();
}

boolean ChangeStatePin(int pin, boolean lastState)
{
  if (digitalRead(pin) == !lastState) return true;
   else return false;
}

boolean GetStatePin(int pin, boolean lastState)
{
  if (digitalRead(pin) == !lastState)
   {
    return (digitalRead(pin));
   }
}

void GetLanVar(String StrVar) {
    if (StrVar[0] == 'S') start = true;
    if (StrVar[0] == 'R') reset = true;
    for (int i=0; i<UST_SIZE; i++) {
      if (StrVar[i] == 'E') u_step[i] = true;
                         else u_step[i] = false;
    }
    if (StrVar[7] == 'E') {
           u_end = true;
    } else u_end = false;
}

void SetDatLan()
{
  String StrSetLan;
  if (start && !sended_start) {
      StrSetLan += "T";
      sended_start = true;
      start=false;
  }   else StrSetLan += "F";
  for (int i=0; i<R_SIZE; i++) {
      if (digitalRead(r[i])==LOW) {
        StrSetLan += "T";
      } else {
        StrSetLan += "F";
      }
  }
  if ((finish_win || reset || finish_lose) && !sended_finish) {
      sended_finish = true;
      finish_win = false;
      finish_lose = false;
      reset = false;
      StrSetLan += "T";
  }   else StrSetLan += "F";
  if (LastStrLan.length() == 0 || LastStrLan != StrSetLan)
  {
    for (byte i = 0; i < 4; i++) {
      if (clients[i]) {
         clients[i].println(StrSetLan);
      }
    }
    LastStrLan = StrSetLan;
  }
}

void goplay(String file) {
    for (byte i = 0; i < 4; i++) {
      if (clients[i]) {
         clients[i].println(file);
      }
    }
}
