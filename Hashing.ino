
String HashText(String text) {
    Serial.print("SHA1:");
    Serial.println(sha1(text));
    
    return sha1(text);
}

