#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Dirección LCD
#define LCD_ADDR 0x27
#define LED_OFF 0
#define LED_ON 1
LiquidCrystal_I2C lcd(LCD_ADDR, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

byte charRelleno[] = {
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F
};

byte charFlecha[] = {
  0x10,
  0x18,
  0x1C,
  0x1E,
  0x1E,
  0x1C,
  0x18,
  0x10
};

void setup() {

  lcd.begin(16, 2);
  lcd.createChar(0,charRelleno);
  lcd.createChar(0,charFlecha);
  lcd.setBacklight(LED_ON);
  lcd.print("Cargando, espere");
}

void loop() {
  // put your main code here, to run repeatedly:

}
