#include <LiquidCrystal_I2C.h>
#include <Servo.h>
//definindo os pinos que serão usados
#define servoPuxarLancador 3
#define servoTrava 5
#define servoGiro 11
#define botao 7
#define botaoRecarregar 10
#define switchLancamento 8
#define potMexer A3
//valores para o movimento do servo do elástico
int valorTensionado = 80;
int valorDestensionado = 180;
//variaveis para o print do lcd
int angForm = 0;
int valorAnt = 1024;
LiquidCrystal_I2C lcd(0x27, 16, 2);
//definição dos servos
Servo servoPuxador;
Servo servoGirador;
Servo travaCatapulta;

int contadorPosServo=45;
bool lancando=false;
bool botaoLancamentoApertado=false;
bool cont2;
String mod = "nao"; //variavel para o loop de escolher o modo

void setup() 
{
  pinMode(botao, INPUT_PULLUP);
  pinMode(botaoRecarregar, INPUT_PULLUP);
  pinMode(switchLancamento,INPUT_PULLUP);
  pinMode(servoPuxarLancador,OUTPUT);
  pinMode(servoTrava,OUTPUT);
  pinMode(servoGiro,OUTPUT);
  lcd.init();
  lcd.backlight();
  servoPuxador.write(valorDestensionado);
  servoPuxador.attach(servoPuxarLancador);
  servoGirador.write(45);
  servoGirador.attach(servoGiro);
  travaCatapulta.write(180);
  travaCatapulta.attach(servoTrava);
}

unsigned long tempoLancamento = millis();
bool updateTempoLancamento = true;
String modos[2] = {"Automatico", "Manual"};
int i = 0;
int tempoBotao = 0;
bool apertou = 0;
bool valorBotao = 0;
bool recarregando=0;
int velocidade=0;
void loop()
{
  unsigned long tempoInicialDificuldade=millis();
  lcd.print("Selecione: ");
  lcd.setCursor(0, 1);
  lcd.print(modos[0]);
  while(mod == "nao"){
    lcd.setCursor(0, 0);
    valorBotao = !digitalRead(botao);
    while(valorBotao){
      tempoBotao+=1;
      delay(1);
      apertou=true;
      valorBotao = !digitalRead(botao);
    }
    
    if(apertou){
      apertou=false;
      if(tempoBotao <= 200){
        tempoBotao = 0;
        i = i + 1;
        if(i==2){
          i = 0;
        }
        lcd.clear();
        lcd.print("Selecione: ");
        lcd.setCursor(0, 1);
        lcd.print(modos[i]);
      }
      else{
        mod = modos[i];
      }
    }
  }
  lcd.clear();
  lcd.home();
  lcd.print("Modo: ");
  if(mod == "Automatico"){
  lcd.setCursor(0, 1);
  lcd.print(mod);
  }
  else if(mod == "Manual"){
  lcd.print(mod);
  lcd.setCursor(0, 1);
  lcd.print("Angulo: ");
  }
  delay(2000);
  
  while(mod != "nao"){ //apos selecionar o modo
    bool travaSwitch = 0, valorBotaoRecarregar = 0;
    valorBotao = !digitalRead(botao);
	  valorBotaoRecarregar = !digitalRead(botaoRecarregar);
    travaSwitch = !digitalRead(switchLancamento);
    
    if(travaSwitch){
      if (valorBotao){
        botaoLancamentoApertado=true;
      }
      if(botaoLancamentoApertado){
        if(updateTempoLancamento){
          cont2=0;
          updateTempoLancamento=false;
          tempoLancamento=millis();
          if(mod == "Automatico"){
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Modo: Auto");
          }
          lcd.setCursor(0, 1);
          lcd.print("Angulo: ");
          lcd.print(angForm);
        
        }
        valorBotaoRecarregar=!digitalRead(botaoRecarregar);
        
        if (millis()-tempoLancamento<4000){ //puxar tensão
            servoPuxador.write(valorTensionado);
        }
        else if(millis()-tempoLancamento<6000){ //destravar trava
          lancando=true;
          travaCatapulta.write(90);
        }
        else if (millis()-tempoLancamento<8000){ //desligar estado de lançamento
          lancando=false;
        }
        else if (millis()-tempoLancamento<10000){ //ciclo destensionar
          servoPuxador.write(valorDestensionado);
          recarregando=1;
        }
        else if (recarregando){ //ciclo retravar/recarregar
          if(!cont2){
            lcd.clear();
            cont2=1;
            lcd.home();
            lcd.print("Apos recarregar");
            lcd.setCursor(0, 1);
            lcd.print("aperte o botao");
          }
          if (valorBotaoRecarregar){
            valorBotaoRecarregar = !digitalRead(botaoRecarregar);
            travaCatapulta.write(180);
            recarregando=0;
          }

        }
        else{
          updateTempoLancamento=true;
          botaoLancamentoApertado=false;
          delay(5000); //temp
          travaCatapulta.write(180); //temp
          lcd.clear();
          lcd.home();
          lcd.print("Modo: ");
          if(mod == "Automatico"){
            lcd.setCursor(0, 1);
            lcd.print(mod);
          }
          else if(mod == "Manual"){
            lcd.print(mod);
            lcd.setCursor(0, 1);
            lcd.print("Angulo: ");
          }
        }
      }
      else if (!lancando){
        arrumar();
        girar();
      }

    }
    else{
      arrumar();
      girar();
      if (valorBotao){
        lcd.clear();
        lcd.home();
        lcd.print("Trava acionada");
        delay(250);
        lcd.clear();
        lcd.home();
        lcd.print("Modo: ");
        lcd.print(mod);
      }
    }
  }
}


void girar()
{
  if (mod == "Automatico"){
  int t0 = millis()%3000;
  if(t0<1500){
    contadorPosServo+=1;
    contadorPosServo = constrain(contadorPosServo,0,90);
  	servoGirador.write(contadorPosServo);
  }
  else{
    contadorPosServo-=1;
    contadorPosServo = constrain(contadorPosServo,0,90);
  	servoGirador.write(contadorPosServo);
  }
  angForm = map(contadorPosServo, 0, 90, -45, 45);
  }
  else if (mod == "Manual"){
    int valorPot = analogRead(potMexer);
    int ang = map(valorPot, 0, 1023, 0, 90);
    servoGirador.write(ang);
    if(valorPot != valorAnt){
    valorAnt = valorPot;
    angForm = map(ang, 0, 90, -45, 45);
    lcd.setCursor(8, 1);
    lcd.print(angForm);
    if(angForm >= 0){
      lcd.print(" ");
    }
    }
    
    
  }
  
}


void arrumar()
{
  travaCatapulta.write(180);
  servoPuxador.write(valorDestensionado);
}

