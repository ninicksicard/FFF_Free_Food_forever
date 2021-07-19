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
  20,    //   GraphUpdates.WaterLevel
  200 ,   //   GraphUpdates.PopulationRead
  400,    //   GraphUpdates.PhRead   
  300,   //   GraphUpdates.TimeCheck
  10      //   
};

CycleDurations ControlCycles = {
  120,    //   ControlCycles.TempRead;
  200,    //   ControlCycles.LightRead;
  60,    //   ControlCycles.WaterLevel;
  200,   //   ControlCycles.PopulationRead;
  400,    //   ControlCycles.PhRead;
  300,   //   ControlCycles.TimeCheck;
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

typedef struct{
  float DayMax;
  float DayMin;
  float NightMax;
  float NightMin;
  float MinTreshold;
  float MaxTreshold;
} Temperatures;

Temperatures Temps = {
  31, //  Temps.DayMax
  30, //  Temps.DayMin
  27, //  Temps.NightMax
  26, //  Temps.NightMin
  30, //  Temps.MinTreshold
  31  //  Temps.MaxTreshold
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
}PinOut;

PinOut Relays = {
  0,  // Lights;        120v  AC
  2,  // Heat;          120v  AC
  8,  // WaterPump;     12v   DC
  14, // WaterWheel;    5v    DC
  13, // AirPump;       12v   DC
  12, // ExtractionPump 12v   DC
  1,  // NutrientPump   12v   DC
  5,  // Fan            12v   DC
  10  // density sensor 5v    DC
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
  false  // density sensor 
};

typedef struct{
//  int Thermometer;
  int PhSensor;
  int WaterLevelSensor;
  int DensitySensor;
  int LightSensor;
}PinIn;

PinIn Sensors = {
      // using a nodemcu pin Thermometer;
   3,   // 3 = A3   PhSensor;
   0,   // 0 = A0   WaterLevelSensor;
   1,   // 1 = A1   DensitySensor;
   2    // 2 = A2   LightSensor;
};

float waterLevel;
