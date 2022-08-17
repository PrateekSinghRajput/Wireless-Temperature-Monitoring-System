 
//Prateek
//www.prateeks.in


#include <Adafruit_SSD1306.h>
#include <Adafruit_MLX90614.h>
#include <Wire.h>
#include <millisDelay.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

const int GREEN_LED = 5;
const int RED_LED = 6;
const int buzzer = 8;
const int statePin = 10;

bool measurement = false;

const unsigned long interval_sensor = 50;
millisDelay sensorDelay;

const unsigned long interval_display = 500;
millisDelay displayDelay;

const unsigned long delay_hold_red = 5000;
const unsigned long delay_hold_green = 1000;
millisDelay holdDelay;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();


void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED Display allocation failed"));
    for (;;);
  }

  pinMode(statePin, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 20);
  display.println("Initializing");
  display.display();
  delay(250);
  display.clearDisplay();

  mlx.begin();

  displayDelay.start(interval_display);
}

void loop()
{
  static float temperature = -1;

  int state = digitalRead(statePin);

  if (state == LOW && measurement == false)
  {
    sensorDelay.start(interval_sensor);
    displayDelay.finish();
    measurement = true;
  }

  if (measurement == true)
  {
    //if sensor reading
    temperature = GetTemp();

  } else
  {
    temperature = -1;
  }

  ShowTemp(temperature );

  holdReading();
}

float GetTemp()
{
  static int index = 0;
  static float temptot = 0;
  float hasil = 0;
  if (sensorDelay.justFinished())
  {

    sensorDelay.repeat();
    temptot +=  mlx.readObjectTempF();
    index++;
    if (index == 19)
    {

      hasil = temptot / 20;
      temptot = 0;
      index = 0;

      sensorDelay.stop();
      displayDelay.finish();

      return hasil;
    }
  }
  return hasil;
}

void ShowTemp(float temperature)
{
  if (displayDelay.justFinished())
  {
    displayDelay.repeat();
    if (temperature == -1)
    {
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(35, 5);
      display.print("-----");
      display.setCursor(105, 20);
      display.print("");
      display.setTextSize(2);
      display.setCursor(35, 40);
      display.print("-----");
      display.setCursor(105, 46);
      display.print("");
      display.display();
    } else if (temperature == 0)
    {

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(20, 25);
      display.println("WAIT ....");
      display.display();
    } else
    {


      temperature += 8.5;

      display.clearDisplay();

      if ( temperature > 100)
      {
        display.setTextSize(3);
        display.setCursor(5, 20);
        display.print(temperature, 1);
        display.print("F");

      }
      else
      {
        display.setTextSize(3);
        display.setCursor(10, 20);
        display.print(temperature, 1);
        display.print(" F");

      }
      display.display();

      if (temperature > 100.4)
      {
        digitalWrite(RED_LED, HIGH);
        holdDelay.start(delay_hold_red);
      } else
      {

        digitalWrite(GREEN_LED, HIGH);
        holdDelay.start(delay_hold_green);
      }
      digitalWrite(buzzer, HIGH);

      displayDelay.stop();
    }
  }
}

void  holdReading()
{
  if (holdDelay.justFinished()) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(buzzer, LOW);

    measurement = false;

    displayDelay.start(interval_display);
  }
}
