#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>

#include <SPI.h>
#include <Ethernet.h>

#include <Adafruit_Sensor.h>

#define DHTPIN A2 // what pim we'ew connected to
#define DHTTYPE DHT11

DHT dht(DHTPIN,DHTTYPE);


 
// Entre com os dados do MAC e ip para o dispositivo.
// Lembre-se que o ip depende de sua rede local
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10,0,0,135);
 
// Inicializando a biblioteca Ehternet
// 80 é a porta que será usada. (padrão http)
EthernetServer server(80);
 
void setup() {
 // Abrindo a comunicação serial para monitoramento.
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
    dht.begin();
  }
 
  // Inicia a conexão Ethernet e o servidor:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("Servidor iniciado em: ");
  Serial.println(Ethernet.localIP());
}
 
void loop() {

  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Aguardando novos clientes;
  EthernetClient client = server.available();
  if (client) {
    Serial.println("Novo Cliente");
    // Uma solicitação http termina com uma linha em branco
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // Se tiver chegado ao fim da linha (recebeu um novo 
        // Caractere) e a linha estiver em branco, o pedido http terminou,
        // Para que você possa enviar uma resposta
        if (c == '\n' && currentLineIsBlank) {
          // Envia um cabeçalho de resposta HTTP padrão
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // a conexão será fechada após a conclusão da resposta
      client.println("Refresh: 5");  // atualizar a página automaticamente a cada 5 segundos
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          
          // saídas dos valores do sensor DHT
          
            client.println("<H2>");
            client.print("Umidade do ar:");
            client.println("</H2>");
            client.println("<p/>");
            client.println("<H1>");
            client.print(h);
            client.print("%\t");
            client.println("</H1>");
            client.println("<p />"); 
            client.println("<H2>");
            client.print("Temperatura: ");
            client.println("</H2>");
            client.println("<p/>");
            client.println("<H1>");
            client.print(t);
            client.print("ºC\t");
            client.println("</H1>");
            
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // Você está começando uma nova linha
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // Você recebeu um caracter na linha atual.
          currentLineIsBlank = false;
        }
      }
    }
    // Dar tempo ao navegador para receber os dados
    delay(1);
    // Fecha a conexão:
    client.stop();
    Serial.println("Cliente desconectado");
  }
}
