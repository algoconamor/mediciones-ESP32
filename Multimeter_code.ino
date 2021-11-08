/*
Multimeter code by ELECTRONOOBS on 09/02/2019
Schematic: https://www.electronoobs.com/eng_arduino_tut84_sch1.php
Code: https://www.electronoobs.com/eng_arduino_tut84_code1.php
YOUTUBE CHANNEL: https://www.youtube.com/c/ELECTRONOOBS
*/

#include <Adafruit_GFX.h>             //DOWNLOAD: https://www.electronoobs.com/eng_arduino_Adafruit_GFX.php
#include <Adafruit_SSD1306.h>         //DOWNLOAD: https://www.electronoobs.com/eng_arduino_Adafruit_SSD1306.php
#define OLED_RESET 10
Adafruit_SSD1306 display(OLED_RESET);
#include <Adafruit_ADS1015.h>         //DOWNLOAD: https://www.electronoobs.com/eng_arduino_Adafruit_ADS1015.php
Adafruit_ADS1115 ads(0x48);


//inputs/outputs PUT THE PINS AS IN THE SCHEMATIC
int res_2k = 6;
int res_20k = 7;
int res_470k = 8;
#define cap_in_analogPin     0          
#define chargePin           13        
#define dischargePin        11
int pulse_induct_out = 5;
int oscillate_in = 9;
const int OUT_PIN = A1;
//const int IN_PIN = A0;
int top_but = 2;
int mid_but = 3;
int bot_but = 4;


//Modes variables
int mode = 0;
int res_scale = 0;
int cap_scale = 0;
bool mid_but_state = true;
bool top_but_state = true;


//Variables for voltage mode
float Voltage = 0.0;
float resistance_voltage = 0.0;
float battery_voltage = 0.0;
float measured_resistance = 0.0;


//Variables for resistance mode
float Res_2k_value = 1998;         //2K resistor          //CHANGE THIS VALUES. MEASURE YOUT 2k, 20k and 470K and put the real values here
float Res_20k_value = 1.962;      //20K resistor
float Res_470k_value = 0.4655;    //470K resistor


//Variables for big capacitance mode
#define resistorValue  9900.0F  //Remember, we've used a 10K resistor to charge the capacitor MEASURE YOUR VALUE!!!
unsigned long startTime;
unsigned long elapsedTime;
float microFarads;                
float nanoFarads;
const float IN_STRAY_CAP_TO_GND = 47.48;
const float IN_CAP_TO_GND  = IN_STRAY_CAP_TO_GND;
const float R_PULLUP = 34.8;  
const int MAX_ADC_VALUE = 1023;


//Variables for inductance mode
//D5 is the input to the circuit (connects to 150ohm resistor), 11 is the comparator/op-amp output.
double pulse, frequency, capacit, inductance;
float C_cap_value = 1E6;                    //The capacitor value used for the LC tank. See schematic. For me that is 2uF

//Variables for current mode with the ACS712 of 5A range
float Current_sensor_Resolution = 0.185;



void setup(void)
{  
  pinMode(top_but,INPUT_PULLUP);
  pinMode(mid_but,INPUT_PULLUP);
  pinMode(bot_but,INPUT_PULLUP);
  pinMode(res_2k,OUTPUT);
  pinMode(res_20k,INPUT);
  pinMode(res_470k,INPUT);
  pinMode(OUT_PIN, OUTPUT);
  pinMode(cap_in_analogPin, OUTPUT); 
  pinMode(pulse_induct_out,OUTPUT);
  pinMode(oscillate_in,INPUT);
  
  digitalWrite(res_2k,LOW);

  pinMode(chargePin, OUTPUT);     
  digitalWrite(chargePin, LOW); 
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  delay(100);  
  display.clearDisplay();

  
  display.setTextSize(1);
  display.setTextColor(WHITE); 
  display.setCursor(25,0);
  display.print("ELECTRONOOBS");  
  display.setCursor(30,20);    
  display.print("MULTIMETER"); 
 

    
  display.display();
  delay(1000);
  Serial.begin(9600);
  ads.begin();
}

