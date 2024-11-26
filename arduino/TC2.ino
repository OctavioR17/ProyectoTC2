#include <SPI.h>
#include <WiFi.h>
#include <MFRC522.h>
#include <HTTPClient.h>
#include "ESP32Servo.h"
#include <WebSocketsServer.h>

// Pines y configuraciones para la alarma
const int contra = 1234;
const int ledVerde = 12;
const int ledRojo = 14;  // azul
const int bocina = 27;
const int sensor = 2;

bool estado = false;
boolean pregunta = true;
boolean fin = false;

bool estadosensor = true; // estado del sensor de movimiento

int urlcompare;
int servo_pin = 34;
int angle = 0;

String urlTemporal = "";
String urlCaracteres;

Servo myservo;

WebSocketsServer webSocket(81);

WiFiClient client;
char server[] = "192.168.50.130";

// Pines y configuraciones para RFID
const int SDA_PIN = 16, RST_PIN = 4;
MFRC522 rfid(SDA_PIN, RST_PIN);
String nombre;  // Almacena el nombre del servidor
String codigo;

// Configuración Wi-Fi y servidor
const char* ssid = "RT-AC1200_28_2G";
const char* password = "feregrinos123456789";
const char* host = "192.168.50.130";

// Funciones
void activarAlarma();
void detenerAlarma();

void setup() {
  Serial.begin(921600);
  delay(1000);
  myservo.attach(servo_pin);
  Serial.println();  //conectar al wifi
  Serial.print("conectando a");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println(WiFi.localIP());


  // Configuración de pines
  pinMode(ledVerde, OUTPUT);
  pinMode(ledRojo, OUTPUT);
  pinMode(sensor, INPUT);
  pinMode(bocina, OUTPUT);

  // Configuración RFID
  SPI.begin(21, 19, 23, 16);
  rfid.PCD_Init();
  Serial.println("Sistema listo. Coloque la tarjeta o ingrese código.");
  digitalWrite(ledRojo, LOW);
  digitalWrite(ledVerde, HIGH);
  delay(3000);


  // Iniciar servidor webSocket
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

}

