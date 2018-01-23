//Simple sketch to access the internal hall effect detector on the esp32.
//values can be quite low.
//Brian Degger / @sctv

#define EMA_A .1

int emaS;

int val = 0;
int lastVal = 0;
void setup() {
  Serial.begin(9600);
  val = hallRead();
  lastVal = hallRead();
    }

void loop() {
  // put your main code here, to run repeatedly:
  val = hallRead();
  emaS = lowPassFilter(val);
  int delta = emaS - lastVal;
  lastVal = emaS;
  int tick = delta < -10 ? 30: 36;
  Serial.println(delta);
//  Serial.print(", ");
//  Serial.println(emaS);
  delay(100);
}

int lowPassFilter(int val){
    emaS = (EMA_A*val) + ((1-EMA_A)*emaS);
    return emaS;
}

