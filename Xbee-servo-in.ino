#include <Servo.h>
#include <math.h>
Servo servo;
int GPMMAnterior = 0;

int GPM = 0;
int GPMM = 60;
int servoDegrees = 0;
int autoControl = 0;

int GPMr = 0;
int GPMMr = 0;
int servoDegreesr = 0;
int autoControlr = 0;

float res = 0;
int grados=90;
int propFactor = 1;

//Demostracion manual
bool automatico = true;
int signo = 1;

void setup() {
  //Inicializa servomotor
  servo.attach(4);
  //Remember that the baud must be the same on both arduinos
  Serial.begin(9600);
}

void loop() {
  //Prueba del giro
  automatico = false;
  
  //Si se recibio un dato del xbee emisor
  while(Serial.available()) {
    GPMMAnterior = GPMM; //Se guarda en una variable temporal
    
    //Se reciben informacion del webserver por xbee
    char readChar = Serial.read();
    //Serial.println("beginToRead:"+readChar);
    if(readChar == '{'){
      GPMr = recibirPorByte(); //Se recibe las Gotas por minuto objetivo
      GPMMr = recibirPorByte();//Se recibe la medicion recibida por el servidor
      servoDegreesr = recibirPorByte();//Se recibe la medicion recibida por el servidor
      autoControlr = recibirPorByte();//Se recibe la medicion recibida por el servidor
    }
    
    if(GPMr>=0 && GPMMr>=0 && (servoDegreesr>=10 && servoDegrees<=170) && (autoControlr==0 || autoControlr==1) ){
      GPM = GPMr;
      GPMM =GPMMr;
      servoDegrees =servoDegreesr;
      autoControl =autoControlr;
    }
    
    //Serial.print("GPMM Rec ");
    //Serial.println(GPMM);
    if(GPMM<0 || GPMM > 90){ //Se evita guardar mediciones que sobrepasan los rangos permitidos
      GPMM = GPMMAnterior;
    }
    //Resta de comparación entre deseado y medido
    res = GPM - GPMM;
  }//fin de while que recibe los datos
  
  
  if(autoControl == 1){ //Control automatico
    /*Algoritmo para calcular el movimiento proporcional a la distancia para hacer correccion*/
    propFactor = (1+ceil(abs(res)/5)); //Factor de proporcional para ordenar el giro de la valcula
    if(grados-propFactor>=0 || grados+propFactor<=180){
      //Tolerancia de +-1 gota por minuto
      if(res < -1 && grados<180){//Si se quedo corto
        grados=grados+propFactor;
      }
      
      if(res > 1 && grados>20){//Si se paso
        grados=grados-propFactor;
      }
    }
    //Codigo de prueba, mueve motor de 10 en 10
    /*grados=grados + signo*10;
    if(grados>=170 || grados<=10){ //Si llega al tope, cambia de sentido
      signo = signo*-1;
    }*/
  }else if(autoControl == 0){ //Control manual
    //Codigo de prueba, mueve motor de 10 en 10
    grados=servoDegrees;
  }
  
  /*******ALGORITMO DE CALIBRACION DE GPM AUTOMATICA************/
  Serial.println("GPM "+String(GPM)+" | servoDegrees "+String(servoDegrees)+" | autoControl "+String(autoControl)+" | GPMM "+String(GPMM)+" | RES "+String(res)+" | Grados "+String(grados));
  
  servo.write(grados);
  delay(400);
  /**************************************************/
}

bool beginToRead(){
  char readChar = Serial.read();
  bool res = readChar == '{';
  Serial.print("beginToRead:"+res);
  return res;
}

//Recibe 2 bytes seguidos para unirlos como un unico numero de 16bytes
int recibirPorByte(){
  uint16_t first_half = Serial.read();
  uint16_t sencond_half = Serial.read();
  first_half = first_half<<8;
  int number = first_half | sencond_half;
  return number;
}
