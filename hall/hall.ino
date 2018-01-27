#define EMA_A .65
#define INACTIVE 42
#define THRESHOLD -2
#define DELAY 0
#define TICK_LIGHT 25

int emaS1;
int emaS2;
unsigned long ticks = 0;
int val = 0;
int hpv = 0;
int lastVal = 0;
bool inTick = false;

unsigned long lastTicks = 0;
unsigned long lastMillis = 0;
unsigned long checkMillis = 0;
int rpm = 0;

void setup() {
  Serial.begin(9600);
  pinMode(TICK_LIGHT, OUTPUT);
  digitalWrite(TICK_LIGHT, 0);
  val = hallRead();
  lastVal = hallRead();
  emaS1 = lastVal;
  lastMillis = millis();
}

void loop() {
  val = hallRead();
  lowPassFilter(&emaS1, val);
  hpv = val - emaS1;
  if(!inTick && hpv < THRESHOLD){
    ticks++;
    inTick = true;
    digitalWrite(TICK_LIGHT, 1);
  } else if(hpv > THRESHOLD){
    inTick = false;
    digitalWrite(TICK_LIGHT, 0);
  }
  checkMillis = millis();
  if(checkMillis - lastMillis  > 2000){

    rpm = (ticks - lastTicks)  * 60000 / (checkMillis - lastMillis);

    lastTicks = ticks;
    lastMillis = checkMillis;
  }

  Serial.print(inTick+20);
  Serial.print(", ");
  Serial.print(rpm);
  Serial.print(", ");
  Serial.println(hpv);
  delay(0);
}

int lowPassFilter(int *pems, int val){
    *pems = (EMA_A*val) + ((1-EMA_A) * *pems);
    return *pems;
}

