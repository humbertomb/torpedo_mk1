// Programa para armado y disparo del torpedo
//
// Programacion Arduino: Antonio Ruis Navarro
// Diseño mecánico: Humberto Martínez Barberá

#define pulsador1Pin A0
#define motorPin 9
#define arduinoLED 13

#define cicloRefresco 500  // Refresh period in millisecond 
#define cicloActualizacion 10  // Duración de cada ciclo de actualización en ms.
#define retardoPulsador 6  //Retardo en numero de ciclos para realizar el antirrebotes en el pulsados

#define nivelAlto 545  // Nivel de la lectura analógica del pulsador a partir de la que interpretamos pulsador activado.
#define nivelBajo 429  // Nivel de la lectura analógica del pulsador a partir de la que interpretamos pulsador desactivado.
#define fuerzaMotor 127 // 0..255 fuerza del motor
#define tiempoMinimoPropulsion 750 // Tiempo minimo de propulsion en milisegundos, un tiempo de pulsación menor que este durante la programacion de tiempo de propulsion apaga el sistema sin disparar.
#define tiempoTimeOut 60000L// Tiempo en milisegundos antes de apagarse el sistema si no actuamos en el pulsador.
#define tiempoFeedback 10  // Durancion del feedback del motor. Poner a cero si no queremos funcion de feedback con el motor.

#include <EEPROM.h>

// VARIABLES DE CONTROL DE PULSADORES //
int vecesDetectadoPulsador; // QUIZA PODRIA SER CHAR en lugar de INT
bool pulsadorActivo;

// VARIABLES DE TEMPORIZADORES //
unsigned long milisegundos=0;
unsigned long milisegundosCiclo=0;
int contadorLED;
byte destelloActual;
long timeOut;
int temporizador;


// VARIABLES DE PROGRAMA //
int duracionPropulsion; //Duración en ms de actuación del motor de de propulsion.
int estado;


