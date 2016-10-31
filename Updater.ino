

void FirmUpdater(char* FIRM_SSID, char* FIRM_PASSWD, char* URL) {

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }
    

    WiFiMulti.addAP(FIRM_SSID, FIRM_PASSWD);

    FirmUpdaterLoop(String(URL));
}

void FirmUpdaterLoop(String URL) {
  
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        t_httpUpdate_return ret = ESPhttpUpdate.update(URL + "/file.bin");
       

        switch(ret) {
            case HTTP_UPDATE_FAILED:
                USE_SERIAL.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                break;

            case HTTP_UPDATE_NO_UPDATES:
                USE_SERIAL.println("HTTP_UPDATE_NO_UPDATES");
                break;

            case HTTP_UPDATE_OK:
                USE_SERIAL.println("HTTP_UPDATE_OK");
                break;
        }
    }
}

