bool Log = false;
void Serialprint(String toprint){
  if (Log){
    Serial.print(toprint);
  }
}
void Serialprintln(String toprint){
  if (Log){
    Serial.println(toprint);
  }
}
