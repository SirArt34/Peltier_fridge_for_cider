#include <Wire.h>   // standardowa biblioteka Arduino
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define DHTPIN_1 5
#define DHTTYPE_1 DHT22
DHT dht1(DHTPIN_1, DHTTYPE_1);
#define DHTPIN_2 6
#define DHTTYPE_2 DHT22
DHT dht2(DHTPIN_2, DHTTYPE_2);

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

#define Peltier_1 2 
#define Peltier_2 3 

#define ok 12 //przycisk ok
#define up 11 //przycisk up
#define down 10 //przycisk down
#define up2 9 //przycisk up
#define down2 8 //przycisk down


int tmin=10;  // USTAW - Temperatura minimalna
int tmax=26;  // USTAW - Temperatura maxymalna
float temp=0.0;   // Srednia temperatury z dwoch czojnikow
    
    //zmienne przedstawiające stan pompy 
    int chlodzenie=0;  //0 - ustawienie temperatury min i max 1 - Pomiar 2 - wyłączone   3 - włączone
    #define USTAWIENIA 0 
    #define POMIAR 1 
    #define OFF  2 
    #define ON  3 

    //zmienne zawierające czas 
    unsigned long time_on = 0;       //czas włączenia pompy 
    unsigned long aktualny_czas=0;   //aktualny czas 

