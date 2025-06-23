/************************************************************************************
 *  Canal: https://www.youtube.com/c/TeachMeSomething
 ************************************************************************************/

#define BLYNK_TEMPLATE_ID "TMPL2EzB7CLk7"
#define BLYNK_TEMPLATE_NAME "Notification"
#define BLYNK_AUTH_TOKEN "czm1jagA1Sb4EWcKWXHj2dSfRNwnQdkI"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// ==== Configura tu red WiFi ====
char ssid[] = "Hacker";         // Reemplaza con tu nombre de red
char pass[] = "123456789";     // Reemplaza con tu contraseña

// ==== Pines usados ====
#define Push_Button 35
#define wifi_led 33
#define LED_GPIO 2               // LED controlado por Blynk (puedes cambiar el pin)

String Message = "Accident detect";
char auth[] = BLYNK_AUTH_TOKEN;

unsigned long old = 0;
unsigned long current = 0;
int interval = 10;
int interval2 = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("\nConectando a Blynk...");

  pinMode(wifi_led, OUTPUT);
  pinMode(Push_Button, INPUT);
  pinMode(LED_GPIO, OUTPUT);
  digitalWrite(LED_GPIO, LOW);  // LED apagado por defecto

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  wifi_testing();
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
  wifi_testing();

  current = millis();
  interval2 = (current - old) / 1000;

  if ((digitalRead(Push_Button) == LOW) && (interval2 >= 3)) {
    Serial.println("Notificación enviada desde ESP32");
    
    // Añade verificación de conexión
    if(Blynk.connected()) {
      Serial.println("Blynk conectado - Enviando notificación");
      Blynk.logEvent("notification", Message); // En minúsculas
      
      // Envía también un mensaje al widget de notificaciones
      Blynk.virtualWrite(V1, "Botón presionado: " + Message);
    } else {
      Serial.println("Error: Blynk no conectado");
    }

    while (digitalRead(Push_Button) == LOW) {
      delay(10);
    }
    old = millis();
  }
}

// ==== Función para controlar LED desde Blynk ====
BLYNK_WRITE(V0) {
  int estado = param.asInt(); // 1 = encender, 0 = apagar
  digitalWrite(LED_GPIO, estado);
}

// ==== Verifica conexión WiFi y parpadea LED si no hay conexión ====
void wifi_testing() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
    digitalWrite(wifi_led, LOW);
    delay(250);
    digitalWrite(wifi_led, HIGH);
    current = millis();

    if ((current - old) / 1000 > interval) {
      ESP.restart();
    }
  }
}
