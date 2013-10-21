/*
 Written by: Mujahed Altahle
 
This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ 
or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

 */
/* A simple Project for Remote Controlling with nRF24L01+ radios. 
 We have 2 nodes, the transmitter (this one) and the receiver which is attached to the Car.
 The idea is to transmit  2 parameters , one is Direction (Backward, or Forward with the speed) the other is the Steering (Left, or Right with the degree of rotation).
 The nRF24L01 transmit values in type of "uint8_t" with maximum of 256 for each packet, so the values of direction is divided by (10) at the Tx side,
 then it is multiplied by 10 again at the Rx side.
 The Rx rules is to output the received parameters to port 3 and 6 where the Servo and the ESC are are attached
 a condition is required to prevent the controller from transmitting values that is not useful (like 1480-1530 for ESC and 88-92 for Servo) to save more power as much as we can
 */
 
#include <Servo.h> 
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
RF24 radio(9,10);

Servo servo; //steering servo declaration 
Servo esc; // Electronic Speed Contoller declaration

// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe = 0xE8E8F0F0E1LL;

//
// Payload
//

uint8_t received_data[2];
uint8_t num_received_data =sizeof(received_data);

//
// Setup
//

void setup(void)
{
  delay(3000); //wait until the esc starts in case of Arduino got power first
  servo.attach(3);  // attaches the servo on pin 3 to the servo object 
  esc.attach(6);  // attaches the ESC on pin 6 to the ese object  
  servo.write(90);
  //
  // Print preamble
  //

  Serial.begin(57600);
  printf_begin();

  //
  // Setup and configure rf radio
  //

  radio.begin(); //Begin operation of the chip.
  // This simple sketch opens a single pipes for these two nodes to communicate
  // back and forth.  One listens on it, the other talks to it.
  radio.openReadingPipe(1,pipe);
  radio.startListening();
  //
  // Dump the configuration of the rf unit for debugging
  //
  radio.printDetails(); 
}


void loop(void)
{
  // if there is data ready
  if ( radio.available() )
  {
    bool done = false;
    int ESC_value;
    while (!done)
    {
      // Fetch the payload, and see if this was the last one.
      done = radio.read( received_data, num_received_data );
      ESC_value=received_data[0]*10; //Multiplication by 10 because the ESC operates for vlues around 1500 and the nRF24L01 can transmit maximum of 255 per packet 
      esc.writeMicroseconds(ESC_value);
    //  Serial.println(ESC_value);
      servo.write((received_data[1]));
     // Serial.println(received_data[1]);
    }
  }
}

