/* LIBRERIAS */
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>


/* DECLARACIÓN */
LiquidCrystal_I2C lcd(0x27); //Sustituir con ejemplo gestorluceslcd
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);
//Pines
uint8_t pinTX=19;
uint8_t pinRX=18;
uint8_t pinTouch=3;
uint8_t pinLed=5;
uint8_t pinRele=7;
//Estados y condicionales
boolean bMensajeWeb=false;
boolean bDedoEnSensor=false;
//Variables auxiliares
String stringWeb;
char sAccion;
String sCuerpoMensaje;
int iNumeroSocio=-1;
String auxConcat="";


void setup() {
  //Declaración de modo de pines
  pinMode(pinLed,OUTPUT);
  pinMode(pinTouch,INPUT);
  pinMode(pinRele,OUTPUT);

  //Declaramos el relé cerrado inicialmente
  digitalWrite(pinRele,LOW);

  //Inicializar LCD puerta
  lcd.begin(16,2);
  lcd.home();
  lcd.print("  INICIALIZANDO ");
  lcd.setCursor(0,1);
  lcd.print("    SISTEMAS    ");

  //Establecemos comunicación con app WEB por puerto COM
  Serial.begin(9600);
  while(!Serial);
  delay(100);

  //Inicializamos sensor de huella
  finger.begin(57600);
  if (!finger.verifyPassword()) {
    lcd.clear();
    lcd.home();
    lcd.print("  ERROR SENSOR  ");
    lcd.setCursor(0,1);
    lcd.print("  NO ENCONTRADO ");
  } else {
    lcd.clear();
    //imprimir cabecera con hroa e iconos 
    lcd.setCursor(0,1);
    lcd.print("  PONGA EL DEDO ");
  }
}

void loop() {
  bMensajeWeb=Serial.available();
  bDedoEnSensor=digitalRead(pinTouch);
  if(bMensajeWeb){
    stringWeb=Serial.readString(); //Primer caracter accion - los demás contenido accion
    sAccion=stringWeb.charAt(0);
    sCuerpoMensaje=stringWeb.substring(1);
    switch(sAccion){
      case 'r': //Registrar huella - CuerpoMensaje: número de socio
        registrarHuella((uint16_t)sCuerpoMensaje.toInt());
        digitalWrite(pinLed,LOW);
        break;
      case 'e': //Eliminar huella - CuerpoMensaje: número de socio
        eliminarHuella((uint16_t)sCuerpoMensaje.toInt());
        break;
      case 'b': //Borrar BBDD sensor huella - CuerpoMensaje: vacio
        finger.emptyDatabase();
        break;

      case 'n': //Numero de huellas almacenadas - CuerpoMensaje: vacio
        Serial.println(finger.getTemplateCount());
        break;
    }
  }else if(bDedoEnSensor){
    digitalWrite(pinLed,HIGH);
    lcd.setCursor(0,1);
    lcd.print(" BUSCANDO HUELLA");
    iNumeroSocio=buscarHuella();
    if(iNumeroSocio == -1){
      lcd.setCursor(0,1);
      lcd.print(" ACCESO DENEGADO");
      delay(2500);
      lcd.setCursor(0,1);
      lcd.print("  PONGA EL DEDO ");
    }else{
      lcd.setCursor(0,1);
      lcd.print(" PUERTA ABIERTA ");
      digitalWrite(pinRele,HIGH);
      delay(1000);
      digitalWrite(pinRele,LOW);
      delay(1000);
      lcd.setCursor(0,1);
      lcd.print("  PONGA EL DEDO ");
    }
    digitalWrite(pinLed,LOW);
  }
  delay(10);
}

uint8_t registrarHuella(uint16_t idSocio) {

  int p = -1;
  Serial.print("Ponga el dedo que prefiera en el sensor.");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Huella tomada");
      break;
    case FINGERPRINT_NOFINGER:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comuncación. Compruebe la conexión USB o reinicie los sensores.");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Sensor sucio o huella pobre");
      break;
    default:
      Serial.println("Error desconocido");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Analizando huella...");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Sensor sucio o huella pobre.");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicación. Compruebe la conexión USB o reinicie los sensores.");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Error analizando huella del sensor. Reinicie los sensores.");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Error analizando huella del sensor. Reinicie los sensores.");
      return p;
    default:
      Serial.println("Error desconocido.");
      return p;
  }
  
  Serial.println("Quite el dedo del sensor para una segunda captura.");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("Socio número "); Serial.println(idSocio);
  p = -1;
  Serial.println("Ponga el mismo dedo otra vez en el sensor.");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Huella captada");
      break;
    case FINGERPRINT_NOFINGER:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicación. Compruebe la conexión USB o reinicie los sensores.");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Sensor sucio o huella pobre");
      break;
    default:
      Serial.println("Error desconocido.");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Analizando huella...");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Sensor sucio o huella pobre.");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de comunicación. Compruebe la conexión USB o reinicie los sensores.");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Error analizando huella del sensor. Reinicie los sensores.");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Error analizando huella del sensor. Reinicie los sensores.");
      return p;
    default:
      Serial.println("Error desconocido.");
      return p;
  }
  
  // OK converted!
  Serial.print("Creando modelo de huella para el socio número ");  Serial.println(idSocio);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Error de comunicación. Compruebe la conexión USB o reinicie los sensores.");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("La primera huella no coincide con la segunda. Intentelo de nuevo limpiando el dedo y/o el sensor.");
    return p;
  } else {
    Serial.println("Error desconocido.");
    return p;
  }   
  
  p = finger.storeModel(idSocio);
  if (p == FINGERPRINT_OK) {
    auxConcat="La huella del socio número ";
    auxConcat+=idSocio;
    Serial.println(auxConcat+" ha sido registrada con éxito.");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Error de comunicación. Compruebe la conexión USB o reinicie los sensores.");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("El número de socio indicado supera la capacidad del sensor. Indique uno menor.");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error almacenando la huella en el sensor. Reinicie los sensores y pruebe de nuevo.");
    return p;
  } else {
    Serial.println("Error desconocido.");
    return p;
  }   
}

int buscarHuella() { //Devuelve -1 si no encuentra huella, si no el Nº socio
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // Control de exactitud de huella
  //if(finger.confidence){}
  return finger.fingerID; 
}

uint8_t eliminarHuella(uint16_t id) {
  uint8_t p = -1;
  
  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    auxConcat="Huella del socio número ";
    auxConcat+=id;
    Serial.println(auxConcat+" borrada.");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Error de comunicación. Compruebe la conexión USB o reinicie los sensores.");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("El número de socio indicado no existe ya que se sale del rango máximo del sensor.");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error borrando la huella del socio. Intentelo de nuevo");
    return p;
  } else {
    Serial.print("Error desconocido: 0x"); Serial.println(p, HEX);
    return p;
  }   
}