void loop(void)
{

   
      
  if(!digitalRead(mid_but) && mid_but_state)
  {
    mode = mode + 1;
    res_scale = 0;
    cap_scale = 0;
    mid_but_state = false;
    if(mode > 4)
    {
      mode=0;
    }
    delay(100);
  }

  if(digitalRead(mid_but) && !mid_but_state)
  {
    mid_but_state = true;
  }


  if(!digitalRead(top_but) && top_but_state)
  {
    res_scale = res_scale + 1;
    cap_scale = cap_scale + 1;
    top_but_state = false;
    if(res_scale > 2)
    {
      res_scale=0;
    }
    if(cap_scale > 1)
    {
      cap_scale=0;
    }
     startTime = micros();           //Reset the counters      
     elapsedTime= micros() - startTime;
    delay(100);
  }

  if(digitalRead(top_but) && !top_but_state)
  {
    top_but_state = true;
  }

  


////////////////////////////VOLTAGE/////////////////////////////
  if(mode == 0)
  {
    int16_t adc0; // Leemos el ADC, con 16 bits 
    adc0 = ads.readADC_SingleEnded(0);
    Voltage = (adc0 * 0.1875)/1000;    
    Voltage = (Voltage / 0.245108); //R1 = 6.674    R2 = 2,167
    if(Voltage > 0.3)
    {
      Voltage = Voltage +  0.27; //We sum 0.27? (voltage drop on the diode only if voltage is applied)
    }
    int16_t adc2; // Leemos el ADC, con 16 bits   
    adc2 = ads.readADC_SingleEnded(2);
    battery_voltage = ((adc2 * 0.1875)/1000);
      
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE); 
    display.setCursor(0,0);
    display.print("Volts"); 
    display.setTextSize(1);
    display.setTextColor(BLACK,WHITE); 
    display.setCursor(95,0);    
    display.print(battery_voltage,1); 
    display.print("V");

    if(Voltage > 0)
    {    
      display.setTextSize(3);
      display.setTextColor(BLACK,WHITE); 
      display.setCursor(0,22);
      display.print(Voltage,4);        
      display.display();
      delay(100);
    }
    else
    {    
      display.setTextSize(3);
      display.setTextColor(BLACK,WHITE); 
      display.setCursor(0,22);
      display.print("0.0000");         
      display.display();
      delay(100);
    }
  }






