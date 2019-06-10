#include <Wire.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
int cntr = 0;

#define EEPROM_ADR_LOW_BLOCK 0x25 //0b.101.0000 0x50
#define EEPROM_ADR_HIGH_BLOCK 0x54 //0b.101.0100 0x54

void setup() {
  Wire.begin();
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.write("-->: ");
  delay(5000);
  lcd.setCursor(0, 0);
  lcd.print("DONE       Addr:");

  //Output raw bytes to terminal
  for (long x = 0 ; x < 0x17F ; x++) //Read 0 - 17F bytes from EERPOM
  {
    byte val = readEEPROM(x);
    Serial.print(val, HEX);
    Serial.print(" ");
    //Serial.write(val);
    lcd.setCursor(7, 0);
    lcd.print("   ");
    lcd.setCursor(7, 0);
    lcd.print(val);
    lcd.setCursor(11, 1);
    lcd.print(x);
    delay(100);
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
