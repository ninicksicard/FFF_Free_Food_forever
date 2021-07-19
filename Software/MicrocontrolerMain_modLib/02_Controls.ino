////////////////////// general purpose relay switch control /////////////////////
void switchRelay(int relay, uint8_t state) {
  // code this according to the right relay board choosen(choose board once number of channel is known)
  if (state == HIGH) {
    pwm.setPWM(relay, 0, 4096);
  } else if (state == LOW) {
    pwm.setPWM(relay, 4096, 0);
  }
}



void setup_relays() {
  for (int relay = 0; relay <= 16; relay++) {
    switchRelay(relay, HIGH);
    switchRelay(relay, LOW);
  }
}

/////////////////// Light Switch /////////////////
int Light_Switch = 0; //x:relay number or pin

void lightSwitch_on() {
  switchRelay(Light_Switch, HIGH);
  Serial.println("light switched on");
  SystemStates.Lights = true;
}

void lightSwitch_off() {
  switchRelay(Light_Switch, LOW);
  Serial.println("light switched off");
  SystemStates.Lights = false;
}

void LightCycleManager() { //todo : move to time cycle management
  int nowhour = GetHour();
  if (morningtime < nowhour && nowhour < noontime && !SystemStates.Lights) {
    lightSwitch_on();

  }
  else if (!(morningtime < nowhour && nowhour < noontime) && SystemStates.Lights) {
    lightSwitch_off();

  }
}



// -------------------------------------- Aeration ----------------------------------------------

void Aeration_on()  {

  switchRelay(Relays.AirPump, HIGH);
  Serial.println("AirPump On");
  SystemStates.AirPump = true;
}

void Aeration_off() {
  switchRelay(Relays.AirPump, LOW);
  Serial.println("AirPump off");
  SystemStates.AirPump = false;
}




// ------------------------------------ Fluids management --------------------------------------
/*  Main tank water level sensor
    recycled tank water level sensor
    tap water input valve
    recycled water pump (from extraction process)
*/



/////////////water level control////////

// relay number for the water pump





// Direct water pump actions
void waterPump_on() {
  switchRelay(Relays.WaterPump, HIGH);
  Serial.println("water Pump turned on");
  SystemStates.WaterPump = true;
}

void waterPump_off() {
  switchRelay(Relays.WaterPump, LOW);
  Serial.println("water Pump turned off");
  SystemStates.WaterPump = false;
}



// water pump control cycle
// todo : duplicate this code for the sensor and pmp of the second water tank

float waterLevelTreshold = 0.05;

int waterLevelControl() {
  if (waterLevel < waterLevelTreshold && !SystemStates.WaterPump) {
    waterPump_on();
    return 1;

  } else if (waterLevel >= waterLevelTreshold && SystemStates.WaterPump) {
    waterPump_off();
    return 0;
  }
}




// --------------------------------------------- Extraction ----------------------------------------

void extraction_on() { // extraction time is in seconds
  switchRelay(Relays.ExtractionPump, HIGH);
}

void extraction_off() {
  switchRelay(Relays.ExtractionPump, LOW);
}

// ------------------------------------------Nutrient management ------------------------------------
/*  solid fertiliser dosing actuator(s)(archimedes screw motor)
    water input pump/valve
    magnetic stiring motor
    nutrient output pump/valve
*/

void nutrienPump_on() {
  switchRelay(Relays.NutrientPump, HIGH);
}

void nutrientPump_off() {
  switchRelay(Relays.NutrientPump, LOW);
}

// ---------------------------------------waterlevel --------------------------------









// ----------------------------------------  Temperature management --------------------------------
/*  thermometers input
    thermal plate control
    fan control
    temperature logs/alert
*/

//////////////// thermal plate control ////////////

int heatingRelay = 2; // x: relay number for heating plate

void heating_on() {
  if (!SystemStates.Heat) {
    switchRelay(Relays.Heat, HIGH);
    Serial.println("Heat turned on");
    SystemStates.Heat = true;
  }
}

void heating_off() {
  if (SystemStates.Heat) {
    switchRelay(Relays.Heat, LOW);
    Serial.println("Heat turned off");
    SystemStates.Heat = false;
  }
}


//////////////// fan control /////////////////////

void fan_on() {
  if (SystemStates.Fan = false) {
    switchRelay(Relays.Fan, HIGH);
    Serial.println("fan turned on");
    SystemStates.Fan = true;
  }
}

void fan_off() {
  if (SystemStates.Fan = true) {
    switchRelay(Relays.Fan, LOW);
    Serial.println("Fan turned off");
    SystemStates.Fan = false;
  }
}
