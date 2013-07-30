#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"


int serial_putc( char c, FILE * ) 
{
  Serial.write( c );

  return c;
} 

void printf_begin(void)
{
  fdevopen( &serial_putc, 0 );
}

RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };


void setup(void) {

  Serial.begin(57600);
  printf_begin();
  printf("\n\rRF24 send \n\r");

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15, 15);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  //radio.setPayloadSize(8);
  
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1, pipes[1]);
}

void loop(void) {
  if (Serial.available())
  {
    unsigned long payload = 0;
    if (Serial.read() == '1')
      payload = 1;
    else if (Serial.read() == '2')
      payload = 2;
    else 
      return;
      
    radio.write(&payload, sizeof(unsigned long));
    
    boolean timeout = true;
    while (false) {
      radio.stopListening();
      if (false == radio.write(&payload, sizeof(unsigned long)))
        continue;
      
      printf("Sent: %lu\n", payload);

      // Now, continue listening
      /*radio.startListening();

      // Wait here until we get a response, or timeout (250ms)
      timeout = false;
      unsigned long startTime = millis();
      while (!radio.available() && !timeout)
        if (millis() - startTime > 1000 )
          timeout = true;
      */
    } 

    /*radio.read(&payload, sizeof(unsigned long));
    radio.stopListening();
    printf("from recv %lu\n\r", payload);*/
  }
}
