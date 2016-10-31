void buttonAction() {
  
  // read the pushbutton input pin:
  buttonState = digitalRead(PIN_BUTTON);
  
  /*
   * Button Logic
   */
 
  if (buttonState == HIGH) {
      startPressed = millis();
//      Serial.println(timeHold);
      pressed = false;
  }
  if (buttonState == LOW) {
    Serial.println(timeHold);
    Serial.println("TRUE");
    pressed = true;
  }

  if (pressed == true) {
    timeHold++;
//    seconds_true++;
//    Serial.println(timeHold);
    delay(900);                 // 1 second per hold
  }
  if (pressed == false) {
//    Serial.println(" >>>> Timehold: " + String(timeHold));
    seconds_true = timeHold;
    timeHold = 0;
//    seconds_true = 0;
//    delay(5000);
  }



  if (seconds_true == 1 && seconds_true < 2 && (ledState == 0 || ledState == 1)) {
    if (ledState == 0) {
      ledState = 1;
    } else {
      ledState = 0;
    }
  }

  if (seconds_true > 1 && seconds_true < 4 && (ledState == 4)) {
    StateOn();
  }
  
  if (seconds_true >= firmwareActivation && seconds_true < AccessPointActivation) {
    Serial.println("#####################");
    Serial.println("Firmware updates ?");


    StateFirmware();
    
  }

  if (seconds_true >= AccessPointActivation && seconds_true < deactivation) {
//    CreateAccessPoint(AccessPointLifetime);
      SetupServer();

      Serial.println("############# Server Started - Now go KOEKKOEK !");
      
      // Set Ligths to StateAP
      ledState = 2;
      
      // Start function AP
      LoopRestServer();

      // When finnished AP Mode set LED to IDLE;
  }


  if (seconds_true > deactivation) {
    // Put the Device OFF
    //    ledState = 4;
    StateOff();
  }


//  if (seconds_true >= 5 && seconds_true < AccessPointActivation) {
////    HashText(UID);
//    StartMQTT();
//    
//  }

}

