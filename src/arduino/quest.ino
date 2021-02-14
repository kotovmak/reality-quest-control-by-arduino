#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0x88, 0xF6, 0xE5
};

IPAddress ip(192, 168, 0, 22);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress myDns(8, 8, 8, 8);

EthernetClient clients[4];
EthernetServer server(12321);

/*-------------------VARS----------------------*/

// stored state. If state is changed it will be send to App
String LastStrLan;

// array of bool
// signals from sensors and in game buttons
#define S_SIZE 20
boolean s[S_SIZE];
// array of int
// pins for sensors
#define P_SIZE 20
int p[P_SIZE] = { 5, 4, 3, 2 };

boolean start, finish_win, finish_lose, reset = false;
boolean sended_start, sended_finish = false;
boolean reseted = true;

// array or bool
// number of steps (open door or open lock is a one step)
#define ST_SIZE 15
boolean step[ST_SIZE];

// array of bool
// used when receive `E` from Java control app
#define UST_SIZE 15
boolean u_step[UST_SIZE];
boolean u_end = false;

// array of bool
// used when we need play sound
#define PL_SIZE 10
boolean play[PL_SIZE];

// pin for start and reset quest button
int pin_reset = A0;
int pin_power = A1;

// array of int
// used for connect relays
#define R_SIZE 32
int r[R_SIZE] = {22, 23, 24, 25};

// array of long
// used for in game timers
#define T_SIZE 18
unsigned long timer[T_SIZE];

/*------------------------------------------------*/

void setup() {
  //first serial for debug in IDE Serial port
  Serial.begin(9600);
  //second serial connection for open Socket
  Serial1.begin(9600);

  Ethernet.begin(mac, ip, myDns, gateway, subnet);

  //open Socket
  server.begin();

  Serial.print("ip ard:   ");
  Serial.println(Ethernet.localIP());
  Serial.println("port ard:      12321");

  //set sensor pins to INPUT mode
  for (int i=0; i<P_SIZE;i++) {
    pinMode(p[i], INPUT_PULLUP);
  }
  pinMode(pin_power,INPUT_PULLUP);
  pinMode(pin_reset,INPUT_PULLUP);

  //set start and reset pins to off state
  digitalWrite(pin_power, HIGH);
  digitalWrite(pin_reset, HIGH);

  //set relay pins to OUTPUT mode
  for (int i=0; i<R_SIZE;i++) {
    pinMode(r[i], OUTPUT);
  }

  InitReset();

  Serial.println("ready");
}

void InitReset()
{
  // all relays to close position
  for (int i=0; i<R_SIZE;i++) {
    digitalWrite(r[i], HIGH);
  }
  //set all pins to off state
  for (int i=0; i<P_SIZE;i++) {
    digitalWrite(p[i], HIGH);
  }
  //set all sensors to off state
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

  // check if pin state changed then change var
  if (ChangeStatePin(pin_power, !start))
    start =  !GetStatePin(pin_power, start);

  if (ChangeStatePin(pin_reset, !reset))
    reset = !GetStatePin(pin_reset, reset);

  // incoming connections worker
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

  // reset from App or from reset button
  if (reset && !reseted) {
    Serial1.println("R");
    InitReset();
  }

  // check if pin state changed then change sensor var
  for (int i=0; i<P_SIZE;i++) {
    if (ChangeStatePin(p[i], !s[i])) {
      s[i] =  !GetStatePin(p[i], s[i]);
      delay(50);
    }
  }

  if (start && reseted) {
     reseted = false;
     u_end = false;
     sended_finish = false;
     step[0] = true;
     // send S to App that means that we start quest and need start in App timer
     Serial1.println("S");
  }

 // u_step[0] - it's remote launch from App
 // step[0] - means: work only after quest start
 // !step[1] - means: work only one time
 // s[0] - means: work after sensor s[0] is activated
 if (u_step[0] || (s[0] && step[0] && !step[1]))
  {
    step[1] = true;
    u_step[0] = false;
    digitalWrite(r[0], LOW);
    // first symbol must be P
    goplay("P_some_sound.mp3");
 }
 if (u_step[0] && digitalRead(r[0]) == LOW) {
    // disable relay by remote launch from App
    digitalWrite(r[0], HIGH);
    u_step[0] = false;
  }

 // u_step[1] - it's remote launch from App
 // step[1] - means: work only after prev step
 // !step[2] - means: work only one time
 // s[1] - means: work after sensor s[1] is activated
 if (u_step[1] || (s[1] && step[1] && !step[2]))
  {
    step[2] = true;
    u_step[1] = false;
    digitalWrite(r[1], LOW);
    // first symbol must be P
    goplay("P_some_sound.mp3");
 }
 if (u_step[1] && digitalRead(r[1]) == LOW) {
    // disable relay by remote launch from App
    digitalWrite(r[1], HIGH);
    u_step[1] = false;
  }

 // u_end - it's remote launch from App
  if (u_end)
  {
    u_end = false;
    goplay("P_some_sound.mp3");
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
    if (StrVar[UST_SIZE] == 'E') {
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
