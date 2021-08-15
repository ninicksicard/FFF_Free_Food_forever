////////////////////// general purpose relay switch control /////////////////////
void switchRelay(int relay, uint8_t state) {
  /* take in the pwm pin to activate as "relay" 
   *  and switch its value to fit the "state" recieved
   */
   
  if (relay == 22){
    Serialprintln("relay was set to 22 and will not be actuated");
    return;
  }
  Serialprint("relay number ");
  Serialprint(String(relay));
  Serialprint(" turned ");
  Serialprintln(String(state));
  if (state == HIGH) {
    pwm.setPWM(relay, 0, 4096);
  } else if (state == LOW) {
    pwm.setPWM(relay, 4096, 0);
  }
}

void setup_relays() {
  /* go thrue all pins of te pwm to reset their state 
   * syncing them with the system's state variables */
  for (int relay = 0; relay <= 16; relay++) {
    switchRelay(relay, HIGH);
    switchRelay(relay, LOW);
  }
}

/////////////////// Light Switch /////////////////

void lightSwitch_on() {
  /*  Turn Lights on, send a status update via serialprint
   *  and change the light state in system memory */
  if (FeatureEnable.Lighting){
     switchRelay(Relays.Lights, HIGH);
     Serial.println("light switched on");
     SystemStates.Lights = true;
  }
}

void lightSwitch_off() {
  /*  Turn Lights off, send a status update via serialprint
   *  and change the light state in system memory
   */
  switchRelay(Relays.Lights, LOW);
  Serial.println("light switched off");
  SystemStates.Lights = false;
}

//void LightCycleManager() { //todo : move to time cycle management
//  /* this code first retrieve the curent hour as a single number
//   *  it then compare the current hour with the time interval that define 
//   *  the night time. 
//   *  If the light is off and should be on, it switch it on
//   *  If the light is on and should be off, it switch it off
//   */
//  int nowhour = GetHour();
//  if (morningtime < nowhour && nowhour < noontime && !SystemStates.Lights) {
//    lightSwitch_on();
//  }
//  else if (!(morningtime < nowhour && nowhour < noontime) && SystemStates.Lights) {
//    lightSwitch_off();
//  }
//}



// -------------------------------------- Aeration ----------------------------------------------

void Aeration_on()  {
  /*  this code switch the air pump on, 
   *  it also print a text update and change the pump's 
   *  state in the system memory
   */
  if (FeatureEnable.Aeration && FeatureAvailable.Aeration){
    switchRelay(Relays.AirPump, HIGH);
    Serial.println("AirPump On");
    SystemStates.AirPump = true;
  }
}
void Aeration_off() {
  /*  this code switch the air pump off, 
   *  it also print a text update and change the pump's 
   *  state in the system memory
   */
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

// todo : add a valve control to fill up the clean water bucket 
// it will also need another sensor(monitoring entire water level , a timer since the filling to allow chlorine to dissipate


/////////////water level control////////

// Direct water pump actions
void waterPump_on() {
  /*  this code switch the water pump on, 
   *  it also print a text update and change the pump's 
   *  state in the system memory
   */
  if (FeatureEnable.WaterLevel && FeatureAvailable.WaterLevel) {
    switchRelay(Relays.WaterPump, HIGH);
    Serial.println("water Pump turned on");
    SystemStates.WaterPump = true;
  }
  
}

void waterPump_off() {
  /*  this code switch the water pump off, 
   *  it also print a text update and change the pump's 
   *  state in the system memory
   */
  switchRelay(Relays.WaterPump, LOW);
  Serial.println("water Pump turned off");
  SystemStates.WaterPump = false;
}


// water pump control cycle


int waterLevelControl() {
  /*  this code compare the reviously measured water level senssor value 
   *  it turns the water pump on or off 
   *  todo : check wats up with the return 1
   */
    Serialprint("comparison : ");
    Serialprintln(String(Variables.WaterLevel > Variables.WaterMaxTreshold) );
    Serialprint("waterlevel : ");
    Serialprintln(String(Variables.WaterLevel));
    Serialprint("treshold value : ");
    Serialprintln(String(Variables.WaterMaxTreshold));
  if (Variables.WaterLevel > Variables.WaterMaxTreshold ) {
    if (!SystemStates.WaterPump){
       waterPump_on();
    }
    return 1;

  } else if (Variables.WaterLevel < Variables.WaterMaxTreshold) {
    if (SystemStates.WaterPump){
      waterPump_off();
    }
    return 0;
  }
}


// --------------------------------------------- Extraction ----------------------------------------

void extraction_on() { // extraction time is in seconds
  /*  this code switch the extraction pump on, 
   *  it also print a text update and change the pump's 
   *  state in the system memory */
  switchRelay(Relays.ExtractionPump, HIGH);
  Serial.println("Extraction pump turned on");
  SystemStates.ExtractionPump = true;
}

void extraction_off() {
  /*  this code switch the Extraction pump off, 
   *  it also print a text update and change the pump's 
   *  state in the system memory */
  switchRelay(Relays.ExtractionPump, LOW);
  Serial.println("Extraction pump turned off");
  SystemStates.ExtractionPump = false;
}

// ------------------------------------------Nutrient management ------------------------------------
/*  solid fertiliser dosing actuator(s)(archimedes screw motor)
    water input pump/valve
    magnetic stiring motor
    nutrient output pump/valve
*/

void nutrienPump_on() {
  /*  this code switch the nutrient pump on, 
   *  it also print a text update and change the pump's 
   *  state in the system memory */
  switchRelay(Relays.NutrientPump, HIGH);
  Serial.println("nutrient pump turned on");
  SystemStates.NutrientPump = true;
}

void nutrientPump_off() {
  /*  this code switch the Extraction pump off, 
   *  it also print a text update and change the pump's 
   *  state in the system memory */
  switchRelay(Relays.NutrientPump, LOW);
  Serial.println("nutrient pump turned off");
  SystemStates.NutrientPump = false;
}

// ---------------------------------------waterlevel --------------------------------

// ----------------------------------------  Temperature management --------------------------------
/*  thermometers input
    thermal plate control
    fan control
    temperature logs/alert
*/

//////////////// thermal plate control ////////////

void heating_on() {
  /*  this code switch the heating element on, 
   *  it also print a text update and change the pump's 
   *  state in the system memory
   */
  if (FeatureEnable.Heating){
    if (!SystemStates.Heat) {
      switchRelay(Relays.Heat, HIGH);
      Serial.println("Heat turned on");
      SystemStates.Heat = true;
    }
  }
}

void heating_off() {
  /*  this code switch the heating element off, 
   *  it also print a text update and change the pump's 
   *  state in the system memory
   */
  if (SystemStates.Heat) {
    switchRelay(Relays.Heat, LOW);
    Serial.println("Heat turned off");
    SystemStates.Heat = false;
  }
}
