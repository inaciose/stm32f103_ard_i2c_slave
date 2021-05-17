
#include <Arduino.h>
#include <Wire.h>

#ifndef LED_BUILTIN
  #define LED_BUILTIN PC13
#endif

#define LED_PIN LED_BUILTIN
bool ledon = false;

// serial data manipulation
union u_tag {
    byte b[4];
    int i[2];
    float f;
} i2c_data[8];

#define I2C_SLAVE_ADDR 0x08
#define DATA_TO_SEND_LENGHT 30


volatile int receiveEventCount = 0;
volatile int requestEventCount = 0;

int bytesRead = 0;

char test[DATA_TO_SEND_LENGHT];

//            SDA  SCL
TwoWire Wire2(PB7, PB6);

// callback for received data
void receiveEvent(int byteCount) {
    receiveEventCount++;
    byte i2c_byte[32];
    int i = 0;
    // fill i2c_byte buffer
    while (Wire2.available()) { // slave may send less than requested
        char c = Wire2.read(); // receive a byte as character
        i2c_byte[i] = c; 
        i++;
    }
    
    // set i2c_data array
    for(i=0; i < 8; i++) {
        i2c_data[i].b[0] = i2c_byte[i*4 + 0];
        i2c_data[i].b[1] = i2c_byte[i*4 + 1];
        i2c_data[i].b[2] = i2c_byte[i*4 + 2];
        i2c_data[i].b[3] = i2c_byte[i*4 + 3];
    }
    
}

// callback for sending data
void requestEvent() {
    requestEventCount++;
    Wire2.write("hello ");
}


void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
    Serial.println("Serial.begin done");

    // initialize wire
    //Wire.begin();
    Wire2.begin(I2C_SLAVE_ADDR);

    // set callbacks
    Wire2.onReceive(receiveEvent);
    Wire2.onRequest(requestEvent);
}
  
void loop()
{

    
    Serial.print((byte)i2c_data[0].b[0]); Serial.print("\t");
    Serial.print((byte)i2c_data[0].b[1]); Serial.print("\t");
    Serial.print((byte)i2c_data[0].b[2]); Serial.print("\t");
    Serial.print((byte)i2c_data[0].b[3]); Serial.print("\t");
    
    Serial.print((int)i2c_data[0].i); Serial.print("\t");
    Serial.print((int)i2c_data[1].i[0]); Serial.print("\t");
    Serial.print((int)i2c_data[1].i[1]); Serial.print("\t");
    Serial.print((int)i2c_data[1].i); Serial.print("\t");

    Serial.print(receiveEventCount); Serial.print("\t");
    Serial.print(requestEventCount); Serial.print("\t");
    //Serial.print(bytesRead); Serial.print("\t");

    Serial.println("");
    //bytesRead = 0;
    delay(500);
    

#ifdef LED_PIN
    // led blink control
    static uint32_t ledtimer = millis();
    if(millis() > ledtimer) {
    ledtimer = millis()+1000;
        if(ledon) {
            digitalWrite(LED_PIN, LOW);
            ledon = false;
        } else {
            digitalWrite(LED_PIN, HIGH);
            ledon = true;  
        }
    }

#endif
}
