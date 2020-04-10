#include <LiquidCrystal_I2C.h>
#include <Key.h>
#include <Keypad.h>
#include <Wire.h>

//Asignación de pines
const byte pinRele = 10;
const byte ranks = 4;
const byte columns = 4;

//Variables para las interrupciones no bloqueantes


//variables para los códigos PIN
char keyPressed;
String codePressed = "";
String pinRobert = "1542";
byte digitNumber = 0;
int pinErrorLimit = 3;
int tryPinNumber = 0;

//Variables para los códigos PUK
bool pukControl = false;
String systemPuk = "12345678";

//Variables Para el buzzer
byte pinBuzzer = 12;

//Variables para las teclas y conversión de valor
char key;
int number = key - 48;

//Variables para la función timeOff() que apaga la pantalla después de un tiempo dado.
unsigned long timeControl;
unsigned long timeWorkingLimit = 10000;

//Variables de las teclas asociadas al teclado
byte pinRanks[ranks] = {9, 8, 7, 6};
byte pinColumns[columns] = {5, 4, 3, 2};

char keys[ranks][columns] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'},
  };

Keypad myKeyboard = Keypad(makeKeymap(keys),pinRanks, pinColumns, ranks, columns);

LiquidCrystal_I2C lcd(0x27, 16,2);



