#include <math.h> 

void setup() {
  Serial.begin(9600);  
}

void loop() {

  float test = -123.456;
  
  Serial.println(fabs(test));

  delay(10000);

}
