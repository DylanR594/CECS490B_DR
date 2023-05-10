#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
//#include <HX711_ADC.h>
#include "HX711.h"

#define calibration_factor 9450.00 //This value is obtained using the SparkFun_HX711_Calibration sketch

#define LOADCELL_DOUT_PIN  4
#define LOADCELL_SCK_PIN  5

HX711 scale;

#define TFT_CS     10
#define TFT_RST    9 
#define TFT_DC     8
#define mosiSDA    11
#define SCL    13

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, mosiSDA, SCL, TFT_RST);
//temp sensor
Adafruit_MLX90614 tempSensor = Adafruit_MLX90614();
//pulsesensor
const int PULSE_INPUT_PIN = A0; // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
volatile bool buttonPressed = false;
volatile bool reset = false;
int buttonPresses = 0;

int PULSE_THRESHOLD = 550;     // Determine which signal to "count as a beat" and which to ignore.
PulseSensorPlayground pulseSensor;
//Buttons
int buttonPin = 2; // set the pin for the button switch
int buttonState = 0; // variable to store the state of the button switch

float tempC;
float tempF;
float heartRate;

void setup() {
  Serial.begin(9600);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare();
  //pinMode(buttonPin, INPUT); // set the button pin as an input
  pinMode(buttonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonISR, RISING);

   if (tempSensor.begin()) {
    Serial.println("TempSensor started!");  //prints one time at Arduino power-up,or on Arduino reset.  
  }
  pulseSensor.analogInput(PULSE_INPUT_PIN);
  pulseSensor.setThreshold(PULSE_THRESHOLD);
   if (pulseSensor.begin()) {
    Serial.println("PulseSensor started!");  //prints one time at Arduino power-up or on Arduino reset.  
  }
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_WHITE);
  tft.setTextWrap(true);
  tft.setTextSize(2);
}

#define SAMPLE_SIZE 50 //number of readings to take
int counter = 0;
void loop() {
 
  //tft.fillScreen(ST7735_WHITE);
  if(buttonPresses < 1){
    Serial.println(" ");
    Serial.println("Press Button to read temperature and BPM");
    tft.setCursor(0, 0);
    tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
    tft.setTextSize(1.5);
    tft.print("Press Button to read temperature and BPM");
  }
  if (buttonPressed && (buttonPresses < 1)) {
    buttonPresses += 1; 
     delay(50);
    // your code here
    Serial.println("Reading Temperature and BPM...");
    tft.setCursor(0, 30);
    tft.print("Reading Temperature and BPM...");
    float temp[SAMPLE_SIZE];
    float avgTemp = 0.0;
    float avgBPM = 0.0;
    for (int i = 0; i < SAMPLE_SIZE; i++) { //takes the number of samples and adds to avgTemp
      avgTemp += tempSensor.readObjectTempC();
      avgBPM += pulseSensor.getBeatsPerMinute(); //adds the current sample to the total
      delay(50); // wait before taking the next sample
    }
    avgTemp /= SAMPLE_SIZE; //divide the total by number of samples
    avgBPM /= SAMPLE_SIZE;
    float temp_object_f = (avgTemp * 9.0 / 5.0) + 32.0; //converts from celcius to farenheit
    Serial.print("Object temperature: ");
    Serial.print(temp_object_f);
    Serial.println(" F");
    Serial.println(" ");
    Serial.print("BPM: ");
    Serial.println(avgBPM);
    tft.fillScreen(ST7735_WHITE);

    tft.fillScreen(ST7735_WHITE);
    tft.setCursor(0, 0);
    tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
    tft.setTextSize(1.5);
    tft.print("Temp: ");
    tft.print(temp_object_f, 2);
    tft.print(" F");
    tft.setCursor(0, 30);
    tft.print("Heart Rate: ");
    tft.print(avgBPM);
    tft.print(" bpm");

    if ((temp_object_f > 70) && (avgBPM > 80)){
      tft.setCursor(0, 60);
      tft.setTextSize(1);
      tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
      tft.print("Possible symptoms for Covid ");
    }

    reset = true;
    buttonPressed = false; // reset the buttonPressed flag
  }
  else if(buttonPressed && (buttonPresses >= 1))
  {
    //delay(50);
    buttonPresses=0;
    buttonPressed = false;
    tft.fillScreen(ST7735_WHITE);
    delay(50);
  }

  Serial.print("Reading: ");
  Serial.print(scale.get_units(), 2); //scale.get_units() returns a float
  Serial.print(" lbs"); //You can change this to kg but you'll need to refactor the calibration_factor
  Serial.println();

  tft.setCursor(0, 100);
  tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
  tft.setTextSize(1);
  tft.print("Weight: ");
  tft.print(scale.get_units(), 2);
  tft.print(" lbs");  

  // check for new data/start next convers
  delay(50);



  // check for new data/start next conversion:

  // Serial.println("Reading Temperature...");
  // tft.setCursor(0, 30);
  // tft.print("Reading Temperature...");
  // float temp[SAMPLE_SIZE];
  // float avgTemp = 0.0;
  // for (int i = 0; i < SAMPLE_SIZE; i++) { //takes the number of samples and adds to avgTemp
  //     avgTemp += tempSensor.readObjectTempC();
  //     delay(50); // wait before taking the next sample
  // }
  // avgTemp /= SAMPLE_SIZE; //divide the total by number of samples
  // float temp_object_f = (avgTemp * 9.0 / 5.0) + 32.0; //converts from celcius to farenheit
  // Serial.print("Object temperature: ");
  // Serial.print(temp_object_f);
  // Serial.println(" F");
  // Serial.println(" ");
  // tft.fillScreen(ST7735_WHITE);


  // Serial.println("Press Button to read Heart rate");
  // tft.setCursor(0, 0);
  // tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
  // tft.setTextSize(1.5);
  // tft.print("Press Button to read BPM ");

  // while (digitalRead(buttonPin) == LOW) { //waits for button to pressed
  //   delay(50); // debounce the button
  //   //tft.fillScreen(ST7735_WHITE);
  // }
  // Serial.println("Reading Heart rate...");
  // tft.setCursor(0, 30);
  // tft.print("Reading Heart rate...");
  // float avgBPM = 0.0;
  // for (int i = 0; i < SAMPLE_SIZE; i++) {
  //     avgBPM += pulseSensor.getBeatsPerMinute(); //adds the current sample to the total
  //     delay(50); // wait before taking the next sample
  // } 
  // avgBPM /= SAMPLE_SIZE;
  // Serial.print("BPM: ");
  // Serial.println(avgBPM);  
  
  
  

  //tft.fillScreen(ST7735_WHITE);
  /*
  Serial.print("Temp: ");
  Serial.print(tempF);
  Serial.print(" F");
  Serial.print("\tHeart Rate: ");
  Serial.print(heartRate);
  Serial.println(" bpm");
  */
}

void buttonISR() {
  buttonPressed = true; // set the buttonPressed flag
}