void setup() {
  Serial.begin(9600);
  pinMode(pinRele, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
  
  lcd.init();
  lcd.init();
  codeRequest();
  }

void loop(){
  keyPressed = myKeyboard.getKey();
 if (pukControl == false){
    if (keyPressed) { // Si se ha pulsado una tecla.
    if (keyPressed >= '0' && keyPressed <= '9' && digitNumber < 4){ //Si se pulsa una tecla entre 0 y 9 y hay menos de 4 dígitos pulsados.
      digitNumber ++;
      codePressed = codePressed + keyPressed;
      timeControl = millis(); //Asignamos a la variable el valor de millis para usarlo en la función timeOff-
    }
    if (keyPressed == 'A'){ //Si se Pulsa A ejecuta codeRequest.
       codeRequest();
    }
    if (keyPressed == 'B' && digitNumber > 0){
      delateNumber(); //Borra el último caracter siempre que haya alguno tecleado.
    }
    if (keyPressed == 'C'){ // Borra todos los caracteres introducidos.
      digitNumber = 0;
      codePressed = ""; 
    }
 //Este código evalua la clave introducida.        
    checkPinCode();
  }
 }else {
    if (keyPressed) { // Si se ha pulsado una tecla.
    if (keyPressed >= '0' && keyPressed <= '9' && digitNumber < 8){ //Si se pulsa una tecla entre 0 y 9 y hay menos de 4 dígitos pulsados.
      digitNumber ++;
      codePressed = codePressed + keyPressed;
      timeControl = millis(); //Asignamos a la variable el valor de millis para usarlo en la función timeOff-
    }
    if (keyPressed == 'A'){ //Si se Pulsa A ejecuta codeRequest.
       codeRequest();
    }
    if (keyPressed == 'B' && digitNumber > 0){
      delateNumber(); //Borra el último caracter siempre que haya alguno tecleado.
    }
    if (keyPressed == 'C'){ // Borra todos los caracteres introducidos.
      digitNumber = 0;
      codePressed = ""; 
    }
 //Este código evalua la clave introducida.        
    checkPukCode();
  }
 }
    
    timeOff();
  
}

// Es la instrucción inicial, deja la pantalla "apagada" hasta que se pulse una tecla
void codeRequest(){
      lcd.noBacklight();
      lcd.clear();
      myKeyboard.waitForKey();
      lcd.backlight();
      lcd.setCursor(3,0);
      lcd.print("BIENVENIDO");
      lcd.setCursor(3,1);
      lcd.print("Clave: ");
      timeControl = millis(); //Para la función timeOff
 }

void pukRequest(){
      lcd.noBacklight();
      lcd.clear();
      myKeyboard.waitForKey();
      lcd.backlight();
      lcd.setCursor(0,0);
      lcd.print("BLOQUEO PARCIAL");
      lcd.setCursor(2,1);
      lcd.print("PUK: ");
      timeControl = millis(); //Para la función timeOff
      
 }

// Mientras no se escriban 4 dígitos los imprime asteriscos en pantalla. 
void insertCode(){
      lcd.clear();
      lcd.backlight();
      lcd.setCursor(3,0);
      lcd.print("BIENVENIDO");
      lcd.setCursor(3,1);
      lcd.print("Clave: ");
      lcd.setCursor(10,1);
      if (digitNumber == 1){
        lcd.print("*");
      }else if (digitNumber == 2){
          lcd.print("**");
       }else if (digitNumber == 3){
          lcd.print("***");
        }
}


// Mientras no se escriban 8 dígitos los imprime asteriscos en pantalla. 
void insertPuk(){
      lcd.clear();
      lcd.backlight();
      lcd.setCursor(0,0);
      lcd.print("BLOQUEO PARCIAL");
      lcd.setCursor(2,1);
      lcd.print("PUK: ");
      lcd.setCursor(6,1);
      if (digitNumber == 1){
        lcd.print("*");
      }else if (digitNumber == 2){
          lcd.print("**");
       }else if (digitNumber == 3){
          lcd.print("***");
        }else if (digitNumber == 4){
          lcd.print("****");
        }else if (digitNumber == 5){
          lcd.print("*****");
        }else if (digitNumber == 6){
          lcd.print("******");
        }else if (digitNumber == 7){
          lcd.print("*******");
        }else if (digitNumber == 8){
          lcd.print("********");
        }
}

 //Compruba el PIN introducido, si es correcto activa correctCode() si no lo es activa errorCode() después resetea los contadores.
void checkPinCode(){
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

 //Compruba el PUK introducido, si es correcto activa codeRequest() si no lo es activa errorCode() después resetea los contadores.
void checkPukCode(){
     if (digitNumber == 8){ // Cuando se han introducido los 4 dígitos
       if (codePressed == systemPuk){ //Se comparan con la contraseña y si es correcta:
          codeRequest();
          pukControl = false;
          }else { // Si no es idéntica:
            errorPukCode();
            }
          codePressed = ""; //Se resetea la variable
          digitNumber = 0; //Se resetea la variable
          tryPinNumber = 0; //Se resetea la variable 
      }else { // Si aún no se han introducido 4 caracteres:
          insertPuk();
      }
  }

//Cuando se pone el pin correcto
void correctCode(){
      lcd.clear();
      lcd.backlight();
      lcd.setCursor(3,0);
      lcd.print("BIENVENIDO");
      lcd.setCursor(1,1);
      lcd.print("Correcto Pase!");
     
      digitalWrite(pinRele, HIGH);
      tone(pinBuzzer,2000,1000);
      delay(100);
      tone(pinBuzzer,4000,700);
      delay(1500);    
      digitalWrite(pinRele, LOW);
      tryPinNumber = 0;
      codeRequest();
  }

//Se ejecuta cuando se mete mal el pin menos de 3 veces
void errorCode(){
      lcd.clear();
      lcd.backlight();
      lcd.setCursor(5,0);
      lcd.print("CLAVE");
      lcd.setCursor(3,1);
      lcd.print("INCORRECTA");
      tone(pinBuzzer,700,1000);
      delay(100);
      tone(pinBuzzer,200,700);
      delay(3000);
      tryPinNumber++;
      if (pinErrorLimit == tryPinNumber){
        pukControl = true;
        pukRequest();
        }else{
      codeRequest();
        }
  }
  
//Se ejecuta cuando se mete mal el PUK
void errorPukCode(){
      lcd.clear();
      lcd.backlight();
      lcd.setCursor(3,0);
      lcd.print("BLOQUEO");
      lcd.setCursor(5,1);
      lcd.print("TOTAL");
      delay(5000);
      lcd.clear();
      lcd.backlight();
      delay(1000);
      lcd.noBacklight();
      delay(10000);
      pukRequest();

  }

void delateNumber(){
      digitNumber --;
      codePressed = codePressed.substring(0, digitNumber);
      lcd.backlight();
      lcd.setCursor(3,0);
      lcd.print("BIENVENIDO");
      lcd.setCursor(0,1);
      lcd.print(codePressed);
  }

//Evita que la pantalla se quede encendida
void timeOff(){
      if(timeControl!=0 && (millis()-timeControl)>timeWorkingLimit){
           codeRequest();
           digitNumber = 0;
           codePressed = "";
           
      }
 }
