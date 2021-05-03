//This code contain everything to control the whole system. 

// import and setup ads1115 for more analog inputs.

// adding reading delay

int preReadDelay = 20; //ms before reading analog input to ensure stabilised sensor voltage 

// should allow for wifi control





/////////////////////// general purpos sensor read //////////////////////////////
float sensorRead(int sensor){
  //sensor is a list of 2 ints, [power pin, read pin]
  digitalWrite(sensor[0], HIGH);
  delay(preReadDelay);
  int16_t sensor_value = ads.readADC_SingleEnded(sensor[1]);
  digitalWrite(sensor[0], Low);
  return sensor_value;
}




////////////////////// general purpose relay switch control /////////////////////
void switchRelay(int relay, int state){
  // code this according to the right relay board choosen(choose board once number of channel is known)
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
-------------------------------------------------------------------------------------------------------------------------------

/*  Here are all the modules that sould be in this code
 *  Light_management
 *  Gas_and_pressure_management
 *  Aeration
 *  Fluids_management
 *  population_management
 *  Extraction_management
 *  Nutrient_Management
 *  PH_Management
 *  Temperature_management
 */




// ------------------------------------------- Light management -------------------------------------------
/* Light level sensor
 * Light temperature sensor
 * Light Switch
 * Light Logs?
 */



////////////////// light level sensor /////////////////
/*  photoresistor power on
 *  analog read of photoresistor
 *  photoresistor power off
 *  adjust value with calibration data
 *  log or save temperature value
 */
int lightCalFact = 1;
int lightCalOffset = 0 ;
int Light_Sensor = [x,y]; // x:photoresistor powerpin y:photoresistor analog input pin number(on ads1115)

float lightSens(){
  float light_level;
  light_level = sensorRead(Light_Sensor) * LightCalFact + LightCalOffset
  return light_level
}



/////////////////// Light Switch /////////////////
int Light_Switch = x; //x:relay number or pin 

void lightSwitch_on(){
  switchRelay(Light_Switch,HIGH);
}

void lightSwitch_off(){
  switchRelay(Light_Switch,LOW);
}



// ---------------------------------- Pressure Management ---------------------------------------
/*  system parameter (open/sealed system)
 *  air pressure sensor
 *  air compressor/fan
 *  pressure log and alarms (to notify for leaks)
 */

// -------------------------------------- Aeration ----------------------------------------------
/* control of the pump for bubbler stone, spray or other.
 */


/////////////////////////////////////
int Aeration_Pump = x //x:relay number for aeration pump 

void Aeration_on(){
  switchRelay(Aeration_Pump, HIGH);
}

void Aeration_off(){
  switchRelay(Aeration_Pump, LOW);
}




// ------------------------------------ Fluids management --------------------------------------
/*  Main tank water level sensor
 *  recycled tank water level sensor
 *  tap water input valve
 *  recycled water pump (from extraction process)
 */


/////////////Main tank water level sensor/////////////
int mainLevelCalFact = 1;
int mainLevelCalOffset = 0;
int main_Level_Sensor = [x,y]; // x: sensor power pin, y:sensor analog in pin

float mainTankLevel(){
  float Water_Level;
  Water_Level = sensorRead(Main_Tank_Level_Sensor) * mainLevelCalFact + mainLevelCalOffset;
  return Water_Level;
 }



/////////////secondary tank water level sensor/////////
int secondaryLevelCalFact = 1;
int secondaryLevelCalOffset = 0;
int secondary_Level_Sensor = [x,y]; // x: sensor power pin, y:sensor analog in pin

float secondaryTankLevel(){
  float Water_Level;
  Water_Level = sensorRead(secondary_Tank_Level_Sensor) * secondaryLevelCalFact + secondaryLevelCalOffset;
  return Water_Level;
 }



// ---------------------------------------- population management ---------------------------------
/*  photodensity sensor
 */
////////////////////////////////////////////////////////
int densityPhotoresistor = [x,y]; // x:photoresistor power pin y:photoresistor analog input pin

float populationDensity(){
  float density;
  density = sensorRead(densityPhotoresistor);
  return density
}


// --------------------------------------------- Extraction ----------------------------------------
/*  filter convayor motor control
 *  vacuum switch
 *  extraction pump/valve
 */

int extractionPump = x; //x:extraction pump relay switch
int vacuumPump = x; // x:vaccume pump relay number 

void extraction_on(){ // extraction time is in seconds
  switchRelay(extractionPump,HIGH);
}

void extraction_off(){
  switchRelay(extractionPump,LOW);
}

void vaccumePump_on(){
  switchRelay(vacuumPump,HIGH);
}

void vacuumPump_off(){
  switchRelay(vacuumPump,LOW);
}

// ------------------------------------------Nutrient management ------------------------------------
/*  solid fertiliser dosing actuator(s)(archimedes screw motor)
 *  water input pump/valve
 *  magnetic stiring motor
 *  nutrient output pump/valve
 */
//right now, considering a premixed fluid that is added as the algea is extracted

int peristalticPump = x //x: peristaltic pump power pins
int stirerMotor = x //x: stiring motor power pin relay

void nutrienPump_on(){
  switchRelay(peristalticPump,HIGH);
}

void nutrientPump_off(){
  switchRelay(peristalticPump,LOW);
}

// -------------------------------------------- Ph management -------------------------------------- 
/*  ph sensor
 *  ph dosing(screw motor)
 */

int phSensor = [x,y] //x: main power relay pin or number y: ph analog input  
int phCalFact=32;
int phCalOffset1=0.82;
int phCalOffset2=7;

float ph_sens(){
  float ph;
  ph = (phCalOffset1-sensorRead(phSensor))*phCalFact+phCalOffset2;
  return ph;
}

// ----------------------------------------  Temperature management --------------------------------
/*  thermometers input
 *  thermal plate control
 *  fan control
 *  temperature logs/alert
 */

 
///////////////// thermometers input ////////////
int tempCalFact = 1;
int tempCalOffset = 0 ;
int thermometer = [x,y]; // x:thermometer power pin, y:thermometer analog input pin

float temperature_sens(int sensor){
  float temp;
  temp = sensorRead(thermometer) * tempCalFact + tempCalOffset;
  return temp;
}

//////////////// thermal plate control ////////////

//////////////// fan control /////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
------------------------------------------------------------------------------------------------------------------------------

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
