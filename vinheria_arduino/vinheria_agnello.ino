/*
  Projeto: Controle de Iluminação – Vinheria Agnello
  Descrição: 
    Sistema que lê a intensidade luminosa via sensor LDR, 
    exibe a porcentagem de luz no display LCD e aciona LEDs 
    e buzzer conforme os níveis de luminosidade detectados.
    
  Funcionalidades:
    - Calibração automática do sensor (mínimo e máximo de luz)
    - Exibição da luz ambiente em porcentagem
    - Indicação visual (LEDs verde, amarelo e vermelho)
    - Alerta sonoro quando há excesso de luz
    - Uso de caracteres customizados no display

  Autores: [Pedro Tavares, David Gama, Pedro Sales]
  Data: 05/10/2025
  Versão: 1.0
  Hardware: Arduino UNO R3 + Sensor LDR + Display LCD 16x2 + LEDs + Buzzer
*/

#include <LiquidCrystal.h> // Biblioteca para controle do display LCD

// Define o objeto LCD: (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 10, 5, 4, 3);

const int pinoLDR = A0;        // Pino analógico do LDR
const int ledVerde = 8;        // LED verde = luz fraca
const int ledAmarelo = 9;      // LED amarelo = luz média
const int ledVermelho = 6;     // LED vermelho = luz forte
const int buzzer = 13;         // Buzzer no pino digital 13

int valorLido;                 // Valor atual do LDR
int minLuz = 1023;             // Valor mínimo (será calibrado)
int maxLuz = 0;                // Valor máximo (será calibrado)

// Caracteres customizados
byte bloco[8] = {B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11111};
byte taca[8]  = {B00100,B01110,B01110,B11111,B11111,B01110,B01110,B00000};

void setup() {
  lcd.begin(16,2);                // Inicializa LCD 16x2
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  pinMode(buzzer, OUTPUT);

  lcd.createChar(0, bloco);       // Caractere bloco (barra de progresso)
  lcd.createChar(1, taca);        // Caractere taça (decoração)

  boasVindas();                   // Mensagem inicial
  calibrarAutomatico();           // Calibração do sensor LDR
}

// Exibe mensagem de boas-vindas
void boasVindas() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Vinheria Agnello");
  lcd.setCursor(0,1);
  lcd.write(byte(1));             // Ícone da taça
  lcd.print(" Bem-vindo!");
  delay(2000);
  lcd.clear();
}

// Calibra automaticamente o sensor de luz
void calibrarAutomatico() {
  // Etapa 1: valor mínimo (sensor coberto)
  lcd.clear();
  lcd.print("Cubra o sensor");
  lcd.setCursor(0,1);
  unsigned long inicio = millis();
  while(millis() - inicio < 3000){
    valorLido = analogRead(pinoLDR);
    if(valorLido < minLuz) minLuz = valorLido;

    int progresso = map(millis()-inicio, 0, 3000, 0, 16);
    for(int i=0;i<16;i++){
      lcd.setCursor(i,1);
      if(i<progresso) lcd.write(byte(0)); else lcd.print(" ");
    }
    delay(50);
  }

  // Etapa 2: valor máximo (sensor iluminado)
  lcd.clear();
  lcd.print("Ilumine sensor");
  lcd.setCursor(0,1);
  inicio = millis();
  while(millis() - inicio < 3000){
    valorLido = analogRead(pinoLDR);
    if(valorLido > maxLuz) maxLuz = valorLido;

    int progresso = map(millis()-inicio, 0, 3000, 0, 16);
    for(int i=0;i<16;i++){
      lcd.setCursor(i,1);
      if(i<progresso) lcd.write(byte(0)); else lcd.print(" ");
    }
    delay(50);
  }

  // Ajusta limites
  minLuz = max(minLuz-15,0);
  maxLuz = min(maxLuz+15,1023);

  lcd.clear();
  lcd.print("Calibrado! :)");
  delay(1500);
  lcd.clear();
}

void loop() {
  // Média de 5 leituras do LDR
  long soma = 0;
  for(int i=0; i<5; i++){
    soma += analogRead(pinoLDR);
    delay(20);
  }
  valorLido = soma / 5;

  // Converte leitura em porcentagem (0–100%)
  int porcentagemLuz = map(valorLido,minLuz,maxLuz,0,100);
  porcentagemLuz = constrain(porcentagemLuz,0,100);

  // Exibe resultado no LCD
  lcd.setCursor(0,0);
  lcd.print("Luz: ");
  lcd.print(porcentagemLuz);
  lcd.print("%   ");
  lcd.setCursor(0,1);
  lcd.write(byte(1));
  lcd.print(" V.Agnello");
  delay(1500);

  // Reseta LEDs
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledAmarelo, LOW);
  digitalWrite(ledVermelho, LOW);

  // Lógica de alerta visual/sonoro
  if(porcentagemLuz <= 40){
    digitalWrite(ledVerde, HIGH);
    noTone(buzzer);
  } 
  else if(porcentagemLuz <= 70){
    digitalWrite(ledAmarelo, HIGH);
    noTone(buzzer);
  } 
  else {
    digitalWrite(ledVermelho, HIGH);
    lcd.clear();
    lcd.print("ALERTA! Luz");
    lcd.setCursor(0,1);
    lcd.print("Excessiva!");

    unsigned long inicioAlerta = millis();
    while(millis() - inicioAlerta < 3000){
      tone(buzzer, 1000);  // Som de 1000Hz por 3 segundos
    }
    noTone(buzzer);
    return;
  }
}