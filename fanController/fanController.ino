// fanController.ino
// Greg Paton
// 30 JUly 2013
//
// Receives message from RPi/Arduino to either
// turn on if a 1 is received or turn off if a
// 2 is received.

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

const unsigned char PAYLOAD_SIZE = 3;

int serial_putc(char c, FILE *) 
{
  Serial.write(c);

  return c;
} 

void printf_begin(void)
{
  fdevopen( &serial_putc, 0 );
}


// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 
RF24 radio(9, 10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0x1212121212LL, 0x1212121212LL };
//const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

const char relayPin = 8;

// PAYLOAD CODE:
// 1  - on
// 2  - off
// 3  - poll

void setup(void) {
  // Turn fan off 
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
  

  Serial.begin(57600);
  printf_begin();
  printf("\n\rRF24 fan controller/\n\r");

  radio.begin();
  radio.setRetries(15, 15);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  radio.setPayloadSize(3);
  
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1, pipes[1]);
  radio.setChannel(0x01);
  
  radio.startListening();
  radio.printDetails();
}

void loop(void) {
  if (radio.available())
  {
    unsigned char payload[PAYLOAD_SIZE];
    radio.read(&payload, PAYLOAD_SIZE);

    printf("Recv: %c%c%c...\n", payload[0],payload[1], payload[2]);
      
    if (payload[0] == '1')
      digitalWrite(relayPin, LOW);
    else if (payload[0] == '2')
      digitalWrite(relayPin, HIGH);
    else if (payload[0] == '3') 
      sendStatus();
  }
}

void sendStatus() {
  unsigned char payload[PAYLOAD_SIZE];
  payload[0] = 2;
  radio.stopListening();
  if (digitalRead(relayPin) == LOW)
    payload[0] = 1;
    
  radio.write(&payload, PAYLOAD_SIZE); 
  radio.startListening();
}
