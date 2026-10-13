#include <HTTPClient.h>

const char* serverEndpoint = "192.168.0.179:6000";

const char* serverHost = "3.7.179.252";
const uint16_t serverPort = 6000;
const char* urlPath = "/predict_anomaly";

bool CheckForAnomaly(float data,char *stream) 
{
  
  bool anomalyAlert ;
  
  double rawScore ;

  DEBUG_PRINTF("Web: CheckForAnomaly entry\n");

  if (WiFi.status() != WL_CONNECTED) 
  {
    Serial.println("❌ Transmission failed: Not connected to Wi-Fi.");
    return false;
  }

  HTTPClient http;

  
  http.begin(serverHost, serverPort, urlPath);
  http.addHeader("Content-Type", "application/json");

  // 1. Pack variables into an explicit JSON container
  JsonDocument doc;
  doc["stream"]  = stream ;
  doc["data"]    =  data;
  
  String jsonPayload;
  serializeJson(doc, jsonPayload);
  DEBUG_PRINTF("\n📡 Transmitting JSON packet to server...%s \n",jsonPayload.c_str());
  
  // 2. Perform HTTP POST transmission
  int httpResponseCode = http.POST(jsonPayload);
  DEBUG_PRINTF("Response code from Server received %d \n", httpResponseCode) ;

  // 3. Evaluate Server Handshake Response

  anomalyAlert = false ; /* Default Value */

  if (httpResponseCode == 200) 
  {
    String responseString = http.getString();
    
    // Parse response fields back out
    JsonDocument responseDoc;
    deserializeJson(responseDoc, responseString);
    anomalyAlert = responseDoc["anomaly_detected"];
    rawScore   = responseDoc["raw_anomaly_score"];
    DEBUG_PRINTF("Response JSON Parsed anomaly status %d , %f\n", anomalyAlert,rawScore) ;
    http.end(); // Terminate network channel cleanly
  }

  DEBUG_PRINTF("Web: CheckForAnomaly exit\n");
  return anomalyAlert ;
}