void loop() {
  webSocket.loop();

  Serial.println("conectando a");
  Serial.println(host);
  //crear una instancia wificliente
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("conexion fallida");
  }
  
  if (!estado) {
    Serial.println("Ingrese el código o use el RFID para activar la alarma:");
    if (Serial.available()) {
      int codigoIngresado = Serial.parseInt();
      if (codigoIngresado == contra) {
        estado = true;
        Serial.println("ALARMA ACTIVADA");
        digitalWrite(ledRojo, HIGH);
        digitalWrite(ledVerde, LOW);
        for (angle = 0; angle <= 180; angle++) {
          myservo.write(angle);
          delay(15);
        }
      }
    } else if (rfid.PICC_IsNewCardPresent()) {
      Serial.println("Tarjeta detectada, intentando leer...");
      // Seleccionar una tarjeta
      if (rfid.PICC_ReadCardSerial()) {
        Serial.println("UID de la tarjeta:");
        for (byte i = 0; i < rfid.uid.size; i++) {
          urlCaracteres += rfid.uid.uidByte[i];
        }
        Serial.println(urlCaracteres);
        String urlTemporal = urlCaracteres;
        String data = "tarjeta=" + urlTemporal;
        Serial.println(data);
        urlCaracteres = "";
        Serial.println();
        //Comprobamos si tenemos datos en el puerto serie
        if (pregunta == true) Serial.print("Escribe el identificador de la persona: ");
        pregunta = false;
        //leemos el identificador
        urlcompare = urlTemporal.toInt();
        int identificador = urlcompare;
        Serial.println(identificador);
        //llamamos a la función quenos permitira comunicarnos con el servidor
        httpRequest(identificador);
        pregunta = true;
        //------------------------------------------------------------------------------------
        // Comparar los UID para determinar si es uno de los usuarios registrados
        if (urlTemporal == nombre) {
          Serial.println("Usuario Permitido");
          Serial.println("Alarma activada por RFID");
          digitalWrite(ledVerde, HIGH);  // Encender el LED verde
          delay(4000);
          digitalWrite(ledRojo, LOW);   // Apagar el LED verde
          digitalWrite(ledVerde, LOW);  // Encender el LED verde
          digitalWrite(ledRojo, LOW);   // Apagar el LED verde
          tone(bocina, 500, 600);
          digitalWrite(ledRojo, HIGH);
          digitalWrite(ledVerde, LOW);
          delay(500);
          estado = true;
          for (angle = 0; angle <= 180; angle++) {
            myservo.write(angle);
            delay(15);
          }

          if (!client.connect(host, httpPort)) {
            Serial.println("conexion fallida");
          }
          // crear direecion POST
          HTTPClient http;
          String url = "http://192.168.50.130/ControlAcceso/registro_date.php";
          http.begin(url);
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");
          String postData = "tarjeta=" + nombre;


          int httpCode = http.POST(postData);
          String payload = http.getString();
          Serial.print("URL : ");
          Serial.println(url);
          Serial.print("Data: ");
          Serial.println(postData);
          Serial.print("httpCode: ");
          Serial.println(httpCode);
          Serial.print("payload: ");
          Serial.println(payload);
          Serial.println("---------------------------------------------------------");
          Serial.println("Pidiendo al servidor URL");

          /*client.print(String("POST ") + url + "HTTP1.0\r\n" +
        "Host: "+host + "\r\n" +
        "Accept: *" + "/" + "*\r\n" +
        "Content-Length: " + data.length() + "\r\n" +
        "Content-Type: application/x-www-form-urlencoded\r\n" +
        "\r\n" + data);
*/
          delay(10);
          Serial.println("El servidor respondio");
          while (client.available()) {
            String line = client.readStringUntil('\r');
            Serial.println(line);
          }
          Serial.println();
        } else {
          Serial.println("Acceso denegado...");
          digitalWrite(ledRojo, HIGH);  // Encender el LED rojo
          delay(2000);
          digitalWrite(ledRojo, LOW);   // Apagar el LED rojo
          digitalWrite(ledRojo, HIGH);  // Encender el LED rojo
          delay(2000);
          digitalWrite(ledRojo, LOW);  // Apagar el LED rojo

          tone(bocina, 400, 300);

          digitalWrite(ledRojo, LOW);    // Encender el LED rojo
          digitalWrite(ledVerde, HIGH);  // Encender el LED rojo
          delay(500);
          tone(bocina, 400, 300);
          delay(500);
          estado = false;
        }
      }
    }
  } else {
    if (digitalRead(sensor) == HIGH && estado == true) {
      Serial.println("MOVIMIENTO DETECTADO");
      Serial.println("INGRESE EL CODIGO PARA DESACTIVAR LA ALARMA");

      // Enviar mensaje a través de WebSocket
      webSocket.broadcastTXT("parpadearCamara");
      delay(1000); // Evitar múltiples mensajes consecutivos
      // El DELAY me daba error en el de PEMCOSYS, porque bloqueaba las solicitudes HTTP
      // Entonces checale

      while (estado == true) {
        tone(bocina, 2500);
        digitalWrite(ledVerde, LOW);
        digitalWrite(ledRojo, HIGH);
        delay(1000);
        digitalWrite(ledVerde, HIGH);
        digitalWrite(ledRojo, LOW);
        Serial.println("Ingrese el código o use el RFID para desactivar la alarma:");
        if (Serial.available()) {
          int codigoIngresado = Serial.parseInt();
          if (codigoIngresado == contra) {
            estado = false;
            detenerAlarma();
            Serial.println("ALARMA DESACTIVADA");
            digitalWrite(ledRojo, LOW);    // Apagar el LED rojo
            digitalWrite(ledVerde, HIGH);  // Encender el LED rojo
            for (angle = 180; angle >= 0; angle--) {
              myservo.write(angle);
              delay(15);
            }
          }
        } else if (rfid.PICC_IsNewCardPresent()) {
          Serial.println("Tarjeta detectada, intentando leer...");
          // Seleccionar una tarjeta
          if (rfid.PICC_ReadCardSerial()) {
            Serial.println("UID de la tarjeta:");
            for (byte i = 0; i < rfid.uid.size; i++) {
              urlCaracteres += rfid.uid.uidByte[i];
            }
            Serial.println(urlCaracteres);
            String urlTemporal = urlCaracteres;
            String data = "tarjeta=" + urlTemporal;
            Serial.println(data);
            urlCaracteres = "";
            Serial.println();
            //Comprobamos si tenemos datos en el puerto serie
            if (pregunta == true) Serial.print("Escribe el identificador de la persona: ");
            pregunta = false;
            //leemos el identificador
            urlcompare = urlTemporal.toInt();
            int identificador = urlcompare;
            Serial.println(identificador);
            //llamamos a la función quenos permitira comunicarnos con el servidor
            httpRequest(identificador);
            pregunta = true;
            //------------------------------------------------------------------------------------
            // Comparar los UID para determinar si es uno de los usuarios registrados
            if (urlTemporal == nombre) {
              Serial.println("Usuario Permitido");
              Serial.println("Alarma desactivada por RFID");
              detenerAlarma();
              digitalWrite(ledVerde, HIGH);  // Encender el LED verde
              delay(4000);
              digitalWrite(ledRojo, LOW);  // Apagar el LED verde
              tone(bocina, 500, 600);
              digitalWrite(ledRojo, LOW);    // Apagar el LED rojo
              digitalWrite(ledVerde, LOW);   // Encender el LED rojo
              digitalWrite(ledRojo, LOW);    // Apagar el LED rojo
              digitalWrite(ledVerde, HIGH);  // Encender el LED rojo
              delay(500);
              estado = false;
              for (angle = 180; angle >= 0; angle--) {
                myservo.write(angle);
                delay(50);
              }

              if (!client.connect(host, httpPort)) {
                Serial.println("conexion fallida");
              }
              // crear direecion POST
              HTTPClient http;
              String url = "http://192.168.50.130/ControlAcceso/registro_date.php";
              http.begin(url);
              http.addHeader("Content-Type", "application/x-www-form-urlencoded");
              String postData = "tarjeta=" + nombre;


              int httpCode = http.POST(postData);
              String payload = http.getString();
              Serial.print("URL : ");
              Serial.println(url);
              Serial.print("Data: ");
              Serial.println(postData);
              Serial.print("httpCode: ");
              Serial.println(httpCode);
              Serial.print("payload: ");
              Serial.println(payload);
              Serial.println("---------------------------------------------------------");
              Serial.println("Pidiendo al servidor URL");

              delay(10);
              Serial.println("El servidor respondio");
              while (client.available()) {
                String line = client.readStringUntil('\r');
                Serial.println(line);
              }
              Serial.println();
            } else {
              Serial.println("Acceso denegado...");
              digitalWrite(ledRojo, HIGH);  // Encender el LED rojo
              delay(2000);
              digitalWrite(ledRojo, LOW);   // Apagar el LED rojo
              digitalWrite(ledRojo, HIGH);  // Encender el LED rojo
              delay(2000);
              digitalWrite(ledRojo, LOW);  // Apagar el LED rojo
              tone(bocina, 400, 300);
              digitalWrite(ledRojo, HIGH);  // Apagar el LED rojo
              digitalWrite(ledVerde, LOW);  // Encender el LED rojo
              delay(500);
              tone(bocina, 400, 300);
              delay(500);
              estado = true;
            }
          }
        }
      }
    }


    //---------------------------------------------------------------------------------
    Serial.println("Ingrese el código o use el RFID para desactivar la alarma:");
    if (Serial.available()) {
      int codigoIngresado = Serial.parseInt();
      if (codigoIngresado == contra) {
        estado = false;
        detenerAlarma();
        Serial.println("ALARMA DESACTIVADA");
        digitalWrite(ledRojo, LOW);    // Apagar el LED rojo
        digitalWrite(ledVerde, HIGH);  // Encender el LED rojo
        for (angle = 180; angle >= 0; angle--) {
          myservo.write(angle);
          delay(15);
        }
      }
    } else if (rfid.PICC_IsNewCardPresent()) {
      Serial.println("Tarjeta detectada, intentando leer...");
      // Seleccionar una tarjeta
      if (rfid.PICC_ReadCardSerial()) {
        Serial.println("UID de la tarjeta:");
        for (byte i = 0; i < rfid.uid.size; i++) {
          urlCaracteres += rfid.uid.uidByte[i];
        }
        Serial.println(urlCaracteres);
        String urlTemporal = urlCaracteres;
        String data = "tarjeta=" + urlTemporal;
        Serial.println(data);
        urlCaracteres = "";
        Serial.println();
        //Comprobamos si tenemos datos en el puerto serie
        if (pregunta == true) Serial.print("Escribe el identificador de la persona: ");
        pregunta = false;
        //leemos el identificador
        urlcompare = urlTemporal.toInt();
        int identificador = urlcompare;
        Serial.println(identificador);
        //llamamos a la función quenos permitira comunicarnos con el servidor
        httpRequest(identificador);
        pregunta = true;
        //------------------------------------------------------------------------------------
        // Comparar los UID para determinar si es uno de los usuarios registrados
        if (urlTemporal == nombre) {
          Serial.println("Usuario Permitido");
          Serial.println("Alarma desactivada por RFID");
          detenerAlarma();
          delay(2000);
          digitalWrite(ledVerde, HIGH);  // Encender el LED verde
          delay(4000);
          digitalWrite(ledVerde, LOW);  // Apagar el LED verde
          tone(bocina, 500, 600);
          digitalWrite(ledRojo, LOW);    // Apagar el LED rojo
          digitalWrite(ledVerde, HIGH);  // Encender el LED rojo
          delay(500);
          estado = false;
          for (angle = 180; angle >= 0; angle--) {
            myservo.write(angle);
            delay(50);
          }

          if (!client.connect(host, httpPort)) {
            Serial.println("conexion fallida");
          }
          // crear direecion POST
          HTTPClient http;
          String url = "http://192.168.50.130/ControlAcceso/registro_date.php";
          http.begin(url);
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");
          String postData = "tarjeta=" + nombre;


          int httpCode = http.POST(postData);
          String payload = http.getString();
          Serial.print("URL : ");
          Serial.println(url);
          Serial.print("Data: ");
          Serial.println(postData);
          Serial.print("httpCode: ");
          Serial.println(httpCode);
          Serial.print("payload: ");
          Serial.println(payload);
          Serial.println("---------------------------------------------------------");
          Serial.println("Pidiendo al servidor URL");

          delay(10);
          Serial.println("El servidor respondio");
          while (client.available()) {
            String line = client.readStringUntil('\r');
            Serial.println(line);
          }
          Serial.println();
        } else {
          Serial.println("Acceso denegado...");
          digitalWrite(ledRojo, HIGH);  // Encender el LED rojo
          delay(2000);
          digitalWrite(ledRojo, LOW);   // Apagar el LED rojo
          digitalWrite(ledRojo, HIGH);  // Encender el LED rojo
          delay(2000);
          digitalWrite(ledRojo, LOW);  // Apagar el LED rojo
          tone(bocina, 400, 300);
          digitalWrite(ledRojo, HIGH);  // Apagar el LED rojo
          digitalWrite(ledVerde, LOW);  // Encender el LED rojo
          delay(500);
          tone(bocina, 400, 300);
          delay(500);
          estado = true;
        }
      }
    }
  }
}


