int VERT = 2; // pin des led Vertes
int VERT1 = 8; // pin des led Vertes
int VERT2 = 12; // pin des led Vertes
int VERT3 = 13; // pin des led Vertes
int ROUGE = 4; // pin des led Rouges
int JAUNE = 7; // pin des led Rouges
int BUZZER = 0; // analog pin A0 for buzzer 
int SONG = 255; // Buzzer tone
void setup() {
  // put your setup code here, to run once:
pinMode(VERT, OUTPUT);
pinMode(VERT1, OUTPUT);
pinMode(VERT2, OUTPUT);
pinMode(VERT3, OUTPUT);
pinMode(JAUNE, OUTPUT);
pinMode(ROUGE, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(JAUNE, HIGH);
  digitalWrite(ROUGE, HIGH);
  digitalWrite(VERT, LOW); // Allumer les led Vertes pendant 25 s
  delay(250);
  digitalWrite(VERT1, LOW); // Allumer les led Vertes pendant 25 s
  delay(250);
  digitalWrite(VERT2, LOW); // Allumer les led Vertes pendant 25 s
  delay(250);
  digitalWrite(VERT3, LOW); // Allumer les led Vertes pendant 25 
  
  
  noTone(0);
  delay(2000);
    
  digitalWrite(VERT, HIGH); // Eteindre les Led vertes puis allumer les led orange pendant 5s
  digitalWrite(JAUNE, LOW);
  digitalWrite(VERT1, HIGH);
  digitalWrite(VERT2, HIGH);
  digitalWrite(VERT3, HIGH);
  noTone(0);
  delay(3000);
  
  digitalWrite(JAUNE, HIGH);//Allumez les led Rouge pendant 30 s
  digitalWrite(ROUGE, LOW);
  tone(BUZZER, SONG);
  delay(3000);
}
