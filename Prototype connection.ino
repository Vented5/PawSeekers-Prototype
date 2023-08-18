#define SIM800L_IPS306_VERSION_20200811
#include <SoftwareSerial.h>
#include <String.h>

SoftwareSerial Sim800(3, 2); //tx, rx
String collarId = "64de07df0a3202d95d48bc63";

void setup() {
  // put your setup code here, to run once:
  Sim800.begin(19200);
  Serial.begin(19200);
  Serial.println("Empezando");
  delay(20000);
}

void loop() {
  // put your main code here, to run repeatedly:
  startConnection();
  String data = getLocation();
  
  int comaIndex = 0;
  int prevComaIndex = 0;
  String dataArray[5];
  for(int i = 0; i < 5; i++) {
    comaIndex = data.indexOf(',', prevComaIndex);
    String segment = data.substring(prevComaIndex, comaIndex);
    dataArray[i] = segment;

    Serial.println(segment);
    prevComaIndex = comaIndex + 1;
  }
  
  float latLng = dataArray[0].toFloat();
  float lat = dataArray[1].toFloat();;
  float lng = dataArray[2].toFloat();;
  String date = dataArray[3];
  String time = dataArray[4];  
  
  transferData(lat, lng);
  if(Sim800.available())//Verificamos si hay datos disponibles desde el SIM900
  Serial.write(Sim800.read());//Escribir datos
  
}

void startConnection(){
  Sim800.println("AT+CIPSTATUS");
  delay(2000);
  Sim800.println("AT+CIPMUX=0");
  delay(3000);
  Sim800.println("AT+CSTT=\"internet.itelcel.com\",\"webgprs\",\"webgprs2002\"");
  delay(1000);
  Sim800.println("AT+CIICR");//REALIZAR UNA CONEXIÓN INALÁMBRICA CON GPRS O CSD
  delay(3000);
  mostrarDatosSeriales();
  Sim800.println("AT+CIFSR");// Obtenemos nuestra IP local
  delay(2000);
  mostrarDatosSeriales();
 
}

String getLocation () {
  Sim800.println("AT+GF");
  delay(2000);
  
  String data;
  if(Sim800.available()){
    data = Sim800.read();
    Serial.println("Data: ");
    Serial.println(data);
  } else {
   data = "Error al obtener los datos.";
  }
  return data;
}

void transferData(float lat, float lng){
  Sim800.println("AT+CIPSPRT=0");//Establece un indicador '>' al enviar datos
  delay(3000);
  mostrarDatosSeriales();
  Sim800.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");//Indicamos el tipo de conexión, url o dirección IP y puerto al que realizamos la conexión
  delay(6000);
  mostrarDatosSeriales();
  Sim800.println("AT+CIPSEND");//ENVÍA DATOS A TRAVÉS DE una CONEXIÓN TCP O UDP
  delay(4000);
  mostrarDatosSeriales();
  String datos="GET https://paw-seekers-api-vented5.vercel.app/api/collars/update/"+ collarId + "/" + String(lat) +"/"+ String(lng) ;
  Sim800.println(datos);//Envía datos al servidor remoto
  delay(4000);
  mostrarDatosSeriales();
  Sim800.println((char)26);
  delay(5000);//Ahora esperaremos una respuesta pero esto va a depender de las condiones de la red y este valor quizá debamos modificarlo dependiendo de las condiciones de la red
  Sim800.println();
  mostrarDatosSeriales();
  Sim800.println("AT+CIPSHUT");//Cierra la conexión(Desactiva el contexto GPRS PDP)
  delay(5000);
  mostrarDatosSeriales(); 
}

void mostrarDatosSeriales()//Muestra los datos que va entregando el sim900
{
  while(Sim800.available()!=0)
  Serial.write(Sim800.read());
}