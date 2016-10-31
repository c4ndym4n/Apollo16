
/*
 * lampje moet aan & uit   1-0   [Aan / Uit]
 * lampje moet knipperen 1-0-1-0-1-0 [Firmware Update]
 * lampje moet knipperen 1-1-1-0-0-0 [AP Mode]
 */

void StateOff() {
  Serial.println("Goodbye!");
  ledState = 4;
}

void StateIdle() {
  
  Serial.println("IDLE .. just look around!");
  ledState = 0;
  
  // Rotor Has to stop
  digitalWrite(PIN_ROTOR, LOW);  
}
void StateOn() {
  
  Serial.println("Set State to On");
  ledState = 1;
  
  // Rotor
  digitalWrite(PIN_ROTOR, HIGH);   
}

void StateFirmware() {
  
  // while firmware update
  Serial.println("MODE: Firmware");
  ledState = 3;
  digitalWrite(PIN_ROTOR, LOW); 
  FirmUpdater();
}

void StateAP() {
  // while acting as access point
  SetupServer();
      
   // Set Ligths to StateAP
   ledState = 2;
     
   // Start function AP
   LoopRestServer();
}

