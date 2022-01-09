//todo :  ajouter un setting cleaning pour pomper du nettoyant dans les tubes


////////////////////// general purpose relay switch control /////////////////////
//void switchRelay(uint8_t relay, uint8_t state) {
//  /* take in the pwm pin to activate as "relay"
//      and switch its value to fit the "state" recieved
//  */
//
//  if (relay == 22) {
//    Serialprintln("relay was set to 22 and will not be actuated");
//    return;
//  }
//  Serialprint("relay number ");
//  Serialprint(String(relay));
//  Serialprint(" turned ");
//  Serialprintln(String(state));
//  digitalWrite(relay, state);
//}

void setup_relays() {
  /* go thrue all pins of te pwm to reset their state
     syncing them with the system's state variables */
//  for (int relay = 0; relay <= 16; relay++) {
//    digitalWrite(relay, HIGH);
//    digitalWrite(relay, LOW);
//  }
}

//////////////////////////////////// Light Switch//////////////////////////////////
bool on_state = HIGH;
bool off_state = LOW;

void lightSwitch_on() {
  /*  Turn Lights on, send a status update via serialprint
      and change the light state in system memory */
  if (!FeatureAvailable.Lighting) {lightSwitch_off();return;}
  Serial.println("Light Availlable");
  if (!FeatureEnable.Lighting) {lightSwitch_off();return;}
  Serial.println("Light Enabled");
  if (!is_day_time){ if (!FeatureEnableAtNight.Lighting){lightSwitch_off();return;}}
  Serial.println("daytime or available at night");
  if (SystemStates.Lights){return;}
  Serial.println("light are off need to be on");
  
  digitalWrite(Relays.Lights, on_state);
  SystemStates.Lights = true;
  
  Serial.print("pin num : ");
  Serial.print(Relays.Lights);
  Serial.print(" Light switched on : ");
  Serial.println(on_state);
}

void lightSwitch_off() {
  /*  Turn Lights off, send a status update via serialprint
      and change the light state in system memory
  */
  if (!SystemStates.Lights){return;}
  digitalWrite(Relays.Lights, off_state);
  SystemStates.Lights = false;
  
  Serial.print("pin num : ");
  Serial.print(Relays.Lights);
  Serial.print(" Light switched off : ");
  Serial.println(off_state);
}


//////////////////////////////////// Aeration///////////////////////////////////////
void Aeration_on()  {
  /*  this code switch the air pump on,
      it also print a text update and change the pump's
      state in the system memory
  */
  
  if (!FeatureAvailable.Aeration) {Aeration_off();return;}
  if (!FeatureEnable.Aeration) {Aeration_off();return;}
  if (!is_day_time){ if (!FeatureEnableAtNight.Aeration){Aeration_off();return;}}
  if (SystemStates.AirPump){return;}
  
  Timestamps.Aeration_on = millis();
  digitalWrite(Relays.AirPump, on_state);
  SystemStates.AirPump = true;

  Serial.print("pin num : ");
  Serial.print(Relays.AirPump);
  Serial.print(" Aeration switched on : ");
  Serial.println(on_state);
}

void Aeration_off() {
  /*  this code switch the air pump off,
      it also print a text update and change the pump's
      state in the system memory
  */

  if (!SystemStates.AirPump){return;}
  
  Timestamps.Aeration_off = millis();
  digitalWrite(Relays.AirPump, off_state);
  SystemStates.AirPump = false;

  Serial.print("pin num : ");
  Serial.print(Relays.AirPump);
  Serial.print(" Aeration switched off : ");
  Serial.println(off_state);
}

/////////////////////////////////////water level control///////////////////////////
// Direct water pump actions
void waterPump_on() {
  /*  this code switch the water pump on,
      it also print a text update and change the pump's
      state in the system memory
  */
 
  if (!FeatureAvailable.WaterLevel) {waterPump_off();return;}
  if (!FeatureEnable.WaterLevel) {waterPump_off();return;}
  if (!(Variables.WaterLevel > Variables.WaterMaxTreshold)){waterPump_off(); return;}
  if (!is_day_time){ if (!FeatureEnableAtNight.WaterLevel){waterPump_off(); return;}}
  if (SystemStates.WaterPump){return;}
  
  digitalWrite(Relays.WaterPump, on_state);
  SystemStates.WaterPump = true;

  Serial.print("pin num : ");
  Serial.print(Relays.WaterPump);
  Serial.print(" WaterPump switched on : ");
  Serial.println(on_state);
}

void waterPump_off() {
  /*  this code switch the water pump off,
      it also print a text update and change the pump's
      state in the system memory
  */
  if (!SystemStates.WaterPump){return;}
  
  digitalWrite(Relays.WaterPump, off_state);
  SystemStates.WaterPump = false;

  Serial.print("pin num : ");
  Serial.print(Relays.WaterPump);
  Serial.print(" WaterPump switched off : ");
  Serial.println(off_state);
}

// water pump control cycle
void waterLevelControl() {
  /*  this code compare the reviously measured water level senssor value
      it turns the water pump on or off
      todo : check wats up with the return 1
  */
  
  Serialprint("comparison : ");
  Serialprintln(String(Variables.WaterLevel > Variables.WaterMaxTreshold) );
  Serialprint("waterlevel : ");
  Serialprintln(String(Variables.WaterLevel));
  Serialprint("treshold value : ");
  Serialprintln(String(Variables.WaterMaxTreshold));
  
  waterPump_on();
}

//////////////////////////////////// Extraction ////////////////////////////////////
void extraction_on() { // extraction time is in seconds
  /*  this code switch the extraction pump on,
      it also print a text update and change the pump's
      state in the system memory */

  if (!FeatureAvailable.extraction) {extraction_off();return;}
  if (!FeatureEnable.extraction) {extraction_off();return;}
  if (!is_day_time){ if (!FeatureEnableAtNight.extraction){extraction_off();return;}}
  if (SystemStates.ExtractionPump){return;}

  digitalWrite(Relays.ExtractionPump, on_state);
  SystemStates.ExtractionPump = true;

  Serial.print("pin num : ");
  Serial.print(Relays.ExtractionPump);
  Serial.print(" ExtractionPump switched on : ");
  Serial.println(on_state); 
}

void extraction_off() {
  /*  this code switch the Extraction pump off,
      it also print a text update and change the pump's
      state in the system memory */
  if (!SystemStates.ExtractionPump){return;}
  
  digitalWrite(Relays.ExtractionPump, off_state);
  SystemStates.ExtractionPump = false;

  Serial.print("pin num : ");
  Serial.print(Relays.ExtractionPump);
  Serial.print(" ExtractionPump switched off : ");
  Serial.println(off_state);
}
