// include the library
#include <RadioLib.h>
#include <WiFi.h> 
#include <PubSubClient.h>
#include <iostream> 
#include <string>

// SX1276 JVTech has the following connections:
// NSS pin:   5
// DIO0 pin:  26
// RESET pin: 14
// DIO1 pin:  3
SX1276 radio = new Module(5, 26, 14, 3);

// Wi-fi
const char* ssid = "2 Day"; 
const char* password = "2dayitsolutions";

// MQTT broker 
const char* mqtt_server = "test.mosquitto.org"; // Broker address
const char* topic = "esp32/sensor";   // Topic for subscription/publication
//const char* mqtt_usuario = "";        // Username (not required for test.mosquitto.org)
//const char* mqtt_senha = "";          // Password (not required for test.mosquitto.org)
const int mqtt_port = 1883;           // MQTT port for TCP connection
//const char* mqtt_client_id = "c7a342cc-f0a0-4137-8df8-5a5db9a90d71"; // Unique client ID
const String mqtt_client_id = "mosquiteiro";

// Objetos para Wi-Fi and MQTT
WiFiClient espClient; 
PubSubClient client(espClient);

String mensagemRecebida = "";

void setup() {
  Serial.begin(9600);

  setup_wifi(); 

  client.setServer(mqtt_server, mqtt_port); 
  client.setCallback(callback);
  Serial.println("1 - Setup");

  while (!client.connected()) { 
    Serial.print("Conectando ao MQTT...");
      Serial.println("2 - Setup");

     if (client.connect(mqtt_server)) {
      Serial.println("conectado!");
      client.subscribe(topic);
      Serial.println("3 - Setup");

      } else {
        Serial.print("falhou, rc=");
        Serial.print(client.state());
        Serial.println(" tentando novamente em 10 segundos...");
        delay(10000); 
        Serial.println("4 - Setup");

    }
  }

 
  // initialize SX1276 with default settings
  Serial.print(F("[SX1276] Initializing ... "));
  int state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true) { delay(10); }
  }

  radio.setFrequency(915.2);
  radio.setBandwidth(125.0);
  radio.setSpreadingFactor(9);
  radio.setCodingRate(8);
  radio.setOutputPower(22);

  // when new packet is received
  radio.setPacketReceivedAction(setFlag);

  // start listening for LoRa packets
  //Serial.print(F("[SX1276] Starting to listen ... "));
  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true) { delay(10); }
  }

}

// flag to indicate that a packet was received
volatile bool receivedFlag = false;

#if defined(ESP8266) || defined(ESP32)
  ICACHE_RAM_ATTR
#endif
void setFlag(void) {
  // we got a packet, set the flag
  receivedFlag = true;
}

void setup_wifi() { 
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
   delay(500);
    Serial.print(".");
  } 
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

// Retorna via uart o que foi lido pela subscricao
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida em [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  } 
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
   Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "mosquiteiro";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
     Serial.println("connected");
      // Once connected, publish an announcement...
     client.publish("esp32/sensor", "hello world");
      // ... and resubscribe
     //client.subscribe("exemple");
    } else {
     Serial.print("failed, rc=");
     Serial.print(client.state());
     Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
    if (!client.connected()) {
       reconnect();
    }
    client.loop();
/*
  //if (!client.connected()) {
    while (!client.connected()) {
      Serial.print("Reconectando ao MQTT...");

      if (client.connect(mqtt_server)) {
        Serial.println("conectado!");
        client.subscribe(topic);

      } else {
        Serial.print("falhou, rc=");
        Serial.print(client.state());
        Serial.println(" tentando novamente em 5 segundos...");
        delay(5000);
        }
      }
      client.loop();
   // }
  */

  // check if the flag is set
  if(receivedFlag) {
    // reset flag
    receivedFlag = false;

    int state = radio.readData(mensagemRecebida);

    if (state == RADIOLIB_ERR_NONE) {

      delay(3);  // delay in between reads for stability

      // print data of the packet
      Serial.print(F("[SX1276] Data:\t\t"));
      Serial.print("Tensão: ");
      Serial.print(mensagemRecebida);
      Serial.print(" V");
      Serial.println();

      //Serial.print(radio.getRSSI());

      //Serial.print(radio.getSNR());

      //Serial.print(radio.getFrequencyError());

    } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
      // packet was received, but is malformed
      Serial.println(F("[SX1278] CRC error!"));

    } else {
      // some other error occurred
      Serial.print(F("[SX1278] Failed, code "));
      Serial.println(state);

    }
  }

String payload = String(mensagemRecebida);
const char* charPtr = payload.c_str();
client.publish("esp32/sensor", charPtr);



  delay(1000); // Enviar mensagem a cada segundo
}