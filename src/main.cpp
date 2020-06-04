#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Homie.h>

#define PIN   D4
#define LED_NUM 7

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_NUM, PIN, NEO_GRB + NEO_KHZ800);

const int UPDATE_INTERVAL = 1;
unsigned long lastUpdated = 0;
unsigned long lastStatusUpdated = 0;
String myStatus = "";
String myStatusDetail = "";
String myColor = "grey";

String newStatus = "";
String newStatusDetail = "";

//id, name, type
HomieNode statuslightNode("statuslight", "Statuslight", "statuslight");

void setColor(uint8 R, uint8 G, uint8 B) {
  for (int i = 0; i < LED_NUM; i++) {
    leds.setPixelColor(i, leds.Color(R, G, B));
  }
  leds.show();
}

void updateDevice(const String& status, const String& statusDetail) {

  if ((status != myStatus) || (statusDetail != myStatusDetail)) {

    myStatus = status;
    myStatusDetail = statusDetail;

    String color = "grey";

    if (myStatus == "busy") {
      color = "red";
      setColor(255, 0, 0);
    }
    if (myStatus == "free") {
      color = "green";
      setColor(0, 255, 0);
    }
    if (myStatus == "away") {
      color = "yellow";
      setColor(200, 155, 0);
    }
    if (myStatus == "error") {
      color = "grey";
      setColor(100, 100, 100);
    }
    if (color != myColor) {
      myColor = color;
      statuslightNode.setProperty("color").send(myColor);
    }

    Homie.getLogger() << "Status: " << myStatus << " - " << myStatusDetail << endl;
    Homie.getLogger() << "Color: " << myColor << endl;
    
  }

}


void onHomieEvent(const HomieEvent& event) {
  switch (event.type) {
    case HomieEventType::STANDALONE_MODE:
      Serial << "Standalone mode started" << endl;
      break;
    case HomieEventType::CONFIGURATION_MODE:
      Serial << "Configuration mode started" << endl;
      updateDevice(myStatus, "Configuration mode");
      break;
    case HomieEventType::NORMAL_MODE:
      Serial << "Normal mode started" << endl;
      break;
    case HomieEventType::OTA_STARTED:
      Serial << "OTA started" << endl;
      break;
    case HomieEventType::OTA_PROGRESS:
      Serial << "OTA progress, " << event.sizeDone << "/" << event.sizeTotal << endl;
      break;
    case HomieEventType::OTA_FAILED:
      Serial << "OTA failed" << endl;
      break;
    case HomieEventType::OTA_SUCCESSFUL:
      Serial << "OTA successful" << endl;
      break;
    case HomieEventType::ABOUT_TO_RESET:
      Serial << "About to reset" << endl;
      break;
    case HomieEventType::WIFI_CONNECTED:
      Serial << "Wi-Fi connected, IP: " << event.ip << ", gateway: " << event.gateway << ", mask: " << event.mask << endl;
      updateDevice(myStatus, "WiFi ok, MQTT...");
      break;
    case HomieEventType::WIFI_DISCONNECTED:
      Serial << "Wi-Fi disconnected, reason: " << (int8_t)event.wifiReason << endl;
      myStatus = "error";
      updateDevice(myStatus, "Wifi disconnected");
      break;
    case HomieEventType::MQTT_READY:
      Serial << "MQTT connected" << endl;
      updateDevice(myStatus, "MQTT connected");
      break;
    case HomieEventType::MQTT_DISCONNECTED:
      Serial << "MQTT disconnected, reason: " << (int8_t)event.mqttReason << endl;
      updateDevice("error", "MQTT disconnected");
      break;
    case HomieEventType::READY_TO_SLEEP:
      Serial << "Ready to sleep" << endl;
      break;
    case HomieEventType::SENDING_STATISTICS:
      Serial << "Sending statistics" << endl;
      break;
    case HomieEventType::MQTT_PACKET_ACKNOWLEDGED:
      break;
  }
}

bool globalInputHandler(const HomieNode& node, const HomieRange& range, const String& property, const String& value) {
  Homie.getLogger() << "Received on node " << node.getId() << ": " << property << " = " << value << endl;

  if (property == "status") {
      lastStatusUpdated = millis();
      statuslightNode.setProperty("status").send(value);
      newStatus = value;
  }
  if (property == "statusdetail") {
    statuslightNode.setProperty("statusdetail").send(value);
    newStatusDetail = value;
  }
  return true;
}

void loopHandler() {

  if (millis() - lastUpdated >= UPDATE_INTERVAL * 1000UL || lastUpdated == 0) {
//    Homie.getLogger() << "Status has been upated " << millis() - lastStatusUpdated << " millisec ago" << endl;
    updateDevice(newStatus, newStatusDetail);
    lastUpdated = millis();


    if (millis() - lastStatusUpdated >= UPDATE_INTERVAL * 100 * 1000UL && lastStatusUpdated != 0) {
      newStatus = "error";
      newStatusDetail = "timeout";
      updateDevice(newStatus, newStatusDetail);
      lastStatusUpdated = millis();
    }

  }
}

void setup() {
  Homie_setBrand("SL");
  Homie.setResetTrigger(0, LOW, 2000);
  Serial.begin(115200);
  Serial << endl << endl;
  Homie_setFirmware("SLON-RGB-LED", "0.0.3");
  Homie.setLoopFunction(loopHandler);
  statuslightNode.advertise("status").setName("Status").setDatatype("string").settable();
  statuslightNode.advertise("statusdetail").setName("StatusDetail").setDatatype("string").settable();
  statuslightNode.advertise("color").setName("Color").setDatatype("string");

  Homie.onEvent(onHomieEvent);
  Homie.setGlobalInputHandler(globalInputHandler);

  leds.begin(); // This initializes the NeoPixel library.
  leds.setBrightness(255);
  leds.setPixelColor(6, leds.Color(0, 0, 10));
  leds.show();

  updateDevice(myStatus, "Starting...");
  Homie.setup();

}



void led_set(uint8 R, uint8 G, uint8 B) {
  for (int i = 0; i < LED_NUM; i++) {
    leds.setPixelColor(i, leds.Color(R, G, B));
    leds.show();
    delay(50);
  }
}

void loop() {
  Homie.loop();
}