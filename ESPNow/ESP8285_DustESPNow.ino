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

#include <espnow.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#endif


// Works with global variables to save memory as the llop is quite short
uint8_t  buff[20] = {};
uint16_t pm25     = 0;

#define MY_ROLE         ESP_NOW_ROLE_COMBO              // set the role of this device: CONTROLLER, SLAVE, COMBO
#define RECEIVER_ROLE   ESP_NOW_ROLE_COMBO              // set the role of the receiver
#define WIFI_CHANNEL    1

#define MY_NAME         "DUSTNODE"
#define MY_PASSWORD     "PASSWORD"

/*
  The structure of the packet is easy: 
  - rule is like 'd'
  - dust is int value of the sensor
*/
struct __attribute__((packed)) dataPacket {
  char rule;
  int dust;
};

uint8_t partnerAddress[6] = { 0x84, 0xF7, 0x03, 0xF6, 0x94, 0xBA }; // The MAC address of Node which receive the values

bool can_i_sleep = false; // If successfully sent go to sleep for a minute

// Callback if sent, if good - set flag to True
void transmissionComplete(uint8_t *receiver_mac, uint8_t transmissionStatus) {
  if (transmissionStatus == 0) {
    can_i_sleep = true;
  } else {
    can_i_sleep = false;
  }
}

// Will never called but to be ready for everything
void dataReceived(uint8_t *senderMac, uint8_t *data, uint8_t dataLength) {
  Serial.println("Receive");
}

/*
 * IKEA VINDRIKTNING have serial on 9600 bod
 *
 * Open Serial port for VINDRIKTNING
 *   - read values (re-try a few times to be sure we have it
 *   - if no values - go to sleep for a while
 *
 * Configure ESPNOW channel
 *   - be sure wifi is down
 *   - setup the channel
 *   - prepare the package
 *   - send the package, wait a second as we are waiting to call-back
 *
 * Go to sleep
 */
void setup() {
  Serial.begin( 9600 );

  pm25 = 0;
  
  for (int f = 0; f < 30; f ++) {
    if ( Serial.available() ) {
      Serial.readBytes( buff, 20 );
      if (  buff[0] == 22 && buff[1] == 17 && buff[2] == 11 ) {
        pm25 = (uint16_t) buff[5] * (uint16_t) 256 + (uint16_t) buff[6];
        break;
      }
    }
    delay(500);
  }

  if (pm25 < 1) {
    ESP.deepSleep(10e6);
  }

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();        // we do not want to connect to a WiFi network

  if (esp_now_init() != 0) {
    return;
  }

  esp_now_set_self_role(MY_ROLE);
  esp_now_register_send_cb(transmissionComplete);         // this function will get called once all data is sent
  esp_now_register_recv_cb(dataReceived);                 // this function will get called whenever we receive data

  esp_now_add_peer(partnerAddress, RECEIVER_ROLE, WIFI_CHANNEL, NULL, 0);

  dataPacket packet;
  packet.rule = 'd';
  packet.dust = int(pm25);

  for ( int f = 0; f < 3; f++ ) {
    esp_now_send(partnerAddress, (uint8_t *) &packet, sizeof(packet));
    delay(100);

    if (can_i_sleep == true) {
      break;
    }
  }

  ESP.deepSleep(60e6);
}

void loop() {
  delay(10);
}
