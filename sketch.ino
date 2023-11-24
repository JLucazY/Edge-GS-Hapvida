//Autores: João Lucas Yudi e Gabriel Machado
//Baseado no código do professor Fábio Henrique Cabrini
//Resumo: Este programa monitora a frequência cardíaca, níveis de oxigênio.

//Bibliotecas necessárias 
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Configurações - variáveis editáveis
const char* default_SSID = "Wokwi-GUEST"; // Nome da rede Wi-Fi
const char* default_PASSWORD = ""; // Senha da rede Wi-Fi
const char* default_BROKER_MQTT = "46.17.108.113"; // IP do Broker MQTT
const int default_BROKER_PORT = 1883; // Porta do Broker MQTT
const char* default_TOPICO_SUBSCRIBE = "/TEF/lamp001/cmd"; // Tópico MQTT de escuta
const char* default_TOPICO_PUBLISH_1 = "/TEF/lamp001/attrs"; // Tópico MQTT de envio de informações para Broker
const char* default_TOPICO_PUBLISH_2 = "/TEF/lamp001/attrs/batimentos"; // Tópico MQTT de envio de informações para Broker
const char* default_TOPICO_PUBLISH_3 = "/TEF/lamp001/attrs/oxigenio";
const char* default_ID_MQTT = "fiware_001"; // ID MQTT
const int default_D4 = 2; // Pino do LED onboard
// Declaração da variável para o prefixo do tópico
const char* topicPrefix = "Heat";


#define PULSE_PER_BEAT    1
#define INTERRUPT_PIN     5
#define OXYGEN_SENSOR_PIN A0
#define SAMPLING_INTERVAL 1000



// Variáveis para configurações editáveis
char* SSID = const_cast<char*>(default_SSID);
char* PASSWORD = const_cast<char*>(default_PASSWORD);
char* BROKER_MQTT = const_cast<char*>(default_BROKER_MQTT);
int BROKER_PORT = default_BROKER_PORT;
char* TOPICO_SUBSCRIBE = const_cast<char*>(default_TOPICO_SUBSCRIBE);
char* TOPICO_PUBLISH_1 = const_cast<char*>(default_TOPICO_PUBLISH_1);
char* TOPICO_PUBLISH_2 = const_cast<char*>(default_TOPICO_PUBLISH_2);
char* TOPICO_PUBLISH_3 = const_cast<char*>(default_TOPICO_PUBLISH_3);
char* ID_MQTT = const_cast<char*>(default_ID_MQTT);
int D4 = default_D4;

WiFiClient espClient;
PubSubClient MQTT(espClient);
char EstadoSaida = '0';


volatile uint16_t pulse;
uint16_t count;
uint16_t heartRate;  // Alterado para uint16_t para remover decimais
uint16_t oxygenLevel;  
LiquidCrystal_I2C lcd(0x27, 20, 4);
 
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
 
void IRAM_ATTR HeartRateInterrupt() {
  portENTER_CRITICAL_ISR(&mux);
  pulse++;
  portEXIT_CRITICAL_ISR(&mux);
}




void initSerial() {
    Serial.begin(115200);
}

void initWiFi() {
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
    reconectWiFi();
}

void initMQTT() {
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);
    MQTT.setCallback(mqtt_callback);
}

//Setup para o código
void setup() {
    InitOutput();
    initSerial();
    initWiFi();
    initMQTT();
    delay(5000);
    MQTT.publish(TOPICO_PUBLISH_1, "s|on");

    Serial.begin(115200);
    Wire.begin();
    lcd.begin(16, 2);
    lcd.backlight();
    pinMode(INTERRUPT_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), HeartRateInterrupt, RISING);
}

//O loop chama as funções responsáveis para a lógica do programa
void loop() {
    VerificaConexoesWiFIEMQTT();
    EnviaEstadoOutputMQTT();
    medirBat();
    MQTT.loop();
    
}

void reconectWiFi() {
    if (WiFi.status() == WL_CONNECTED)
        return;
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());

    // Garantir que o LED inicie desligado
    digitalWrite(D4, LOW);
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    String msg;
    for (int i = 0; i < length; i++) {
        char c = (char)payload[i];
        msg += c;
    }
    Serial.print("- Mensagem recebida: ");
    Serial.println(msg);

    // Forma o padrão de tópico para comparação
    String onTopic = String(topicPrefix) + "@on|";
    String offTopic = String(topicPrefix) + "@off|";

    // Compara com o tópico recebido
    if (msg.equals(onTopic)) {
        digitalWrite(D4, HIGH);
        EstadoSaida = '1';
    }

    if (msg.equals(offTopic)) {
        digitalWrite(D4, LOW);
        EstadoSaida = '0';
    }
}

void VerificaConexoesWiFIEMQTT() {
    if (!MQTT.connected())
        reconnectMQTT();
    reconectWiFi();
}

void EnviaEstadoOutputMQTT() {
    if (EstadoSaida == '1') {
        MQTT.publish(TOPICO_PUBLISH_1, "s|on");
    }

    if (EstadoSaida == '0') {
        MQTT.publish(TOPICO_PUBLISH_1, "s|off");
    }
    delay(1000);
}

void InitOutput() {
    pinMode(D4, OUTPUT);
    digitalWrite(D4, HIGH);
    boolean toggle = false;

    for (int i = 0; i <= 10; i++) {
        toggle = !toggle;
        digitalWrite(D4, toggle);
        delay(200);
    }
}

void reconnectMQTT() {
    while (!MQTT.connected()) {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE);
        } else {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Haverá nova tentativa de conexão em 2s");
            delay(2000);
        }
    }
}

//Função responsável sobre a leitura dos batimentos cárdiacos e simulação do oxigênio
void HeartRate() {
  static unsigned long startTime;
  if (millis() - startTime < SAMPLING_INTERVAL) return;
  startTime = millis();
 
  portENTER_CRITICAL(&mux);
  count = pulse;
  pulse = 0;
  portEXIT_CRITICAL(&mux);
 
  heartRate = map(count, 0, 220, 0, 100);  
  String batimentos = String(heartRate);
  MQTT.publish(TOPICO_PUBLISH_2, batimentos.c_str());
 
 
  oxygenLevel = analogRead(OXYGEN_SENSOR_PIN);
  oxygenLevel = map(oxygenLevel, 0, 1023, 5, 25);  
  String oxigenio= String(oxygenLevel);
  MQTT.publish(TOPICO_PUBLISH_3, oxigenio.c_str());

  Serial.print("Heart Rate: ");
  Serial.print(heartRate);
  Serial.print(" BPM | Oxygen Level: ");
  Serial.println(oxygenLevel);
}
 
//Função do display, exibe as informações
void DisplayInfo() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("batimentos: ");
  lcd.print(heartRate);
  lcd.print(" BPM");
 
  lcd.setCursor(0, 2);
  lcd.print("nvl oxigenio: ");
  lcd.print(oxygenLevel);
  lcd.print(" %");
}

//Chama as funções e aplica um delay de 3 segundos para o código rodar novamente
void medirBat() {
  HeartRate();
  DisplayInfo();
  delay(3000); 

}