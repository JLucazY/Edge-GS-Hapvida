# Edge-GS-Hapvida
Repositório solicitado pelo professor para a entrega de EDGE em global solutions

Este código foi baseado no projeto de nosso professor: Fabio Hnerique Cabrini 


# Descrição do problema de saúde abordado
O código aborda a monitorização de sinais vitais relacionados à saúde cardiovascular e respiratória. Em específico, monitora a frequência cardíaca (batimentos por minuto - BPM) e os níveis de oxigênio no sangue, indicados como percentagem de saturação de oxigênio. Esses são indicadores cruciais para avaliar a saúde do coração e a capacidade do corpo em transportar oxigênio para os tecidos.

# Visão Geral da Solução Proposta
O programa utiliza um microcontrolador ESP32 para ler os dados de um sensor de frequência cardíaca e de um sensor de oxigênio. Os dados são então enviados a um servidor MQTT (Message Queuing Telemetry Transport), permitindo a comunicação assíncrona entre dispositivos. Além disso, o código controla um LED onboard que pode ser ligado ou desligado remotamente por meio de mensagens MQTT.

O sistema é capaz de publicar informações sobre a frequência cardíaca e os níveis de oxigênio em tópicos específicos do servidor MQTT. Esses dados podem ser posteriormente consumidos por outros dispositivos ou sistemas, como o "Helix", para monitorar o estado de saúde do usuário em tempo real.

# Instruções para Configurar e Executar o Programa

# 1. Configurações iniciais 
Defina as configurações de Wi-Fi (SSID e senha) para que o ESP32 possa se conectar à rede.

Configure o endereço IP e a porta do Broker MQTT. O IP do Broker MQTT é fornecido como "46.17.108.113", e a porta é "1883".

Especifique os tópicos MQTT para subscrição (TOPICO_SUBSCRIBE) e publicação (TOPICO_PUBLISH_1, TOPICO_PUBLISH_2, TOPICO_PUBLISH_3).
# 2. Configuração do Hardware
Conecte o ESP32 apropriadamente aos sensores de frequência cardíaca e oxímetro de pulso.

# 3. Compilação e Carregamento do Código
Utilize uma IDE Arduino para compilar e carregar o código para o ESP32.

Certifique-se de ter as bibliotecas necessárias instaladas (WiFi, PubSubClient, Wire, LiquidCrystal_I2C).

# 4. Execução
Inicialize o sistema e aguarde a conexão com a rede Wi-Fi e o Broker MQTT.

O programa começará a ler os dados dos sensores e publicar as informações nos tópicos MQTT especificados.

O LED onboard pode ser controlado remotamente enviando mensagens MQTT para o tópico de subscrição.

# Link do projeto no Wokwi
https://wokwi.com/projects/382197947765446657
