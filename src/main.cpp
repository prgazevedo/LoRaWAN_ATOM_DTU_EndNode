#include "ATOM_DTU_LoRaWAN.h"
#include "M5Atom.h"

ATOM_DTU_LoRaWAN LoRaWAN;

// Enum to define the state of the device
enum DTUState_t {
  kError = 0, kConnecting, kConnected, kSending
};

DTUState_t state = kConnecting;

void setup() {
  M5.begin(true, true, true);
  Serial.begin(115200);
  Serial.println("[INFO] -------Setup start------");

  LoRaWAN.Init();
  Serial.println("[INFO] Module Initialized.");

  // Configure LoRaWAN OTAA settings
  Serial.println("[INFO] Configuring OTAA settings...");
  LoRaWAN.configOTTA("00BB9DA5B97ADDF6", "00BB9DA5B97ADDF6", "BB03484DACF0DBF2CAAC07D417B8C725", "2");
  LoRaWAN.setClass("2");
  LoRaWAN.writeCMD("AT+CWORKMODE=2\r\n");
  LoRaWAN.setRxWindow("869525000");
  LoRaWAN.setFreqMask("0001");

  delay(1000);

  Serial.println("[INFO] Waiting for response: ");
  String response = LoRaWAN.waitMsg(1000);
  Serial.println(response);
  
  Serial.println("[INFO] Starting Join Process:");
  LoRaWAN.startJoin();
  Serial.println("[INFO] -------Start Join.....-------");

  // Wait until the device is connected to the network
  while (true) {
    response = LoRaWAN.waitMsg(1000);
    Serial.print("[INFO] Response from Join: ");
    Serial.println(response);

    if (response.indexOf("+CJOIN:") != -1) {
      state = kConnected;
      Serial.println("[INFO] Device Connected.");
      break;
    } else if (response.indexOf("ERROR") != -1) {
      state = kError;
      Serial.println("[ERROR] Join ERROR.");
      ESP.restart();
    } else {
      Serial.println("[WARN] Unexpected response during join: " + response);
    }
  }
  
  Serial.println("[INFO] -------Setup end------");
  delay(2000);
}

void loop() {
  Serial.println("[INFO] -------Loop Enter------");
  
  // Create a sample message to send
  Serial.println("[INFO] Hello from LoRaWAN End Node!");

  String message ="4d35535441434b";
  Serial.println("[INFO] Send LoRa Message: ");
  Serial.println("[INFO] Tx data: ");
  Serial.print(message); // Print the message
  
  LoRaWAN.sendMsg(1,15,7,"4d35535441434b");
  Serial.println("[INFO] Message sent. Waiting for response...");

  // Wait for response
  while (true) {
    state = kSending;
    String response = LoRaWAN.waitMsg(1000);
    Serial.print("[INFO] Rx data: ");
    Serial.println(response);

    if (response.indexOf("OK") != -1) {
      Serial.println("[INFO] Message sent successfully.");
      break;
    } else if (response.indexOf("ERR") != -1) {
      state = kError;
      Serial.println("[ERROR] Send Error: " + response);
      break;
    } else if (response.indexOf("AT") != -1) {
      // Handling unexpected AT command responses
      Serial.println("[WARN] Unexpected AT command response: " + response);
    } else {
      Serial.println("[WARN] Unexpected response: " + response);
    }
  }
  
  delay(2000);

  // Receive data
  String response1 = LoRaWAN.receiveMsg();
  Serial.print("[INFO] Received data: ");
  Serial.println(response1);

  if (response1.indexOf("ERROR") != -1) {
    Serial.println("[ERROR] Error receiving data: " + response1);
  } else if (response1.indexOf("OK") != -1) {
    Serial.println("[INFO] Data received successfully.");
  } else if (response1.indexOf("AT") != -1) {
    // Handling unexpected AT command responses
    Serial.println("[WARN] Unexpected AT command response: " + response1);
  } else {
    Serial.println("[WARN] Unexpected received data: " + response1);
  }

  Serial.println("[INFO] -------Loop End------");
  delay(2000); // Delay before the next loop iteration
}
