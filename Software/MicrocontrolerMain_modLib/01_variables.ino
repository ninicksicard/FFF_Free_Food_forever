///////////////////////////////////////////////////////All variables////////////////////////////////////
typedef struct {
  int TempRead;
  int LightRead;
  int WaterLevel;
  int PopulationRead;
  int PhRead;
  int TimeCheck;
  int Global;
}CycleDurations;

// en secondes
CycleDurations GraphUpdates = {
  400,    //   GraphUpdates.TempRead
  200,    //   GraphUpdates.LightRead
  20,     //   GraphUpdates.WaterLevel
  80,   //   GraphUpdates.PopulationRead
  600,    //   GraphUpdates.PhRead   
  300,    //   GraphUpdates.TimeCheck
  10      //   
};

CycleDurations ControlCycles = {
  120,    //   ControlCycles.TempRead;
  90,     //   ControlCycles.LightRead;
  60,     //   ControlCycles.WaterLevel;
  20,    //   ControlCycles.PopulationRead;
  400,    //   ControlCycles.PhRead;
  300,    //   ControlCycles.TimeCheck;
  10      //   ControlCycles.Global
};

CycleDurations Timestamps = {
  millis(),   //Timestamps.TempRead
  millis(),   //Timestamps.LightRead
  millis(),   //Timestamps.WaterLevel
  millis(),   //Timestamps.PopulationRead
  millis(),   //Timestamps.PhRead
  millis(),   //Timestamps.TimeCheck
  millis()    //Timestamps.Global
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
  31, //  Temps.DayMax
  30, //  Temps.DayMin
  27, //  Temps.NightMax
  26, //  Temps.NightMin
  31, //  Temps.MaxTreshold
  30  //  Temps.MinTreshold
};

typedef struct{
  double WaterMaxTreshold;
  double WaterLevel;
  double DensityHighTreshold;
  double DensityLowTreshold;
  double Density;
}Things;

Things Variables = {
  25600,  //  Variables.WaterLevelTreshold
  0,  //  Variables.WaterLevel
  1,  //  Variables.DensityHighTreshold
  0,  //  Variables.DensityLowTreshold
  0   //  Variables.Density
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
  0,  // Lights;        120v  AC
  1,  // Heat;          120v  AC
  12, // WaterPump;     12v   DC
  22, //--- WaterWheel;    5v    DC  // using 22 for null value
  4,  // AirPump;       12v   DC
  11, // ExtractionPump 12v   DC
  13, // NutrientPump   12v   DC
  22, // ---Fan            12v   DC
  22,  // ---density sensor 5v    DC
  22
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
//  int Thermometer;
  int PhSensor;
  int WaterLevelSensor;
  int DensitySensor;
  int LightSensor;
}Pins;

Pins Sensors = {
      // using a nodemcu pin Thermometer;
   3,   // 3 = A3   PhSensor;
   1,   // 1 = A1   WaterLevelSensor;
   0,   // 0 = A0   DensitySensor;
   2    // 2 = A2   LightSensor;
};

Pins SensorsOut = {
  // none
  22, //always on
  D7, // handeled by other function
  D7, // handeled by other function 
  22 // always on
};

typedef struct{
  bool Aeration;
  bool PhRead;
  bool WaterWheel;
  bool WaterLevel;
  bool DensityRead;
  bool Heating;
  bool Lighting;
}Feature;

Feature FeatureEnable = {
  true,
  true,
  false,
  false,
  true,
  true,
  true
};

Feature FeatureAvailable = {
  true,
  true,
  false,
  false,
  true,
  true,
  true
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
  24.6461703497,  //  phoffet 
  -0.0012872774,  //  phfactor
  0,              //  DensityOffset
  1,              //  DensityFact
  0,              //  WaterLevelOffset
  1,              //  WaterLevelFactor
  0,              //  LightLevelOffset
  1               //  LightLevelFactor
};

float waterLevel;
