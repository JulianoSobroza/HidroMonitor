// include the library  https://github.com/jgromes/RadioLib
#include <RadioLib.h>

// SX1276 JVTech has the following connections:
// NSS pin:   5
// DIO0 pin:  26
// RESET pin: 14
// DIO1 pin:  3
SX1276 radio = new Module(5, 26, 14, 3);

// save transmission state between loops
int transmissionState = RADIOLIB_ERR_NONE;

// define the message to be sent
String message;

// leitura do pino GPIO36/SensVP/ADC1_0

int adcPin = 36; // Terceiro pino de acordo com https://jvtech.gitbook.io/modulo-iot-jvtech/hardware/pinos
int adcValue = 0;
float voltage = 0.0;

void setup() {
  Serial.begin(9600);
  analogReadResolution(12); // ESP32 suporta resoluções de 9 a 12 bits

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

  // set the function that will be called
  radio.setFrequency(915.2);
  radio.setBandwidth(125.0);
  radio.setSpreadingFactor(9);
  radio.setCodingRate(8);
  radio.setOutputPower(22);

  // when packet transmission is finished
  radio.setPacketSentAction(setFlag);

  // start transmitting the first packet
  Serial.print(F("[SX1276] Sending first packet ... "));

  transmissionState = radio.startTransmit(message);

}

// flag to indicate that a packet was sent
volatile bool transmittedFlag = false;

// this function is called when a complete packet
// is transmitted by the module
#if defined(ESP8266) || defined(ESP32)
  ICACHE_RAM_ATTR
#endif
void setFlag(void) {
  // we sent a packet, set the flag
  transmittedFlag = true;
}

// counter to keep track of transmitted packets
int count = 0;

void loop() {

  adcValue = analogRead(adcPin);
  voltage = (adcValue * 3.3) / 4095.0; // Convertendo para a tensão real, considerando 12 bits (0-4095) e Vref de 3.3V
  Serial.print("Tensão: ");
  Serial.print(voltage);
  Serial.println(" V");
  delay(1000); // Aguarda 1 segundo


  // check if the previous transmission finished
  if(transmittedFlag) {
    // reset flag
    transmittedFlag = false;

    if (transmissionState == RADIOLIB_ERR_NONE) {
      // packet was successfully sent
      Serial.println(F("transmission finished!"));

      // NOTE: when using interrupt-driven transmit method,
      //       it is not possible to automatically measure
      //       transmission data rate using getDataRate()

    } else {
      Serial.print(F("failed, code "));
      Serial.println(transmissionState);

    }

    // clean up after transmission is finished
    // this will ensure transmitter is disabled,
    // RF switch is powered down etc.
    radio.finishTransmit();

    // wait a second before transmitting again
    delay(100);

    // send another one
    Serial.print(F("[SX1276] Sending another packet ... "));

    // you can transmit C-string or Arduino string up to
    // 255 characters long
    String voltageString = String(voltage);
    transmissionState = radio.startTransmit(voltageString);

  }
}
