#include <OneWire.h>
#include <DallasTemperature.h>

#include <WiFi.h>
#include <HTTPClient.h>

int inGas = A0;
int inTemp = 53;

float valorGas = 0;
float valorTemp = 0;

float porcentagem;

int buzzerSaida = 47;

const char* rede = "nome-da-rede";
const char* senha = "";

String telefone = "+5561900000000;
String chaveApi = "5363213";
String mensagem = "Perigo! O dispositivo encontrou valores acima do limite!\nConcentração de Gás: "+
      String(valorGas)+
      " ADC\nTemperatura: "+
      String(valorTemp)+
      " ºC.";

String url;

OneWire oneWire(inTemp);
DallasTemperature sensores(&oneWire);

void setup() {
    Serial.begin(9600);
    pinMode(inGas,INPUT);
    pinMode(inTemp,INPUT);
    pinMode(buzzerSaida,OUTPUT);
  
    sensores.begin();
}

void loop() {
    sensores.requestTemperatures();
  
    valorGas = analogRead(inGas);

    porcentagem = (valorGas/1023.0)*100.0;
    valorTemp = sensores.getTempCByIndex(0);
  
    Serial.print("Concentração de Gás: ");
    Serial.println(valorGas);
    Serial.print("Porcentagem de concentração: ");
    Serial.print(porcentagem);
    Serial.println(" %");

    Serial.println();
  
    Serial.print("Temperatura do local:");
    Serial.print(valorTemp);
    Serial.println(" Cº");

    Serial.println("-----------------------------");

    diagnosticarGas();
    diagnosticarTemperatura();
    emitirAlerta();

    delay(5000);

    Serial.println("-----------------------------");
}

void emitirAlerta(){
  if (porcentagem > 50.0 || valorTemp >= 50.0){
    digitalWrite(buzzerSaida,HIGH);

    conectarRede();

    enviarNotificacao();
  }else{
    digitalWrite(buzzerSaida,LOW);
  }
}

void diagnosticarTemperatura(){
  if(valorTemp >= 50.0){
    Serial.println("Temperatura muito alta!");
    
    Serial.println();
  }else{
    Serial.println("Temperatura dentro do normal.");
    
    Serial.println();
  }
}

void diagnosticarGas(){
  if(porcentagem > 50.0){
      Serial.println("Alerta! Concentração de gás acima do normal!");

      Serial.println();
    }else{
      Serial.println("Concentração de gás dentro do normal.");

      Serial.println();
    }
}

void conectarRede(){
  WiFi.begin(rede,senha);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("Conectando...");
  }

  Serial.println("-----------");
  Serial.println("Conectado ao Wi-Fi!!");
}

void enviarNotificacao(){
   HTTPClient http;

  url = "https://api.callmebot.com/whatsapp.php?phone=" +telefone+
   "&text= "+mensagem+ "&apikey=" +chaveApi;

   url.replace(" ", "%20");
   url.replace("\n", "%0A");
   url.replace("º", "%C2%B0");
   
  //Serial.println(url);
  http.begin(url);

  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
      Serial.print("Código HTTP: ");
      Serial.println(httpResponseCode);

    String resposta = http.getString();
    Serial.println(resposta);
  } else {
      Serial.print("Erro: ");
      Serial.println(http.errorToString(httpResponseCode));
  }

  http.end();
}
