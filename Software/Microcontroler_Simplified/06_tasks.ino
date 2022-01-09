void setup_routine() {
//  setup_relays();
  DayTimeVerif();
  mainTankLevel();
  TimeProfileManagement();
  
}


void water_level(){
  if ((millis() - Timestamps.WaterLevel) < (GraphUpdates.WaterLevel * 1000)) {return;}
  mainTankLevel();
  waterLevelControl();
  Timestamps.WaterLevel = millis();
}

void manage_clock(){
  if ((millis() - Timestamps.TimeCheck) < (GraphUpdates.TimeCheck * 1000)) {return;}
  TimeProfileManagement();
  DayTimeVerif();
  Timestamps.TimeCheck = millis();
}

void aeration(){
  // si la pompe est off
  if (!SystemStates.AirPump) {
    // verifie si ça fait 10 min qu'elle est off si oui turn it on
    if ((millis() - Timestamps.Aeration_off) > (10*60 * 1000)) {
      Aeration_on();
    }
    // si la pompe est on, verifie si elle est on depuis 20 secondes
  } else if ((millis() - Timestamps.Aeration_on) > (20 * 1000)) {
    Aeration_off();
  }
}

void routine() {
  water_level();
  manage_clock();
  aeration();
  UI_loop();
}