void setup(){
  vecesDetectadoPulsador=0;
  pulsadorActivo = false;
  contadorLED = 0;
  destelloActual = 0;
  duracionPropulsion = 0;
  timeOut = 0;
  estado = 0;
  temporizador = 1000;  // Tiempo de espera inicial 1 segundo.
  Serial.begin(115200);  // Set serial monitor baud rate
  pinMode(arduinoLED,OUTPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(pulsador1Pin, INPUT);
  Serial.println("Iniciando");
  feedback(tiempoFeedback);
  temporizador = 1000;  // Tiempo de espera inicial 1 segundo.
  estado = -1;
//  delay(1000);
  Serial.println("Ok");
}


void loop(){
  unsigned long currentMillis = millis();
  if (currentMillis - milisegundos > cicloRefresco) {  // Este codigo se ejecuta cada ciclo de refresco para mostrar informacion de depuración.
    milisegundos = currentMillis;  // Otra opcion es: milisegundos += cicloRefresco; (la diferencia entre las dos es muy sutil)
    Serial.print(analogRead(pulsador1Pin));
    Serial.print(" ");
    Serial.println(pulsadorActivo,BIN);
  }

     
  if (currentMillis - milisegundosCiclo > cicloActualizacion) { // Se ejecuta cada ciclo de Actualizacion.
//    milisegundosCiclo += cicloActualizacion;
    milisegundosCiclo = currentMillis;
    comprobarPulsadores();
    timeOut += cicloActualizacion;
//    digitalWrite(arduinoLED,pulsadorActivo);
    switch (estado) {
      case -1:  // Espera inicial tras la activacion del sistema.
        temporizador -= cicloActualizacion;
        if (temporizador <= 0) {
          estado = 0;
        }
        break;
      case 0:  // Inicio esperando a la pulsacion.
        if (pulsadorActivo) {  // Si se activa el pulsador pasamos a contar el tiempo.
          estado = 1;
          Serial.println("Contando tiempo de propulsion");
          feedback(tiempoFeedback);
          timeOut = 0;
        } else { // Si se ha soltado el pulsador desde que iniciamos entendemos que queremos usar la misma duracion de la ultima vez.
          duracionPropulsion = leerEeprom();
          Serial.print("Lectura del tiempo de propulsion de la EEPROM: ");
          Serial.println(duracionPropulsion);
          if (duracionPropulsion < tiempoMinimoPropulsion) { // Si no hay almacenado ningun tiempo de disparo anterior o el tiempo es muy corto damos un tiempo minimo.
            duracionPropulsion = tiempoMinimoPropulsion/2;
          }
          estado = 2;
        }
        break;
      case 1:  // Programando tiempo de propulsion
        if (pulsadorActivo) {  // Si se activa el pulsador vamos incrementando el tiempo.
          duracionPropulsion += cicloActualizacion;
        } else {
          estado = 2;
          Serial.print("Tiempo de propulsion: ");
          Serial.println(duracionPropulsion);
          feedback(tiempoFeedback);
          timeOut = 0;
          if (duracionPropulsion < tiempoMinimoPropulsion) { // Si el tiempo es muy corto entendemos que queremos apagar en lugar de disparar.
            Serial.println("Tiempo de propulsion muy corto");
            estado = 100;
          } else {
            guardarEeprom(duracionPropulsion);
          }
        }
        break;
      case 2:  // Esperando recibir la orden de disparo
        if (pulsadorActivo) {  // Si se activa el pulsador pasamos a orden de disparo.
          estado = 3;
          Serial.println("Disparo");
          timeOut = 0;
          // &&&&&&&&&&&&&&& Igual hay que poner aquí un delay para retrasar un poco el disparo &&&&&&&&&&&&&&&&&&
        }
        break;
      case 3:  // Propulsando con pulsador todavía activo.
        analogWrite(motorPin, fuerzaMotor);
        duracionPropulsion -= cicloActualizacion;
        if (!pulsadorActivo) estado = 4;  // Cuando se libera la pulsacion seguimos en propulsion.
        if (duracionPropulsion <= 0) {  // Cuando se pasa el tiempo de propulsion, apagamos.
          estado = 100;
        }
        break;
      case 4:  // Propulsando con pulsador desactivado.
        analogWrite(motorPin, fuerzaMotor);
        duracionPropulsion -= cicloActualizacion;
        if (pulsadorActivo) estado = 100;  // Si se vuelve a dar una pulsacion durante la propulsion, apagamos.
        if (duracionPropulsion <= 0) {  // Cuando se pasa el tiempo de propulsion, apagamos.
          estado = 100;
        }
        break;
      case 100:  // Apagar alimentacion
        digitalWrite(motorPin, LOW);
        Serial.println("Apagando...");
        delay(1000);
        Serial.println("Apagado");
        pinMode(pulsador1Pin,OUTPUT);
        digitalWrite(pulsador1Pin,LOW);  // Apagamos la alimentacion.
        delay(5000);
        softReset(); // No debería ejecutarse salvo que estemos alimentando el Arduino por el USB.
        break;
      default:
        estado = 100;  // Si llegamos a un estado incorrecto apagamos igualmente.
        break;
    }
    if (timeOut >= tiempoTimeOut) {  // Si hemos superado el tiempo de timeOut sin hacer nada notificamos y apagamos.
      feedback(tiempoFeedback);
      estado = 100;
    }
    notificarLED(estado);
  
//    Serial.print(pulsadorActivo,DEC);
//    Serial.print(", ");
//    Serial.println(tiempoActivoPulsador,DEC);
//    if (pulsadorActivo) {
//      Serial.println("Apagando...");
//    }

    
  }

 }


 // --------------------------------- Otras funciones ------------------------------------------

void comprobarPulsadores(){ // Comprobar los pulsadores, lo ejecutaremos periodicamente y realiza el antirebotes
//  byte estado = B00000000;
//  estado |=  !digitalRead(pulsador1Pin);
//  Serial.println(estado,BIN);
//  if ((estado == estadoAnterior) & (estado != B00000000)) {
  int lectura = analogRead(pulsador1Pin);
  if (lectura > nivelAlto) {  // Si se ha pulsado el pulsador...
    vecesDetectadoPulsador ++;
  } else if (lectura < nivelBajo) {  // Si se ha desactivado el pulsador...
    vecesDetectadoPulsador -=2;  // Podemos dar mas prioriad a un estado que a otro
  } else {  // Y en la zona de la histéresis...
    // Ruido. No hacemos nada salvo que queramos cuantificar lo ruidosa que es la medida.
  }
  if (vecesDetectadoPulsador > retardoPulsador) {
    pulsadorActivo = true;
    vecesDetectadoPulsador = retardoPulsador;
  }
  if (vecesDetectadoPulsador < retardoPulsador*(-1)) {
    vecesDetectadoPulsador = retardoPulsador*(-1);
    pulsadorActivo = false;
  }
  
}

void notificarLED(byte indicacion) {
  contadorLED++;
  if (contadorLED == (destelloActual * 16 + 8)){
    digitalWrite(arduinoLED, HIGH);
    destelloActual++;
    if (destelloActual > indicacion) destelloActual = 0;
  } else digitalWrite(arduinoLED, LOW);
  if (contadorLED > (indicacion * 16 + 80)) {
    contadorLED = 0;
    destelloActual = 0;
  }
}

void feedback(int milisegundos) {
  if (milisegundos > 0) {
    analogWrite(motorPin, fuerzaMotor);
    delay(milisegundos);
//    analogWrite(motorPin, 0);
    digitalWrite(motorPin, LOW);
  }
}

void softReset() {
  setup();
}

int leerEeprom(){  // Devuelve el tiempo de propulsion almacenado en la EEPROM si no tiene tiempo bien memorizado -1.
  int duracion = -1;
  if ((EEPROM.read(0) == 69) && (EEPROM.read(1) == 12) && (EEPROM.read(2) == 13) && (EEPROM.read(3) == 18)) {
    // Comprobamos que la eeprom tenga una configuracion valida con numeros concretos
    if ((EEPROM.read(4) == EEPROM.read(6)) && (EEPROM.read(5) == EEPROM.read(7))) {
      EEPROM.get(4,duracion);  // solo cargamos el valor de la configuracion si los valores coinciden.
    } else return -1;
  }
  return duracion;
}

void guardarEeprom(int duracion){
  EEPROM.update(0,69);
  EEPROM.update(1,12);
  EEPROM.update(2,13);
  EEPROM.update(3,18);
  // Ponemos numeros concretos en el comienzo de la EEPROM para confirmar que tiene valores correctos.
  EEPROM.put(4,duracion);
  EEPROM.put(6,duracion);  // almacenamos los valores 2 veces
}
