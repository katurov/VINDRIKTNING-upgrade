/*
 *
 * IKEA VINDRIKTNING WIRING
 *   ISPDA  - NC
 *   REST   - TX of sensor connect to RX of ESP8285 (it is 5v tolerant)
 *   ISPCLK - NC
 *   5v     - VCC 5V connect to IN of LD1117XX33 (and OUT 3v3 to VCC pin of ESP8285)
 *   GND    - Ground (common ground)
 *
 */
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Please, fill with YOUR data
#define SERVER_URL "http://192.168.1.2:8080/"   // Server URL where service waits for a message
#define STASSID  "<YOUR WIFI>"
#define STAPSK   "<WIFI PASSWORD>"

// Works with global variables to save memory as the llop is quite short
uint8_t  buff[20] = {};
uint16_t pm25     = 0;



/*
 * IKEA VINDRIKTNING have serial on 9600 bod
 *
 * Open Serial port for VINDRIKTNING
 * Open WiFi connection
 */
void setup() {
  Serial.begin( 9600 );

  WiFi.begin(STASSID, STAPSK);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

/*
 * SENDING values to server in one JSON
 */
void sendValuesToServer () {
  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    http.begin(client, SERVER_URL);
    http.addHeader("Content-Type", "application/json; charset=utf-8");

    String query = "{ \"DUST\" : " + String( pm25 ) + " } ";   // JSON like { "DUST" : value } will be sent
    int httpCode = http.POST( query );

    /* This part of the code needs only to debug purpose, so it is closed on production
    if (httpCode > 0) {
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    */

    http.end();
  }

}


/*
 * READING values from
 *
 *    format: 22 17 11 DF1 DF2 DF3 DF4 DF5 DF6 DF7 DF8 DF9 DF10 DF11 DF12 [CS]
 *    PM2.5 = DF3 * 256 + DF4
 *
 * The idea: read 20 bytes and if it starts with device's signature, when convert the output into numbers
 *
 */
void loop() {
  if ( Serial.available() ) {
    Serial.readBytes( buff, 20 );
    if (  buff[0] == 22 && buff[1] == 17 && buff[2] == 11 ) {
      pm25 = (uint16_t) buff[5] * (uint16_t) 256 + (uint16_t) buff[6];
      sendValuesToServer ();
    }
  }
  delay (50);
}
