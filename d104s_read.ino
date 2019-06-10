#include <Wire.h>
#include <LiquidCrystal.h>

// ------- FROM ---------------------------------
/**
 * I2CScanner.ino -- I2C bus scanner for Arduino
 *
 * 2009,2014, Tod E. Kurt, http://todbot.com/blog/
 *
 */
extern "C" { 
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}
// Scan the I2C bus between addresses from_addr and to_addr.
// On each address, call the callback function with the address and result.
// If result==0, address was found, otherwise, address wasn't found
// (can use result to potentially get other status on the I2C bus, see twi.c)
// Assumes Wire.begin() has already been called
void scanI2CBus(byte from_addr, byte to_addr, 
                void(*callback)(byte address, byte result) ) 
{
  byte rc;
  byte data = 0; // not used, just an address to feed to twi_writeTo()
  for( byte addr = from_addr; addr <= to_addr; addr++ ) {
    rc = twi_writeTo(addr, &data, 0, 1, 0);
    callback( addr, rc );
  }
}

// Called when address is found in scanI2CBus()
// Feel free to change this as needed
// (like adding I2C comm code to figure out what kind of I2C device is there)
void scanFunc( byte addr, byte result ) {
  Serial.print("addr: ");
  Serial.print(addr,DEC);
  Serial.print( (result==0) ? " found!":"       ");
  Serial.print( (addr%4) ? "\t":"\n");
}
byte start_address = 8;       // lower addresses are reserved to prevent conflicts with other protocols
byte end_address = 119;       // higher addresses unlock other modes, like 10-bit addressing

// ----------------------------------------
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
int cntr = 0;

#define EEPROM_ADR_LOW_BLOCK 0x28 //0b.101.0000 0x50    0x28 = 40d -> YES!, 0x55 = 85d -> something unknown
#define EEPROM_ADR_HIGH_BLOCK 0x54 //0b.101.0100 0x54

void setup() {
  // -- from i2c scanner --------------------------------------------------------
  delay(10000);
  Wire.begin();

  Serial.begin(9600);
  Serial.println("\nI2CScanner ready!");

  Serial.print("starting scanning of I2C bus from ");
  Serial.print(start_address,DEC);
  Serial.print(" to ");
  Serial.print(end_address,DEC);
  Serial.println("...");

  // start the scan, will call "scanFunc()" on result from each address
  scanI2CBus( start_address, end_address, scanFunc );

  Serial.println("\ndone");
  delay(4000);
  // ----------------------------------------------------------
  //Wire.begin();
  //Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.write("-->: ");
  delay(3000);
  lcd.setCursor(0, 0);
  lcd.print("DONE       Addr:");

  //Output raw bytes to terminal
  for (long x = 0 ; x < 0x17F ; x++) //Read 0 - 17F bytes from EERPOM
  {
    byte val = readEEPROM(x);
    if (val > 31 && val < 127){
      Serial.print((char) val);
    } //else {
      //Serial.print(" ");
      //Serial.print(val, HEX);}
    //Serial.print(".");
    //Serial.write(val);
    
    //lcd.setCursor(7, 0);
    //lcd.print("   ");
    lcd.setCursor(7, 0);
    lcd.print(val, HEX);
    lcd.setCursor(11, 1);
    lcd.print(x);
    //delay(100);
  }
  
}

void loop() {
//  Wire.requestFrom(8, 6);
//  while (Wire.available()){
//    char c = Wire.read();
//    Serial.print(c);  
//  }

  delay(500);
  lcd.setCursor(0, 1);
  lcd.print(cntr);
  cntr++;
}

byte readEEPROM(long eeaddress)
{
  if (eeaddress < 65536)
    Wire.beginTransmission(EEPROM_ADR_LOW_BLOCK);
  else
    Wire.beginTransmission(EEPROM_ADR_HIGH_BLOCK);

  Wire.write((int)(eeaddress >> 8)); // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();

  if (eeaddress < 65536)
    Wire.requestFrom(EEPROM_ADR_LOW_BLOCK, 1);
  else
    Wire.requestFrom(EEPROM_ADR_HIGH_BLOCK, 1);

  byte rdata = 0xFF;
  if (Wire.available()) rdata = Wire.read();
  return rdata;
}

/*
 * found i2c addresses:
 starting scanning of I2C bus from 8 to 119...
addr: 8
addr: 9        addr: 10        addr: 11        addr: 12
addr: 13        addr: 14        addr: 15        addr: 16
addr: 17        addr: 18        addr: 19        addr: 20
addr: 21        addr: 22        addr: 23        addr: 24
addr: 25        addr: 26        addr: 27        addr: 28
addr: 29        addr: 30        addr: 31        addr: 32
addr: 33        addr: 34        addr: 35        addr: 36
addr: 37        addr: 38        addr: 39        addr: 40 found!
addr: 41        addr: 42        addr: 43        addr: 44
addr: 45        addr: 46        addr: 47        addr: 48
addr: 49        addr: 50        addr: 51        addr: 52
addr: 53        addr: 54        addr: 55        addr: 56
addr: 57        addr: 58        addr: 59        addr: 60
addr: 61        addr: 62        addr: 63        addr: 64
addr: 65        addr: 66        addr: 67        addr: 68
addr: 69        addr: 70        addr: 71        addr: 72
addr: 73        addr: 74        addr: 75        addr: 76
addr: 77        addr: 78        addr: 79        addr: 80
addr: 81        addr: 82        addr: 83        addr: 84
addr: 85 found! addr: 86        addr: 87        addr: 88
addr: 89        addr: 90        addr: 91        addr: 92
addr: 93        addr: 94        addr: 95        addr: 96
addr: 97        addr: 98        addr: 99        addr: 100
addr: 101         addr: 102         addr: 103         addr: 104
addr: 105         addr: 106         addr: 107         addr: 108
addr: 109         addr: 110         addr: 111         addr: 112
addr: 113         addr: 114         addr: 115         addr: 116
addr: 117         addr: 118         addr: 119
done 
 */
