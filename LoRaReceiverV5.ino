#include <SPI.h>
#include <LoRa.h>

int LED = 4;
int t = 200;
const int dataPin = 7; 

#define MAX_PACKET_SIZE 10  // maximum number of packets to store

// global arrays to store the received values
int id_array[MAX_PACKET_SIZE];
int freq_bin_array[MAX_PACKET_SIZE];
int displacement_array[MAX_PACKET_SIZE];

// counter variable to keep track of the number of packets received
int packCounter = 0;


void blinkk() {
  digitalWrite(LED, HIGH);
  delay(t);
  digitalWrite(LED, LOW);
  delay(t);
  digitalWrite(LED, HIGH);
  delay(t);
  digitalWrite(LED, LOW);
  delay(t);
  digitalWrite(LED, HIGH);
}


void setup() {

  pinMode(LED, OUTPUT);
  pinMode(dataPin, OUTPUT);
  digitalWrite(dataPin,HIGH);

  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Receiver");

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}


void printArrays() {
  Serial.println("Data in arrays:");
  for (int i = 0; i < packCounter; i++) {
    Serial.print("ID: ");
    Serial.print(id_array[i]);
    Serial.print(", Frequency bin: ");
    Serial.print(freq_bin_array[i]);
    Serial.print(", Displacement: ");
    Serial.println(displacement_array[i]);
  }
}


void binaryConversion() {
  Serial.println("PACKAGE CONVERSION");
  for (int k = 0; k < 10; k++) { // processing performed for all 10 package strings in all three string arrays

    String IDString = String(id_array[k], BIN);
    String freqString = String(freq_bin_array[k], BIN);
    String displacementString = String(displacement_array[k], BIN);


    while (IDString.length() < 7) {
      IDString = "0" + IDString;
    }

    while (freqString.length() < 4) {
      freqString = "0" + freqString;
    }

    while (displacementString.length() < 10) {
      displacementString = "0" + displacementString;
    }

    // print binary string

    /**/
    Serial.print(IDString);
    Serial.print(freqString);
    Serial.println(displacementString);
    
    


/**/
  // For all strings in the data package, the binary value are checked and printed..
  digitalWrite(dataPin, LOW);  // START BIT
  delayMicroseconds(104);

    for (int n = 0; n < 7; n++) {
      if (IDString[n] == '1') {
        digitalWrite(dataPin, HIGH);
      } else if (IDString[n] == '0') {
        digitalWrite(dataPin, LOW);
      }
      delayMicroseconds(104);
    }

    for (int n = 0; n < 4; n++) {
      if (freqString[n] == '1') {
        digitalWrite(dataPin, HIGH);
      } else if (freqString[n] == '0') {
        digitalWrite(dataPin, LOW);
      }
      delayMicroseconds(104);
    }

    for (int n = 0; n < 10; n++) {
      if (displacementString[n] == '1') {
        digitalWrite(dataPin, HIGH);
      } else if (displacementString[n] == '0') {
        digitalWrite(dataPin, LOW);
      }
      delayMicroseconds(104);
    }
    
    
    digitalWrite(dataPin,HIGH); // STOP BIT
    delayMicroseconds(104);

    delay(100); // delay of 100 ms between each datapackage.
    
  }
}





void loop() {
  // Check if a packet has been received.
  int packetSize = LoRa.parsePacket();
  if (packetSize) {

    // received a packet
    Serial.print("Received packet: ");
    Serial.print(packCounter + 1);

    // initialize variables for storing the values
    int id = 0;
    int freq_bin = 0;
    int displacement = 0;

    // read packet and convert characters to intergers and store in variables
    int i = 0;
    while (LoRa.available()) {
      char c = LoRa.read();
      if (c == ',') {
        i++;
        continue;
      }
      if (i == 0) {
        id = id * 10 + (c - '0');
      } else if (i == 1) {
        freq_bin = freq_bin * 10 + (c - '0');
      } else if (i == 2) {
        displacement = displacement * 10 + (c - '0');
      }
    }

    // store the integers in arrays of the current packCounter
    id_array[packCounter] = id;
    freq_bin_array[packCounter] = freq_bin;
    displacement_array[packCounter] = displacement;

    // increment the packcounter variable
    packCounter++;

    // print RSSI of packet
    Serial.print(", RSSI: ");
    Serial.println(LoRa.packetRssi());
  }


  // If all data packages have been received
  if (packCounter == MAX_PACKET_SIZE) {
    //Serial.println("ALL PACKAGES RECEIVED");

    // print contents of data arrays 
    printArrays();

    // convert packages to binary string and output
    binaryConversion();


    // reset the packCounter variable for the next round of data packets
    packCounter = 0;
  }
}
