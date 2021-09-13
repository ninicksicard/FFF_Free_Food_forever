/*  contains most of the variables. the system works mostly 
 *  with global variables. This allows for better timing of  
 *  tasks without the need to read the sensors again and again.
 */


int morningtime = 7;
int noontime = 19;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int preReadDelay = 400; //ms before reading analog input to ensure stabilised sensor voltage

typedef struct {
  float TempRead;
  int LightRead;
  int WaterLevel;
  int PopulationRead;
  int PhRead;
  int TimeCheck;
  int Global;
  int Aeration_on;
  int Aeration_off;
}CycleDurations;

// en secondes
CycleDurations GraphUpdates = {
  60,    //   GraphUpdates.TempRead
  700,    //   GraphUpdates.LightRead
  20,     //   GraphUpdates.WaterLevel
  20,   //   GraphUpdates.PopulationRead
  120,    //   GraphUpdates.PhRead   
  50,    //   GraphUpdates.TimeCheck
  10,      //   GraphUpdates.Global
  10,
  10*60*1000
};

CycleDurations ControlCycles = {
  60,    //   ControlCycles.TempRead;
  700,     //   ControlCycles.LightRead;
  20,     //   ControlCycles.WaterLevel;
  20,    //   ControlCycles.PopulationRead;
  120,    //   ControlCycles.PhRead;
  50,    //   ControlCycles.TimeCheck;
  10,      //   ControlCycles.Global
  10,
  10
};

CycleDurations Timestamps = {
  0,   //Timestamps.TempRead
  0,   //Timestamps.LightRead
  0,   //Timestamps.WaterLevel
  0,   //Timestamps.PopulationRead
  0,   //Timestamps.PhRead
  0,   //Timestamps.TimeCheck
  0,    //Timestamps.Global
  0,
  -10*60*1000
};

CycleDurations LastValue = {
  0, 
  0,
  0,
  0,//LastValue.PopulationRead
  0,
  0,
  0
};

//todo add the ready to harvest parameter
typedef struct{
  float DayMax;
  float DayMin;
  float NightMax;
  float NightMin;
  float MaxTreshold;
  float MinTreshold;
} Limits;

Limits Temps = {
  36, //  Temps.DayMax
  35, //  Temps.DayMin
  33, //  Temps.NightMax
  32, //  Temps.NightMin
  36, //  Temps.MaxTreshold
  35  //  Temps.MinTreshold
};

typedef struct{
  float WaterMaxTreshold;
  float WaterLevel;
  float DensityHighTreshold;
  float DensityLowTreshold;
  float Density;
}Things;

Things Variables = {
  12000,  //  Variables.WaterLevelTreshold 11197.00 = in water 14296 = ot in water
  0,      //  Variables.WaterLevel
  26500,  //  Variables.DensityHighTreshold if density lower than this, extracton
  26600,      //  Variables.DensityLowTreshold
  0       //  Variables.Density
};

typedef struct{
  int Lights;
  int Heat;
  int WaterPump;
  int WaterWheel;
  int AirPump;
  int ExtractionPump;
  int NutrientPump;
  int Fan;
  int DensitySensor;
  int NightTime;
}PinOut;


/* PWM pin | Relay number
 * 0 | 1
 * 1 | 3
 * 2 | 5
 * 3 | 7
 * 4 | 9
 * 5 | 11
 * 6 | 13
 * 7 | 15
 * 8 | 2
 * 9 | 4
 * 10| 6
 * 11| 8
 * 12| 10
 * 13| 12
 * 14| 14
 * 15| 16
 */

PinOut Relays = {
  0,  // Lights;        110v  AC
  8,  // Heat;          110v  AC
  12, // WaterPump;     12v   DC
  22, //--- WaterWheel;    5v    DC  // using 22 for null value
  4,  // AirPump;       12v   DC
  5, // ExtractionPump 12v   DC
  13, // NutrientPump   12v   DC
  22, // ---Fan            12v   DC
  22,  // ---density sensor 5v    DC
  22
  // water level and density sensor
  // 7 output pins needed 
};

PinOut SystemStates = {
  false, //  bool Lights;
  false, //  bool Heat; // true = daytimeheat , false = nightime heat
  false, //  bool WaterPump;
  false, //  bool WaterWheel;
  false, //  bool AirPump;
  false, //  ExtractionPump   
  false, //  NutrientPump     
  false, //  Fan            
  false,  // density sensor 
  false
};

typedef struct{
  int Thermometer;
  int PhSensor;
  int WaterLevelSensor;
  int DensitySensor;
  int LightSensor;
}Pins;

Pins Sensors = {
   14,  // using a nodemcu pin Thermometer;
   3,   // 3 = A3   PhSensor;
   1,   // 1 = A1   WaterLevelSensor;
   0,   // 0 = A0   DensitySensor;
   2    // 2 = A2   LightSensor;
};

Pins SensorsOut = {
  22, // none
  22, //always on
  D7, // handeled by other function
  D7, // handeled by other function 
  22  // always on
};

typedef struct{
  bool Aeration;
  bool PhRead;
  bool WaterWheel;
  bool WaterLevel;
  bool DensityRead;
  bool Heating;
  bool Lighting;
  bool extraction;
}Feature;

Feature FeatureEnable = {
  true,
  false,
  false,
  true,
  true,
  true,
  true,
  true,
};

Feature FeatureAvailable = {
  true,
  false,
  false,
  true,
  true,
  true,
  true,
  false
};

typedef struct{
  double phOffset;
  double phFactor;
  double DensityOffset;
  double DensityFactor;
  double WaterLevelOffset;
  double WaterLevelFactor;
  double LightLevelOffset;
  double LightLevelFactor;
}calvars;

calvars Calibration = {
  24.6461703497,  //  Calibration.phOffset       todo : retrieve from memory
  -0.0012872774,  //  Calibration.phFactor      todo : retrieve from memory
  0,              //  Calibration.DensityOffset
  1,              //  Calibration.DensityFactor
  0,              //  Calibration.WaterLevelOffset
  1,              //  Calibration.WaterLevelFactor
  0,              //  Calibration.LightLevelOffset
  1               //  Calibration.LightLevelFactor
};

float waterLevel;


typedef struct{
  float calibrate_7;
  float calibrate_4;
  float calibrate_10;
  float value;
}PhSensorVariables;


PhSensorVariables Ph_Sensor_Variables = {
  0,
  0,
  0,
  -1
};


//void SaveVariables(){
//  eeWriteInt(3, val);
//  eeWriteInt(3, val);
//  eeWriteInt(3, val);
//  eeWriteInt(3, val);
//  eeWriteInt(3, val);
//
//}
//
//void LoadVariables(){
//  = eeGetInt(3)
//  = eeGetInt(3)
//  = eeGetInt(3)
//  = eeGetInt(3)
//  = eeGetInt(3)
//  = eeGetInt(3)
//  
//}
