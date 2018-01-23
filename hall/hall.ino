//Simple sketch to access the internal hall effect detector on the esp32.
//values can be quite low.
//Brian Degger / @sctv

#define EMA_A .8
#define INACTIVE 37
#define THRESHOLD 34
int emaS;
int ticks = 0;
int val = 0;
int lastVal = 0;
bool in_tick = false;

void setup() {
  Serial.begin(9600);
  val = hallRead();
  lastVal = hallRead();
    }

void loop() {
  // put your main code here, to run repeatedly:
  val = hallRead();
  emaS = lowPassFilter(val);
  if(!in_tick && emaS < THRESHOLD){
    ticks++;
    in_tick = true;
  } else if(emaS > THRESHOLD){
    in_tick = false;
  }

  Serial.print(in_tick+20);
  Serial.print(", ");
  Serial.println(emaS);
  delay(20);
}

int lowPassFilter(int val){
    emaS = (EMA_A*val) + ((1-EMA_A)*emaS);
    return emaS;
}