void detenerAlarma() {
  noTone(bocina);
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledRojo, LOW);
}

int httpRequest(int identificador) {
  // Comprobar si hay conexión
  if (client.connect(server, 80)) {
    Serial.println("nConectado");
    // Enviar la petición HTTP
    //Dirección del archivo php dentro del servidor
    client.print("GET http://192.168.50.130/ControlAcceso/pedir_datos.php?id_rfid=");
    //Mandamos la variable junto a la línea de GET
    client.print(identificador);
    client.println(" HTTP/1.0");
    //IP del servidor
    client.println("Host: 192.168.50.130");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();
  } else {
    // Si no conseguimos conectarnos
    Serial.println("Conexión fallida");
    Serial.println("Desconectando");
    client.stop();
  }
  delay(500);
  //Comprobamos si tenemos respuesta del servidor y la
  //almacenamos en el string ----> codigo.
  while (client.available()) {
    char c = client.read();
    codigo += c;
    //Habilitamos la comprobación del código recibido
    fin = true;
  }
  //Si está habilitada la comprobación del código entramos en el IF
  if (fin) {
    // Serial.println(codigo);
    //Analizamos la longitud del código recibido
    int longitud = codigo.length();
    //Buscamos en que posición del string se encuentra nuestra variable
    int posicion = codigo.indexOf("valor=");
    //Borramos lo que haya almacenado en el string nombre
    nombre = "";
    //Analizamos el código obtenido y almacenamos el nombre en el string nombre
    for (int i = posicion + 6; i < longitud; i++) {
      if (codigo[i] == ';') i = longitud;
      else nombre += codigo[i];
    }
    //Deshabilitamos el análisis del código
    fin = false;
    //Imprimir el nombre obtenido
    Serial.println("Valor de la variable nombre: " + nombre);
    //Cerrar conexión
    Serial.println("Desconectar");
    client.stop();
  }
  //Borrar código y salir de la función//Dirección IP del servidor
  codigo = "";
  return 1;
}

// Función para manejar eventos WebSocket
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  if (type == WStype_TEXT) {
    Serial.printf("Mensaje recibido: %s\n", payload);
  }
}