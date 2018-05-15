#include <DallasTemperature.h>
#include <OneWire.h>

unsigned long time;
// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_INFLOW 6
#define ONE_WIRE_OUTFLOW 8
#define ONE_WIRE_F1 9
#define ONE_WIRE_F2 4
#define HALLSENSOR 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire_inflow(ONE_WIRE_INFLOW);
OneWire oneWire_outflow(ONE_WIRE_OUTFLOW);
OneWire oneWire_f1(ONE_WIRE_F1);
OneWire oneWire_f2(ONE_WIRE_F2);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensor_inflow(&oneWire_inflow);
DallasTemperature sensor_outflow(&oneWire_outflow);
DallasTemperature sensor_f1(&oneWire_f1);
DallasTemperature sensor_f2(&oneWire_f2);

volatile int NbTopsFan; //measuring the rising edges of the signal
int Calc;                               

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  // Start up the library
  sensor_inflow.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
  sensor_outflow.begin();
  sensor_f1.begin();
  sensor_f2.begin();
  pinMode(HALLSENSOR, INPUT); //initializes digital pin 2 as an input
  attachInterrupt(0, rpm, RISING); //and the interrupt is attached
}
void rpm ()     //This is the function that the interupt calls 
{ 
 NbTopsFan++;  //This function measures the rising and falling edge of the hall effect sensors signal
}
void printDigits(int digits) {
   // utility function for digital clock display: prints preceding colon and leading 0
   Serial.print(":");
   if (digits < 10)
   Serial.print('0');
   Serial.print(digits);
}

void loop(void)
{ 
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures and flow rate...");
  sensor_inflow.requestTemperatures(); // Send the command to get temperatures
  sensor_outflow.requestTemperatures();
  sensor_f1.requestTemperatures();
  sensor_f2.requestTemperatures();
  NbTopsFan = 0;      //Set NbTops to 0 ready for calculations
  sei();            //Enables interrupts
  delay (1000);      //Wait 1 second
  cli();            //Disable interrupts
  Calc = (NbTopsFan * 60 / 7.5); //(Pulse frequency x 60) / 7.5Q, = flow rate in L/hour
  time = millis();
  Serial.println(time);
  Serial.print("Temperature InFlow is:");
  Serial.print(sensor_inflow.getTempCByIndex(0)); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  Serial.print(" = OutFlow is:");
  Serial.print(sensor_outflow.getTempCByIndex(0)); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  Serial.print(" = F1 is:");
  Serial.print(sensor_f1.getTempCByIndex(0)); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  Serial.print(" = F2 is:");
  Serial.print(sensor_f2.getTempCByIndex(0)); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  Serial.print(" = Water Flow Rate is: ");
  Serial.print (Calc);
  Serial.print (" L/hour");
  Serial.print("\n");
}