////////////////////////////RESISTANCE/////////////////////////////
  if(mode == 1)
  {
    if(res_scale == 0)
    {
      pinMode(res_2k,OUTPUT);
      pinMode(res_20k,INPUT);
      pinMode(res_470k,INPUT);     
      digitalWrite(res_2k,LOW);
  
      int16_t adc1; // Leemos el ADC, con 16 bits   
      adc1 = ads.readADC_SingleEnded(1);
      resistance_voltage = (adc1 * 0.1875)/1000;
      int16_t adc2; // Leemos el ADC, con 16 bits   
      adc2 = ads.readADC_SingleEnded(2);
      battery_voltage = ((adc2 * 0.1875)/1000);
      measured_resistance = (Res_2k_value - 149) * (  (battery_voltage/resistance_voltage)-1  );
      if(measured_resistance < 4000)
      {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE); 
        display.setCursor(0,0);
        display.print("Ohms");
        display.setTextSize(1);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(95,0);    
        display.print(battery_voltage,1); 
        display.print("V");
        
        display.setTextSize(3);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(0,22);
        display.print(measured_resistance,5);  
        display.display();
        delay(100);        
      }
      else
      {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE); 
        display.setCursor(0,0);
        display.print("Ohms");
        display.setTextSize(1);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(95,0);    
        display.print(battery_voltage,1); 
        display.print("V");
        
        display.setTextSize(3);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(0,22);
        display.print(">4000");  
        display.display();          
        delay(100);
      }
    }
    if(res_scale == 1)
    {
      pinMode(res_2k,INPUT);
      pinMode(res_20k,OUTPUT);
      pinMode(res_470k,INPUT);     
      digitalWrite(res_20k,LOW);
      int16_t adc1; // Leemos el ADC, con 16 bits   
      adc1 = ads.readADC_SingleEnded(1);
      resistance_voltage = (adc1 * 0.1875)/1000;
      int16_t adc2; // Leemos el ADC, con 16 bits   
      adc2 = ads.readADC_SingleEnded(2);
      battery_voltage = ((adc2 * 0.1875)/1000);      
      measured_resistance = (Res_20k_value - 0.149) * (  (battery_voltage/resistance_voltage)-1  );
      if(measured_resistance < 200)
      {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE); 
        display.setCursor(0,0);
        display.print("Kohms");
        display.setTextSize(1);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(95,0);    
        display.print(battery_voltage,1); 
        display.print("V");
        
        display.setTextSize(3);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(0,22);
        display.print(measured_resistance,2);  
        display.display();          
        delay(100);        
      }
      else
      {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE); 
        display.setCursor(0,0);
        display.print("Kohms");
        display.setTextSize(1);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(95,0);    
        display.print(battery_voltage,1); 
        display.print("V"); 
        
        display.setTextSize(3);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(0,22);
        display.print(">200K");  
        display.display();          
        delay(100);
      }
    }
    if(res_scale == 2)
    {
      pinMode(res_2k,INPUT);
      pinMode(res_20k,INPUT);
      pinMode(res_470k,OUTPUT);     
      digitalWrite(res_470k,LOW);
      int16_t adc1; // Leemos el ADC, con 16 bits   
      adc1 = ads.readADC_SingleEnded(1);
      resistance_voltage = (adc1 * 0.1875)/1000;
      int16_t adc2; // Leemos el ADC, con 16 bits   
      adc2 = ads.readADC_SingleEnded(2);
      battery_voltage = ((adc2 * 0.1875)/1000);
      measured_resistance = (Res_470k_value - 0.000149) * (  (battery_voltage/resistance_voltage)-1  );
      if(measured_resistance < 4 && measured_resistance > 0)
      {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE); 
        display.setCursor(0,0);
        display.print("Mohms");
        display.setTextSize(1);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(95,0);    
        display.print(battery_voltage,1); 
        display.print("V"); 
        
        display.setTextSize(3);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(0,22);
        display.print(measured_resistance,2);  
        display.display();          
        delay(200);
      }
      else
      {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE); 
        display.setCursor(0,0);
        display.print("Mohms"); 
        display.setTextSize(1);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(95,0);    
        display.print(battery_voltage,1); 
        display.print("V");
        
        display.setTextSize(3);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(0,22);
        display.print(">4M");  
        display.display();          
        delay(200);
      }
    }
  }//end of mode 1 = resistance mode


  if(mode == 2)
  {
    int16_t adc2; // Leemos el ADC, con 16 bits   
    adc2 = ads.readADC_SingleEnded(2);
    battery_voltage = ((adc2 * 0.1875)/1000);
    if(cap_scale == 0)    
    {   
      pinMode(cap_in_analogPin, INPUT);  
      pinMode(OUT_PIN,OUTPUT);
      digitalWrite(OUT_PIN, LOW); 
      pinMode(chargePin, OUTPUT);   
       
      digitalWrite(chargePin, HIGH);  //apply 5 Volts
      startTime = micros();           //Start the counter
      while(analogRead(cap_in_analogPin) < 648){       //While the value is lower than 648, just wait
      }
    
      elapsedTime= micros() - startTime;
      microFarads = ((float)elapsedTime / resistorValue) ; //calculate the capacitance value
  
  
      if (microFarads > 1)
      {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE); 
        display.setCursor(0,0);
        display.print("uF");
        display.setTextSize(1);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(95,0);    
        display.print(battery_voltage,1); 
        display.print("V");
        
        display.setTextSize(3);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(0,22);
        display.print(microFarads);  
        display.display();          
        delay(100);   
      }
    
      else{
        nanoFarads = microFarads * 1000.0; 
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE); 
        display.setCursor(0,0);
        display.print("nF");
        display.setTextSize(1);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(95,0);    
        display.print(battery_voltage,1); 
        display.print("V"); 
        
        display.setTextSize(3);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(0,22);
        display.print(nanoFarads);  
        display.display();
        delay(100);
      }
  
      digitalWrite(chargePin, LOW);            
      pinMode(dischargePin, OUTPUT);            
      digitalWrite(dischargePin, LOW);     //discharging the capacitor     
      while(analogRead(cap_in_analogPin) > 0){         
      }//This while waits till the capaccitor is discharged
    
      pinMode(dischargePin, INPUT);      //this sets the pin to high impedance
      
              
      display.setTextSize(1);
      display.setTextColor(BLACK,WHITE); 
      display.setCursor(0,54);
      display.print("Discharging...");  
      display.display();       
    }



    if(cap_scale == 1)    
    {
      pinMode(chargePin, INPUT);     
      pinMode(dischargePin, INPUT);  
      pinMode(cap_in_analogPin, INPUT);
      digitalWrite(OUT_PIN, HIGH);
      int val = analogRead(cap_in_analogPin);
      digitalWrite(OUT_PIN, LOW);
  
      if (val < 1000)
      {
        pinMode(cap_in_analogPin, OUTPUT);  
        float capacitance = (float)val * IN_CAP_TO_GND / (float)(MAX_ADC_VALUE - val);
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE); 
        display.setCursor(0,0);
        display.print("pF");
        display.setTextSize(1);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(95,0);    
        display.print(battery_voltage,1); 
        display.print("V");
        
        display.setTextSize(3);
        display.setTextColor(BLACK,WHITE); 
        display.setCursor(0,22);
        display.print(capacitance);  
        display.display();
        delay(200);
      }
      
      else
      {
        pinMode(cap_in_analogPin, OUTPUT);
        delay(1);
        pinMode(OUT_PIN, INPUT_PULLUP);
        unsigned long u1 = micros();
        unsigned long t;
        int digVal;
  
        do
        {
          digVal = digitalRead(OUT_PIN);
          unsigned long u2 = micros();
          t = u2 > u1 ? u2 - u1 : u1 - u2;
        } 
        
        while ((digVal < 1) && (t < 400000L));
  
        pinMode(OUT_PIN, INPUT);  
        val = analogRead(OUT_PIN);
        digitalWrite(cap_in_analogPin, HIGH);
        int dischargeTime = (int)(t / 1000L) * 5;
        delay(dischargeTime);   
        pinMode(OUT_PIN, OUTPUT);  
        digitalWrite(OUT_PIN, LOW);
        digitalWrite(cap_in_analogPin, LOW);
  
        float capacitance = -(float)t / R_PULLUP / log(1.0 - (float)val / (float)MAX_ADC_VALUE);
  
               
        if (capacitance > 1000.0)
        {
          display.clearDisplay();
          display.setTextSize(2);
          display.setTextColor(WHITE); 
          display.setCursor(0,0);
          display.print("uF");
          display.setTextSize(1);
          display.setTextColor(BLACK,WHITE); 
          display.setCursor(95,0);    
          display.print(battery_voltage,1); 
          display.print("V"); 
          
          display.setTextSize(3);
          display.setTextColor(BLACK,WHITE); 
          display.setCursor(0,22);
          display.print(capacitance/1000);  
          display.display();
          delay(200);        
        }
          
        else
        {
          display.clearDisplay();
          display.setTextSize(2);
          display.setTextColor(WHITE); 
          display.setCursor(0,0);
          display.print("nF");
          display.setTextSize(1);
          display.setTextColor(BLACK,WHITE); 
          display.setCursor(95,0);    
          display.print(battery_voltage,1); 
          display.print("V"); 
          
          display.setTextSize(3);
          display.setTextColor(BLACK,WHITE); 
          display.setCursor(0,22);
          display.print(capacitance);  
          display.display();
          delay(200);
        }
    }
      //while (micros() % 1000 != 0);
    }
    
  }//end of mode 2  = capacitance




  if(mode == 3)
  {
    
    digitalWrite(pulse_induct_out, HIGH);
    delay(4);//give some time to charge inductor.
    digitalWrite(pulse_induct_out,LOW);
    delayMicroseconds(100); //make sure resonation is measured
    pulse = pulseIn(oscillate_in,HIGH,5000);//returns 0 if timeout
    
    if(pulse > 0.1){ //if a timeout did not occur and it took a reading:            
      //#error insert your used capacitance value here. Currently using 2uF. Delete this line after that
      capacit = 16.E-7; // - insert value here
      
      int16_t adc2; // Leemos el ADC, con 16 bits   
      adc2 = ads.readADC_SingleEnded(2);
      battery_voltage = ((adc2 * 0.1875)/1000);
      
      frequency = C_cap_value/(2*pulse);
      inductance = 1./(capacit*frequency*frequency*4.*3.14159*3.14159);//one of my profs told me just do squares like this
      inductance *= C_cap_value; //note that this is the same as saying inductance = inductance*1E6

      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE); 
      display.setCursor(0,0);
      display.print("uH");
      display.setTextSize(1);
      display.setTextColor(BLACK,WHITE); 
      display.setCursor(95,0);    
      display.print(battery_voltage,1); 
      display.print("V");    
          
      display.setTextSize(3);
      display.setTextColor(BLACK,WHITE); 
      display.setCursor(0,22);
      display.print(inductance);  
      display.display();
      delay(100);      
    }
    else
    {
      int16_t adc2; // Leemos el ADC, con 16 bits   
      adc2 = ads.readADC_SingleEnded(2);
      battery_voltage = ((adc2 * 0.1875)/1000);
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE); 
      display.setCursor(0,0);
      display.print("uH");
      display.setTextSize(1);
      display.setTextColor(BLACK,WHITE); 
      display.setCursor(95,0);    
      display.print(battery_voltage,1); 
      display.print("V");  
          
      display.setTextSize(3);
      display.setTextColor(BLACK,WHITE); 
      display.setCursor(0,22);
      display.print("NONE");  
      display.display();
      delay(100); 
    }
    
  }//end of mode 3 = inductance
  








   //Current mode
   if(mode == 4)
   {  
      int16_t adc3;                                 // Create the ADC of 16 bits
      adc3 = ads.readADC_SingleEnded(3);            //The Current is connected on teh ADC 3
      float SensorVoltage = ((adc3 * 0.1875)/1000); //Pass from digital to voltage values  
      float I = (SensorVoltage-2.5)/Current_sensor_Resolution;       //Get current value using the formula
      
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE); 
      display.setCursor(0,0);
      display.print("Amps");
      display.setTextSize(1);
      display.setTextColor(BLACK,WHITE); 
      display.setCursor(95,0);    
      display.print(battery_voltage,1); 
      display.print("V");  
          
      display.setTextSize(3);
      display.setTextColor(BLACK,WHITE); 
      display.setCursor(0,22);
      display.print(I,3);  
      display.display();
      delay(100);  
    
   }//end of mode 4 = current


  


  
}