void setup() 
  { 
  Serial.begin(9600); 
  lcd.begin(20,4);
  lcd.backlight();
  lcd.setCursor(2,0);
  lcd.print("Lodowka Peltiera");
    delay(1000);
  lcd.setCursor(6,1);
  lcd.print("ver 1.0");
    delay(1000);
  lcd.setCursor(7,3);
  lcd.print("2018r");
    delay(1000);

  lcd.clear();
    
  dht1.begin();
  dht2.begin();
  
  pinMode(Peltier_1, OUTPUT);     // Peltier 1 
  pinMode(Peltier_2, OUTPUT);     // Peltier 2 
  pinMode(up, INPUT_PULLUP); //UP
  pinMode(down, INPUT_PULLUP);//DOWN
  pinMode(up2, INPUT_PULLUP); //UP
  pinMode(down2, INPUT_PULLUP);//DOWN
  pinMode(ok, INPUT_PULLUP);//DOWN

  digitalWrite(Peltier_1, HIGH);
  digitalWrite(Peltier_2, HIGH);  
  chlodzenie = 0; 
  time_on = 0; 
    } 
      
    void loop() 
    { 
      //Pobierz liczbe milisekund od startu 
      aktualny_czas = millis(); 

float t1 = dht1.readTemperature();
float t2 = dht2.readTemperature();

 if (isnan(t1)) {
    lcd.setCursor(0,0);
    lcd.print("Failed to read from DHT #1");
  } else {
    Serial.print("Temperature 1: "); 
    Serial.print(t1);
    Serial.println(" *C");
  }
  if (isnan(t2)) {
    lcd.setCursor(0,3);
    lcd.println("Failed to read from DHT #2");
  } else {

    Serial.print("Temperature 2: "); 
    Serial.print(t2);
    Serial.println(" *C");
  }
  temp=(t1+t2)/2;

      //w pętli switch w zależności od wartości zmiennej Peltier wykonujemy nasz program 
      switch(chlodzenie) 
      { 
        //---------------------------------------------------------------------------- 
        case USTAWIENIA: 
             //----------------------------------przyciski---------------------// 
           Serial.println("USTAWIENIA");
           
            if (digitalRead(up) == LOW ){ //jeśli przycisk jest wciśnięty
                tmin ++; //dodaj +1 do naszego licznika
              while (digitalRead(up) == LOW) {
                delay(150);} //wyjście z if'a
                }
            if (digitalRead(down) == LOW ) { //jeśli przycisk jest wciśnięty
                tmin --; //odejmij -1 do naszego licznika
              while (digitalRead(down) == LOW){
                delay(150); } //wyjście z if'a 
                }
            if (digitalRead(up2) == LOW ){ //jeśli przycisk jest wciśnięty
                tmax ++; //dodaj +1 do naszego licznika
              while (digitalRead(up2) == LOW) {
                delay(150);} //wyjście z if'a
                }
            if (digitalRead(down2) == LOW ) { //jeśli przycisk jest wciśnięty
                tmax --; //odejmij -1 do naszego licznika
              while (digitalRead(down2) == LOW){
                delay(150); } //wyjście z if'a 
                }

              //---------------------------------LCD-------------------------//
           Serial.println("LCD");
             lcd.setCursor(0,0);
             lcd.print("USTAWIENIA:");
             lcd.setCursor(0,2);
             lcd.print("Temp. min.:");
             lcd.setCursor(0,3);
             lcd.print("Temp. max.:");

             lcd.setCursor(12,2);
             lcd.print(tmin);
             lcd.setCursor(15,2);
             lcd.print("\xDF" "C");
             lcd.setCursor(12,3);
             lcd.print(tmax);
             lcd.setCursor(15,3);
             lcd.print("\xDF" "C");
//-------------------------PODCZAS USTAWIEN PELTIER WYLACZONY------------------------
          digitalWrite(Peltier_1, HIGH);
              
              
              if (digitalRead(ok) == LOW){
                delay(200);
                lcd.clear();
                chlodzenie=POMIAR; 
                } //wyjście z if'a 
                
         break;    //wyjście ze switch 
  //--------------------------------------------------------------------------//       
        case POMIAR: 
           Serial.println("POMIAR");
 //----------------------------------LCD----------------------------------------------//
             lcd.setCursor(7,0);
             lcd.print("POMIAR");
             lcd.setCursor(2,1);
             lcd.print("Temp. w srodku:");
             lcd.setCursor(6,2);
             lcd.print(temp);
             lcd.setCursor(12,2);
             lcd.print("\xDF" "C");   
             lcd.setCursor(2,3);
             lcd.print("Min:");
             lcd.setCursor(7,3);
             lcd.print(tmin);               
             lcd.setCursor(11,3);
             lcd.print("Max:");
             lcd.setCursor(16,3);
             lcd.print(tmax); 
           
          if(temp>=tmax)             //Jeśli temperatura wzrosla powyzej T-MAX
            { 
              time_on=aktualny_czas;             //zapamiętaj czas włączenia 
              digitalWrite(Peltier_1, LOW);        //włącz peltiera
             
              chlodzenie=ON; 
            } 
            if(temp<=tmin){
              chlodzenie=OFF;
            }
                if (digitalRead(ok) == LOW){
                delay(200);
                lcd.clear();
                chlodzenie=USTAWIENIA; 
                } 
          break;    //wyjście ze switch 
        //---------------------------------------------------------------------------- 
        case OFF: 
          //chlodzenie wylaczone 
           Serial.println("OFF");
 //----------------------------------LCD----------------------------------------------//
             lcd.setCursor(1,0);
             lcd.print("PELTIER WYLACZONY!");
             lcd.setCursor(2,1);
             lcd.print("Temp. w srodku:");
             lcd.setCursor(6,2);
             lcd.print(temp);
             lcd.setCursor(12,2);
             lcd.print("\xDF" "C");   
             lcd.setCursor(2,3);
             lcd.print("Min:");
             lcd.setCursor(7,3);
             lcd.print(tmin);               
             lcd.setCursor(11,3);
             lcd.print("Max:");
             lcd.setCursor(16,3);
             lcd.print(tmax);  

             
          if(t1>=tmax)             //Jeśli temperatura wzrosla powyzej T-MAX
            { 
              time_on=aktualny_czas;             //zapamiętaj czas włączenia 
              digitalWrite(Peltier_1, LOW);        //włącz peltiera
             
              chlodzenie=ON; 
            } 
                if (digitalRead(ok) == LOW){
                delay(200);
                lcd.clear();
                chlodzenie=USTAWIENIA; 
                } 
          break;    //wyjście ze switch 
         //---------------------------------------------------------------------------- 
         case ON: 
           Serial.println("ON");
 //----------------------------------LCD----------------------------------------------//
             lcd.setCursor(1,0);
             lcd.print("PELTIER WLACZONY!");
             lcd.setCursor(2,1);
             lcd.print("Temp. w srodku:");
             lcd.setCursor(6,2);
             lcd.print(temp);
             lcd.setCursor(12,2);
             lcd.print("\xDF" "C");   
             lcd.setCursor(2,3);
             lcd.print("Min:");
             lcd.setCursor(7,3);
             lcd.print(tmin);               
             lcd.setCursor(11,3);
             lcd.print("Max:");
             lcd.setCursor(16,3);
             lcd.print(tmax);  

           if(t1<=tmin)             //Jeśli temperatura spadła do T-MIN 
            { 
              digitalWrite(Peltier_1, HIGH);         //wyłącz chlodzenie 
              chlodzenie=OFF; 

            } 
                if (digitalRead(ok) == LOW){
                delay(200);
                lcd.clear();
                chlodzenie=USTAWIENIA; 
                } 
   //wyjście ze switch 
      default:
      break;
      }       //koniec switch 
    } 
