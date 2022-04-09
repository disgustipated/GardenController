//Webpage setup and config
void setupWeb(){
  server.on("/tempstatus", HTTP_GET, checkSensors);
  server.on("/levelstatus", HTTP_GET, checkWaterLevels);
  server.on("/activatePump", HTTP_POST, activatePump);
  server.on("/lightsOn", HTTP_POST, lightRelayOn);
  server.on("/lightsOff", HTTP_POST, lightRelayOff);
  server.on("/relayState", HTTP_POST, relayState);
  // need to figure this out so i can handle partial file loads server.on("/",HTTP_GET,handleFileRead);
  server.onNotFound([]() {                              // If the client requests any URI
    if (!handleFileRead(server.uri()))                  // send it if it exists
      server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });
  server.begin();
}

String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  String servedPage ="";
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";   
  String contentType = getContentType(path);         
  if (SPIFFS.exists(path)) {                          
    File file = SPIFFS.open(path, "r");                
    if(path == "/index.html"){
      vector<String> v;
      while (file.available()) {
        v.push_back(file.readStringUntil('\n'));
      }
      file.close();
      for (String s : v) {
        servedPage += replaceContent(s);
      }
      server.send(200, contentType, servedPage);
    } else {
      size_t sent = server.streamFile(file, contentType);  
      file.close();
    }
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
}

String replaceContent(String incomingPage){
  //NAME
  incomingPage.replace("%DEVICENAME%", DEVICENAME);
  incomingPage.replace("%DEVICEPORT%", (String)statusPagePort);
  //IP
  incomingPage.replace("%CURRENTIP%", WiFi.localIP().toString());
  
  //currentTime
  String currentTime = ((String)hourFormat12() + ":" + minute());
  incomingPage.replace("%CURRENTTIME%", currentTime);
  
  //uptime
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  String uptimeValue = (String)hr + ":" + (String)(min % 60)  + ":" + (String)(sec % 60);
  incomingPage.replace("%UPTIME%", uptimeValue);
  
  return incomingPage;
}
