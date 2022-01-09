/*  contains most of the variables. the system works mostly 
 *  with global variables. This allows for better timing of  
 *  tasks without the need to read the sensors again and again.
 */

bool is_day_time = true;

typedef struct {
  int WaterLevel;
  int TimeCheck;
  int Global;
  int Aeration_on;
  int Aeration_off;
}CycleDurations;
// en secondes
CycleDurations GraphUpdates = {
  20,     //   GraphUpdates.WaterLevel
  50,    //   GraphUpdates.TimeCheck
  10,      //   GraphUpdates.Global
  10,
  10*60*1000
};
CycleDurations Timestamps = {
  0,   //Timestamps.WaterLevel
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
  int morningtime;
  int noontime;
  int preReadDelay; //ms before reading analog input to ensure stabilised sensor voltage
}Things;

Things Variables = {
  500,  //  Variables.WaterLevelTreshold 11197.00 = in water 14296 = ot in water
  0,      //  Variables.WaterLevel
  9,
  28,
  400, //ms before reading analog input to ensure stabilised sensor voltage
};

// todo : remplacer les pin pwm par des pins de nodemcu
typedef struct{
  int Lights;
  int AirPump;
  int WaterPump;
  int ExtractionPump;
}PinOut;
// pins qui fonctionnent : [D7, D6, D5, D8
PinOut Relays = {
  D5,  // Lights;        110v  AC
  D6, // AirPump;     12v   DC
  D7,  // WaterPump;       12v   DC
  D8, // ExtractionPump 12v   DC
};

typedef struct{
  int Lights;
  int WaterPump;
  int AirPump;
  int ExtractionPump;
  int NightTime;
}States;
States SystemStates = {
  false, //  bool Lights;
  false, //  bool WaterPump;
  false, //  bool AirPump;
  false, //  ExtractionPump
  false // NightTime
};

typedef struct{int WaterLevelSensor;}SensorPinsIn;
SensorPinsIn Sensors = {
   A0   // 1 = A1   WaterLevelSensor;
};

typedef struct{int WaterLevelSensor;}SensorPinsOut;
SensorPinsOut SensorsOut = {
  D7, // handeled by other function
};

typedef struct{
  bool Aeration;
  bool WaterLevel;
  bool Lighting;
  bool extraction;
}Feature;
Feature FeatureAvailable = {
  true,
  true,
  true,
  true
};
Feature FeatureEnable = {
  true,
  true,
  true,
  true
};
Feature FeatureManualOverride = {
  true,
  true,
  true,
  true
};

Feature FeatureEnableAtNight = {
  false,
  false,
  false,
  false
};

typedef struct{
  double WaterLevelOffset;
  double WaterLevelFactor;
}calvars;
calvars Calibration = {
  0,              //  Calibration.WaterLevelOffset
  1,              //  Calibration.WaterLevelFactor
};
