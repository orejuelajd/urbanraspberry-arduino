#include <Bridge.h>
#include <HttpClient.h>
#include <ArduinoJson.h>

HttpClient client;
int variable_id = 0;
int value = 0;
int frecuencia = 0;
char c;
StaticJsonBuffer<1000> jsonBuffer;

/*
 * ARDUINOID define el id que tiene el
 * presente arduino, que se relaciona
 * con el id del dispositivo agregado
 * en la interfaz gráfica de la Raspberry
*/
String ARDUINOID = "arduinoyunuao";

void setup() {

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();

  //Se prende la luz roja del Yun (led 13)
  //como señal para saber que inició Bridge
  digitalWrite(13, HIGH);


  Serial.begin(9600);
  while (!Serial);

  //Se llama al servicio web de la Raspberry, pidiendo
  //la lista de equipos disponibles
  client.get("http://192.168.1.10/urbanraspberry/equipos/" + ARDUINOID);

  String equiposText = "";
  String slashed = "";
  while (client.available()) {
    char c = client.read();
    if (c == '"') {
      slashed += "\\";
    }
    equiposText += c;
    slashed += c;
  }

  Serial.println(equiposText);
  JsonObject& equipos = jsonBuffer.parseObject(equiposText);

  if (!equipos.success())
  {
    Serial.println("parseObject() failed");
  } else {
    Serial.println("success!");
  }

  //{"id":"arduinoyunuao","nombre":"ArduinoYunUAO","variables":[{"id":"d41d8cd98f00b204e9800998ecf8427e","nombre":"temperatura","pines":["a0"],"pinesTexto":"a0"}],"frecuencia":null}

  //Obtenemos la frecuencia en que se debe realizar una lectura
  //frecuencia = equipos["frecuencia"];
  //Serial.println(frecuencia);

  JsonArray& variables = equipos["variables"].asArray();
  variables.prettyPrintTo(Serial);

  for (JsonArray::iterator it = variables.begin(); it != variables.end(); ++it){
    const char* sensor = (*it)["id"];
    Serial.println(sensor);
  }
}



void loop() {

  /* variable_id = equipos["vari"];
   value = equipos["pines"];
   Serial.print(value);

   */

  Serial.flush();
  delay(2000);
}
