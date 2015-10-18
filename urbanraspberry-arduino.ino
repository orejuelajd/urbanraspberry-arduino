#include <Bridge.h>
#include <HttpClient.h>
#include <ArduinoJson.h>

HttpClient client;
int frecuencia = 0;
char c;
String id_vars[2];
String nombre_vars[2];
String pinesTexto_vars[2];

StaticJsonBuffer<300> jsonBuffer;

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

  /*
   * Se prende la luz roja del Yun (led 13) como señal para saber que inició Bridge
   */
  digitalWrite(13, HIGH);


  Serial.begin(9600);
  while (!Serial);

  /* 
   *  Se llama al servicio web de la Raspberry, pidiendo
   *  la lista de variables del equipo ARDUINOID
  */
  client.get("http://192.168.1.10/urbanraspberry/equipos/" + ARDUINOID);

  String equiposText = "";
  //String slashed = "";
  while (client.available()) {
    char c = client.read();
    if (c == '"') {
      //slashed += "\\";
    }
    equiposText += c;
    //slashed += c;
  }

  Serial.println(equiposText);
  JsonObject& equipos = jsonBuffer.parseObject(equiposText);

  if (!equipos.success())
  {
    Serial.println("parseObject() failed");
  } else {
    Serial.println("success!");
  }

  //Ejemplo del json que llega despues de hacer un GET.
  //{"id":"arduinoyunuao","nombre":"ArduinoYunUAO","variables":[{"id":"d41d8cd98f00b204e9800998ecf8427e","nombre":"temperatura","pines":["a0"],"pinesTexto":"a0"}],"frecuencia":null}

  //Captura el arreglo de las variables en un JsonArray&.
  JsonArray& variables = equipos["variables"].asArray();
  //variables.prettyPrintTo(Serial); // para imprimir las variables del dispositivo con formato json

  // Se recorre el JsonArray& con un iterator para obtener los valores que hay dentro del arreglo variables.
  int contador = 0;
  for (JsonArray::iterator it = variables.begin(); it != variables.end(); ++it) {
    const char* id_var = (*it)["id"];
    const char* nombre_var = (*it)["nombre"];
    JsonArray& pines_var = (*it)["pines"].asArray();
    const char* pinesTexto_var = (*it)["pines"];

    /*
    * En los siguientes arreglos se alamcenan los valores de cada variable para que en cualquier momento
    * se pueda acceder a ellas sin necesidad de recorrer todo el Array con un iterator.
    */ 
    id_vars[contador] = id_var;
    nombre_vars[contador] = nombre_var;
    pinesTexto_vars[contador] = pinesTexto_var;
  }

  //Obtenemos la frecuencia en que se debe realizar una lectura
  frecuencia = equipos["frecuencia"];
  //como aún la frecuencia es null se coloca un valor ficticio
  frecuencia = 20000;
}

void loop() {
  
  //Realizar un POST con el id de la variable numero 0 y el valor de la lectura de esa variable.
  post("http://192.168.1.10/urbanraspberry/datosensor", 0);
  Serial.flush();
  // La siguiente lectura se realizara después de que pase el tiempo que demarca la variable frecuencia.
  delay(frecuencia);
}

void post (String server, int i) {
  Process p;
  String cmd = "curl --data \"?variable_id="+id_vars[i]+"&value="+analogRead(A0)+ "\" "+server;
  Serial.println(cmd);
  p.runShellCommand(cmd);
  p.close();
}
