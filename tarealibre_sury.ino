/* Autor: Surya N. Stasiak Vega
*  
* El arduino sirve de traductor de lenguaje escrito a codigo morse
* Incluye ajuste de velocidad
*
* DISTRIBUCION DE PINES:
* - LED en el pin 7
* - Potenciometro en el pin Analog 0
*
*/

/*
  Relacion de tiempos segun la convencion del codigo morse (tal como se explica en https://en.wikipedia.org/wiki/Morse_code):
  - punto:                                          un intervalo de tiempo
  - linea:                                          tres intervalos de tiempo
  - espacio dado entre simbolos de cada letra:      un intervalo
  - espacio entre letras:                           tres intervalos
  - espacio entre palabras:                         siete intervalos

*/

// definicion de pines
#define led 7
#define pot A0

// para deshabilitar el ajuste del tiempo descomentar esta linea:
//#define timeunit 100

// definicion de variables
String myinput;
int status;
int potval = 0;
int speed;
int timeunit = 100;


void setup() {
  // inicio del puerto serie
  Serial.begin(9600);
  Serial.setTimeout(10);

  // definicion del puerto de la led
  pinMode(led, OUTPUT);

}

// funcion que se usa para traducir el codigo ascii dado por el puerto serial a codigo morse
String ascii_to_morse(int asciicode) {
  String mystring;
  switch (asciicode) {
  case 97:
  mystring = ".-";
  break;
  case 98:
  mystring = "-...";
  break;
  case 99:
  mystring = "-.-.";
  break;
  case 100:
  mystring = "-..";
  break;
  case 101:
  mystring = ".";
  break;
  case 102:
  mystring = "..-.";
  break;
  case 103:
  mystring = "--.";
  break;
  case 104:
  mystring = "....";
  break;
  case 105:
  mystring = "..";
  break;
  case 106:
  mystring = ".---";
  break;
  case 107:
  mystring = "-.-";
  break;
  case 108:
  mystring = ".-..";
  break;
  case 109:
  mystring = "--";
  break;
  case 110:
  mystring = "-.";
  break;
  case 111:
  mystring = "---";
  break;
  case 112:
  mystring = ".--.";
  break;
  case 113:
  mystring = "--.-";
  break;
  case 114:
  mystring = ".-.";
  break;
  case 115:
  mystring = "...";
  break;
  case 116:
  mystring = "-";
  break;
  case 117:
  mystring = "..-";
  break;
  case 118:
   mystring = "...-";
  break;
  case 119:
  mystring = ".--";
  break;
  case 120:
  mystring = "-..-";
  break;
  case 121:
  mystring = "-.--";
  break;
  case 122:
  mystring = "--..";
  break;

  case 32: // espacio
    mystring = "w";
    break;
  }
  // se devuelve el codigo para cada letra
  return mystring;
}

// funcion dedicada a la ejecucion del codigo en la LED
void blink_leds(String morsecode) {
  // se manda el codigo al puerto serial por motivos de depuracion
  Serial.print("[INFO] Started transmition: ");
  Serial.println(morsecode);

  // se cicla a traves de cada uno de los componentes de cada mensaje en codigo morse
  for (int i = 0; i<morsecode.length(); i++) {
    // se enciende la LED
    digitalWrite(led, HIGH);

    // segun el simbolo, se espera o un intervalo de tiempo o tres
    if (morsecode.charAt(i) == '.') {
      delay(1*timeunit);
    } else if (morsecode.charAt(i) == '-') {
      delay(3*timeunit);
    } else if (morsecode.charAt(i) == 'w') {
      digitalWrite(led, LOW);
      delay(4*timeunit); // 5 + 1 + 2 (del final de la funcion) dan 7 intervalos de tiempo = espacio entre palabras
    } else {
      // por si acaso se encuentra algun error
      Serial.print("[ERROR] Corrupted morsecode string: ");
      Serial.println(morsecode);
    }
    // despues de esperar el periodo de tiempo elegido, se vuelve a apagar la LED
    digitalWrite(led, LOW);
    // se espera un intervalo y se sigue con el siguiente simbolo
    delay(1*timeunit);
  }
  // se esperan dos tiempos mas, para llegar al tiempo total de tres intervalos.
  delay(2*timeunit);
  Serial.println("[INFO] Finished transmition.");
}

// loop principal
void loop() {
  if (Serial.available()>0) {
    int myinput = Serial.read();    // devuelve cada letra de el mensahje por separado, en codigo ascii

    if (myinput == 42) {
      // IMPORTANTE: hay mucha inexactitud entre los valores dados por el potenciometro dado que es un potenciometro de baja calidad
      // se implementa una funcion normalizadora, aun asi, puede interferir en la exactitud de los intervalos del mensaje.

      // para combatir esto, se usa el comando especial "*" con el que se pone como intervalo de tiempo base el que se muestra por el potenciometro 
      // en el instante en el que el comando es mandado.

      potval = analogRead(pot); // se lee el valor del potenciometro
      speed = 1023 - potval; // speed es la inversion de potval
      timeunit = speed/5; // se cambia el intervalo de tiempo base

      Serial.print("Set new base time interval to: "); // se describe el cambio en el serial
      Serial.print(timeunit);
      Serial.println(" ms.");

    } else if (myinput != 10 && myinput != 0) {              // se pasa si el caracter es NULL o BACKSPACE
      Serial.print("[INFO] Serial input: "); Serial.print(myinput); Serial.print(" ("); Serial.print(char(myinput)); Serial.println(")");
      String morsecode = ascii_to_morse(myinput);     // se convierte el codigo ascii al codigo morse
      blink_leds(morsecode);                          // se reproduce el codigo morse en el diodo LED
    }
  } else {
    delay(1);  // si no sucede nada, se espera un ms 
  }
}