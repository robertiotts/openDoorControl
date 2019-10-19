#include <LiquidCrystal_I2C.h>
#include <Key.h>
#include <Keypad.h>
#include <Wire.h>

//Asignación de pines
const byte pinRele = 10;
const byte ranks = 4;
const byte columns = 4;

//Variables para las interrupciones no bloqueantes


//variables para los códigos
char keyPressed;
String codePressed = "";
String pinRobert = "1542";
byte digitNumber = 0;

//Variables para las teclas y conversión de valor
char key;
int number = key - 48;

//Variables de tiempo para reiniciar
unsigned long timeOn;
unsigned long timeVariation;
unsigned long preTime;
unsigned long workingTimeLimit = 10000;

byte pinRanks[ranks] = {9, 8, 7, 6};
byte pinColumns[columns] = {5, 4, 3, 2};

char keys[ranks][columns] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'},
  };

Keypad myKeyboard = Keypad(makeKeymap(keys),pinRanks, pinColumns, ranks, columns);

LiquidCrystal_I2C lcd(0x27, 20,4);

void setup() {
  Serial.begin(9600);
  pinMode(pinRele, OUTPUT);
  
  lcd.init();
  lcd.init();
  codeRequest();
  
    
  }

void loop(){

  keyPressed = myKeyboard.getKey();
  if (keyPressed) { // Si se ha pulsado una tecla.
    if (keyPressed >= '0' && keyPressed <= '9' && digitNumber < 4){ //Si se pulsa una tecla entre 0 y 9 y hay menos de 4 dígitos pulsados.
      digitNumber ++;
      codePressed = codePressed + keyPressed;
        }
    if (keyPressed == 'A'){ //Si se Pulsa A ejecuta codeRequest.
       codeRequest();
       digitNumber = 0;
       codePressed = "";
        }
    if (keyPressed == 'B' && digitNumber > 0){
      delateNumber(); //Borra el último caracter siempre que haya alguno tecleado.
        }
    if (keyPressed == 'C'){ // Borra todos los caracteres introducidos.
      digitNumber = 0;
      codePressed = ""; 
        }
 //Este código evalua la clave introducida.        
      if (digitNumber == 4){ // Cuando se han introducido los 4 dígitos
        if (codePressed == pinRobert){ //Se comparan con la contraseña y si es correcta:
          correctCode();
            }else { // Si no es idéntica:
              errorCode();
                }
                codePressed = ""; //Se resetea la variable
                digitNumber = 0; //Se resetea la variable
                  }else { // Si aún no se han introducido 4 caracteres:
                    insertCode();
                      }
    }
  
  }

// Es la instrucción inicial, deja la pantalla "apagada" hasta que se pulse una tecla
void codeRequest(){
 lcd.noBacklight();
 lcd.clear();
 myKeyboard.waitForKey();
 lcd.backlight();
 lcd.setCursor(5,0);
 lcd.print("BIENVENIDO");
 lcd.setCursor(0,2);
 lcd.print("Introduzca su clave:");
  }

// Mientras no se escriban 4 dígitos imprime asteriscos en pantalla. 
void insertCode(){
 lcd.clear();
 lcd.backlight();
 lcd.setCursor(5,0);
 lcd.print("BIENVENIDO");
 lcd.setCursor(0,2);
 lcd.print("Introduzca su clave:");
 lcd.setCursor(8,3);
 if (digitNumber == 1){
  lcd.print("*");
  }else if (digitNumber == 2){
    lcd.print("**");
    }else if (digitNumber == 3){
      lcd.print("***");
      }
  }

// Cuando el código introducido coresponde con la clave elegida
void correctCode(){
 lcd.clear();
 lcd.backlight();
 lcd.setCursor(5,0);
 lcd.print("BIENVENIDO");
 lcd.setCursor(0,2);
 lcd.print("Correcto! Adelante.");
       
 digitalWrite(pinRele, HIGH);
 delay(1500);    
 digitalWrite(pinRele, LOW);
 codeRequest();
  }

// Cuando el código introducido NO corresponde con la clave
void errorCode(){
 lcd.clear();
 lcd.backlight();
 lcd.setCursor(0,0);
 lcd.print("********************");
 lcd.setCursor(5,1);
 lcd.print("INCORRECTO");
 lcd.setCursor(0,2);
 lcd.print("VUELVA A INTENTARLO");
 lcd.setCursor(0,3);
 lcd.print("********************");
 delay(3000);
 codeRequest();
  }

// Borra dígito a dígito introducido
void delateNumber(){
 digitNumber --;
 codePressed = codePressed.substring(0, digitNumber);
 lcd.backlight();
 lcd.setCursor(5,0);
 lcd.print("BIENVENIDO");
 lcd.setCursor(0,2);
 lcd.print(codePressed);
  }

void timeOff(){
  timeVariation = millis() - preTime;
  preTime = millis();
  if (timeVariation > workingTimeLimit){
    codeRequest();
    }
 }
