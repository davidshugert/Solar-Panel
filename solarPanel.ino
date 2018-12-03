#include <SimpleTimer.h>
#include "DHT.h"
#include <SoftwareSerial.h>
#include <Servo.h>

Servo myServo;  // Servo Banda
Servo myServo2; //Servo Engrane
Servo myServo3;  //Servo Engrane

int dPin = 2;  //Tempeture PinOut 
SimpleTimer timer;
SimpleTimer timer2;
DHT dht(dPin, DHT11); // Tempeture Sensor PinOut
SoftwareSerial mySerial = SoftwareSerial(255, 6); //Screen PinOut

int lightTime = 2000; //Time interval to read the light sensors
int lightDiff =45; // Delta error of the sensors
int delayScreen = 4000; //Screen Time
int moveStep = 5; // Servo degrees
int yLimit = 60; // Limit of movement in Y axis
int motorDelay = 140;  //Limits the servo delta movement for saftey
int screenMode=0;
int modo=0; // modo 1 es para diagnosticos

/*Light Sensors PinOut */
int lUpRight = A0;
int lUpLeft= A1;
int lDownRight = A3;
int lDownLeft = A2;

void getLight(float & xAxis, float & yAxis,float & prom){
  int Sensors[] = {analogRead(lUpRight),analogRead(lUpLeft),analogRead(lDownRight),analogRead(lDownLeft)};
  int Up= (Sensors[0] + Sensors[1])/2;
  int Down= (Sensors[2]+Sensors[3])/2;
  int Left = (Sensors[3] + Sensors[1])/2;
  int Right = (Sensors[0]+Sensors[2])/2;
  prom= (Sensors[0]+Sensors[1]+Sensors[2]+Sensors[3])/4;
  
  //Assuming 1023 is brightest
  yAxis=Down-Up;
  xAxis=Right-Left;

  int data[] ={Sensors[0],Sensors[1],Sensors[2],Sensors[3],Up,Down,Left,Right,xAxis,yAxis,prom};

   // Optional Serial Print Out

   if(modo==1){
    
      Serial.print("Up-Left: ");
  Serial.print(data[1]);
  Serial.print(" Up-Right: ");
  Serial.print(data[0]);
  Serial.print(" Down-Right: ");
  Serial.print(data[2]);
  Serial.print(" Down-Left: ");
  Serial.println(data[3]);
  Serial.print("Up: ");
  Serial.print(data[4]);
  Serial.print(" Down: ");
  Serial.print(data[5]);
  Serial.print(" Right: ");
  Serial.print(data[6]);
  Serial.print(" Left: ");
  Serial.println(data[7]);
  Serial.print("Value on y: ");
  Serial.print(data[9]);
  Serial.print(" Value on x: ");
  Serial.print(data[8]);
  Serial.print("  Promedio ");
  Serial.println(data[10]);
    
    }else{ // for use of a visualization in "Program IDE"
      float voltaje= map(prom,0,1023,0,17);
      Serial.print((float)voltaje);
        float temp,humidity;
        getTemp(temp,humidity);
        Serial.print(",");
        Serial.print((float)temp);
      }


  }
  
void movePanel(){
  bool xDone = false;
  bool yDone = false; 
  
  while(!(xDone && yDone)){
  float xAxis,yAxis,prom;
  getLight(xAxis,yAxis,prom);
  
   if(abs(yAxis)<lightDiff){
    yDone= true;
   }
   if(abs(xAxis)<lightDiff){
    xDone= true;
   }
   if(!(xDone)){
      moveX(xAxis);
    }
   if(!(yDone)){
      moveY(yAxis);
    }
  int posX = myServo.read();
  int posY1 = myServo2.read();
  int posY2 = myServo3.read();
 }
  return;
  
}

int getTemp(float & t,float & h){
  h = dht.readHumidity();
  delay(75);
  t = dht.readTemperature();
  Serial.print("Temp: ");
  Serial.print(t);
  Serial.print("Humedad: ");
  Serial.println(h);
  if (isnan(h) || isnan(t)) {
  Serial.println("Failed to read from DHT sensor!");
  }
}

void moveX(int pos){
  int posX = myServo.read();
   if(pos>0){
    myServo.write(posX+moveStep);
   }else{
    myServo.write(posX-moveStep);
    }
   delay(motorDelay);
  
}
void moveY(int pos){
  int posY1 = myServo2.read();
  int posY2 = myServo3.read();

  if(pos<0){
    posY1=posY1+moveStep;
    posY2=posY2-moveStep;
    }
  if(pos>0){
    posY1=posY1-moveStep;
    posY2=posY2+moveStep;
    }
  
  if(posY1<=yLimit){
    posY1=yLimit;
    posY2=180-yLimit;
    }
    if(posY1>=180-yLimit){
    posY2=yLimit;
    posY1=180-yLimit;
    }
  myServo2.write(posY1);
  myServo3.write(posY2); 
  delay(motorDelay);
  }
  
void displayScreen(){
  float temp,humidity;
  float xAxis,yAxis,prom;
  getTemp(temp,humidity);
  getLight(xAxis,yAxis,prom);
switch (screenMode) {
    case 1:
       /*-------------Temperatura---------------- */
  mySerial.write(12);                 // Clear 
  mySerial.print("Temp: ");    // First line
  mySerial.print(round(temp));               // Form feed
  mySerial.print(" C");               // Second line
  mySerial.write(13);                 // Form feed
  mySerial.print("Humedad: ");        // First line
  mySerial.print(round(humidity));           // Form feed
  mySerial.print(" %\t");             // Second line
  screenMode=2;
      break;
    case 2:
       /*-------------Luz----------------------- */
  mySerial.write(12);                    // Clear 
  mySerial.print("Luz: ");    // First line
  mySerial.print(map(prom,0,1023,0,10)); // Form feed
  mySerial.write(13);                    // Form feed
  mySerial.print("Voltaje: ");           // First line
  mySerial.print(map(prom,0,1023,12,17));// Form feed
  mySerial.print(" V");                // Second line
  screenMode = 0;
      break;
    default:
       /*-------------Intro---------------- */
  mySerial.write(12);                 // Clear
  mySerial.print("Panel Solar");      // First line
  mySerial.write(13);                 // Form feed
  mySerial.print("Los Vengadores");   // Second line
  screenMode = 1;
      break;
  }
  }

void setup() {
  myServo.attach(9);  
  myServo2.attach(10);
  myServo3.attach(11);
  Serial.begin(9600);
  pinMode(dPin, OUTPUT);
  digitalWrite(dPin, HIGH);
  mySerial.begin(9600);
  delay(100);
  mySerial.write(12);  // Clear             
  mySerial.write(17);  // Turn backlight on
  delay(5);
  timer.setInterval(lightTime,movePanel);
  timer2.setInterval(delayScreen,displayScreen);
}

void loop() {
  timer.run();
  timer2.run();
}
