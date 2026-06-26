#include <LiquidCrystal_I2C.h>
#include <Servo.h>
//#include <pitches.h>
#define servoPuxarLancador 3
#define servoTrava 5
/*
#define motorGiroLigar 13
#define motorGiroPos 11
#define motorGiroNeg 10
*/
#define servoGiro 11
#define motorAlvoLigar 13
#define motorAlvoPos 6
#define motorAlvoNeg 9
#define botao 7
#define botaoAlvo 10
#define switchLancamento 8
#define buzzer 2
/*#define RS 23
#define En 22
#define db4 24
#define db5 25
#define db6 26
#define db7 27
*/
#define PotMexer A5
int valorTensionado = 80;
int valorDestensionado = 180;

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servoPuxador;
Servo servoGirador;
Servo travaCatapulta;
int contadorPosServo=45;
bool ladoGiro=true;
bool ladoAlvo=true;
bool lancando=false;
bool botaoLancamentoApertado=false;

int pontos=0;
String dificuldade = "nao";

void setup() 
{
  Serial.begin(9600);
  //lcd_1.begin(16, 2);
  pinMode(botao, INPUT_PULLUP);
  pinMode(botaoAlvo, INPUT_PULLUP);
  pinMode(switchLancamento,INPUT_PULLUP);
  pinMode(servoPuxarLancador,OUTPUT);
  pinMode(servoTrava,OUTPUT);
  pinMode(servoGiro,OUTPUT);
  pinMode(motorAlvoLigar,OUTPUT);
  pinMode(motorAlvoPos,OUTPUT);
  pinMode(motorAlvoNeg,OUTPUT);
  pinMode(buzzer, OUTPUT);
  //pinMode(RS,OUTPUT);
  pinMode(En,OUTPUT);
  //pinMode(db4,OUTPUT);
  //pinMode(db5,OUTPUT);
  //pinMode(db6,OUTPUT);
  //pinMode(db7,OUTPUT);
  digitalWrite(motorAlvoLigar, HIGH);
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
String dif[2] = {"Auto", "Manual"};
int i = 0;
int tempoBotao = 0;
bool apertou = 0;
bool valorBotao = 0;
bool recarregando=0;
int velocidade=0;
void loop()
{
  Serial.print("botao ");
  Serial.println(valorBotao);
  unsigned long tempoInicialDificuldade=millis();
  Serial.println("selecione");
  lcd.print("Selecione: ");
  lcd.setCursor(0, 1);
  lcd.print("Facil");
  while(dificuldade == "nao"){
    lcd.setCursor(0, 0);
    valorBotao = !digitalRead(botao);
    while(valorBotao){
      Serial.println(tempoBotao);
      tempoBotao+=1;
      delay(1);
      apertou=true;
      valorBotao = !digitalRead(botao);
    }
    /*if(!valorBotao){
      if (millis()-tempoInicialDificuldade<2000){
        lcd.clear();
        lcd.print("Selecione: ");
        lcd.setCursor(0, 1);
      	lcd.print(dif[0]);
      }
      else if (millis()-tempoInicialDificuldade<4000){
        lcd.clear();
        lcd.print("Selecione: ");
        lcd.setCursor(0, 1);
      	lcd.print(dif[1]);
      }
      else if (millis()-tempoInicialDificuldade<6000){
        lcd.clear();
        lcd.print("Selecione: ");
        lcd.setCursor(0, 1);
      	lcd.print(dif[2]);
      }
      else{
      	tempoInicialDificuldade=millis();
      }
      apertou = 0;
    }
    while(valorBotao){
      tempoBotao = millis() + 1000;
      apertou = 1;
    }
    */
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
        lcd.print(dif[i]);
      }
      else{
        dificuldade = dif[i];
      }
    }
  }
  //Serial.println("saiu");
  lcd.clear();
  lcd.home();
  Serial.print("dificuldade");
  lcd.print("Dificuldade: ");
  lcd.setCursor(0, 1);
  lcd.print(dificuldade);
  delay(2000);
  lcd.clear();
  lcd.home();
  lcd.print("Pontuacao: 0");
  lcd.setCursor(0, 1);
  lcd.print("Dificuldade: ");
  lcd.print(dificuldade[0]);
  
  while(dificuldade != "nao"){ 
    //Serial.print("botao2");
    //Serial.println(valorBotao);
    if(dificuldade==dif[0]){
      velocidade=128;
    }
    else if(dificuldade==dif[1]){
      velocidade=200;
    }
    else{
      velocidade=255;
    }
    //Serial.print("loop");
    //valorBotao = 0;
    bool travaSwitch = 0, valorBotaoAlvo = 0;
    valorBotao = !digitalRead(botao);
	  valorBotaoAlvo = !digitalRead(botaoAlvo);
    travaSwitch = !digitalRead(switchLancamento);
    //Serial.println(valorBotao);
    //Serial.print("botao");
    //Serial.println(valorBotao);
    //Serial.print("trava");
    //Serial.println(travaSwitch);

    
    if (valorBotaoAlvo){
      tone(buzzer, 800, 300);
      pontos+=1;
      lcd.clear();
      lcd.home();
      lcd.print("Pontuacao: ");
      lcd.print(pontos);
      lcd.setCursor(0, 1);
      lcd.print("Dificuldade: ");
      lcd.print(dificuldade[0]);
    }
    
    if(travaSwitch){
      if (valorBotao){
        botaoLancamentoApertado=true;
      }
      if(botaoLancamentoApertado){
        if(updateTempoLancamento){
          //Serial.println("aaaaaaaaaaaaaaaaaaaaaaa");
          updateTempoLancamento=false;
          tempoLancamento=millis();
        }
        //Serial.print("pontos ");
        //Serial.println(pontos);
        valorBotaoAlvo=!digitalRead(botaoAlvo);
        bool cont2=0;
        //Serial.println(tempoLancamento);
        //Serial.println(millis()-tempoLancamento);
        if (millis()-tempoLancamento<4000){ //puxar tensão
            servoPuxador.write(valorTensionado);
        }
        else if(millis()-tempoLancamento<6000){ //destravar trava
          Serial.print("a ");
          Serial.println(tempoLancamento);
          analogWrite(motorAlvoPos, 0);
          lancando=true;
          lancar();
          travaCatapulta.write(90);
        }
        else if (millis()-tempoLancamento<8000){ //desligar estado de lançamento
          Serial.print("b ");
          Serial.println(tempoLancamento);
          lancando=false;
        }
        else if (millis()-tempoLancamento<10000){ //ciclo destensionar
          Serial.print("c");
          Serial.println(tempoLancamento);
          servoPuxador.write(valorDestensionado);
          recarregando=1;
        }
        else if (recarregando){ //ciclo retravar // trocar pra um if(recarregando)
          Serial.print("d");
          Serial.println(tempoLancamento);
          if(!cont2){
            lcd.clear();
            cont2=1;
            lcd.home();
            lcd.print("apos recarregar");
            lcd.setCursor(0, 1);
            lcd.print("aperte o botao");
          }
          if (valorBotaoAlvo){
            valorBotaoAlvo = !digitalRead(botaoAlvo);
            travaCatapulta.write(180);
            recarregando=0;
          }
          //travaCatapulta.write(180); por enquanto removido, botar recarregar aqui
        }
        else{
          Serial.print("e");
          Serial.println(tempoLancamento);
          updateTempoLancamento=true;
          botaoLancamentoApertado=false;
          analogWrite(motorAlvoPos, 255);
          delay(5000); //temp
          travaCatapulta.write(180); //temp
          //delay(20);
        }
      }
      else if (!lancando){
        //Serial.print("a");
        moverAlvo(velocidade);
        arrumar();
        girar();
      }

    }
    else{
      //Serial.print("noy");
      moverAlvo(velocidade);
      arrumar();
      girar();
      if (valorBotao){
        lcd.clear();
        lcd.home();
        Serial.print("trava");
        lcd.print("Trava acionada");
        //delay(250);
        lcd.clear();
      	lcd.home();
      	lcd.print("Pontuacao: ");
      	lcd.print(pontos);
      	lcd.setCursor(0, 1);
      	lcd.print("Dificuldade: ");
      	lcd.print(dificuldade[0]);
      }
    }
  }
}

void moverAlvo(int velocidadeMotor)
{ //o uso da biblioteca do servo inibe pwm nas portas 9 e 10 --> motivo do problema
  //Serial.println(velocidadeMotor);
  analogWrite(motorAlvoPos, velocidadeMotor);
  analogWrite(motorAlvoNeg, 0);
  
}

void girar()
{
  //ativa motorGiro
  //digitalWrite(motorAlvoLigar, HIGH);
  if (dificuldade == "Auto"){
  int t0 = millis()%3000;
  Serial.println(t0);
  //Serial.println(contadorPosServo);
  if(t0<1500){
    contadorPosServo+=1;
    contadorPosServo = constrain(contadorPosServo,0,90);
    //Serial.print(contadorPosServo);
  	servoGirador.write(contadorPosServo);
    //if(contadorPosServo==180){
    //  ladoGiro=false;    
    //}
  }
  else{
    contadorPosServo-=1;
    contadorPosServo = constrain(contadorPosServo,0,90);
    //Serial.println(contadorPosServo);
  	servoGirador.write(contadorPosServo);
    //if(contadorPosServo==0){
    //  ladoGiro=true;    
    //}
  }
  }
  else if (dificuldade == "Manual"){
    int pot = analogRead(A3);
    int x = map(pot, 0, 1023, 0, 90);
    servoGirador.write(x);
  }
}

void lancar()
{
  analogWrite(motorAlvoPos, 0); // deixar parando o movimento do alvo durante o lançamento?
  analogWrite(motorAlvoNeg, 0);
  //servoPuxador.write();
}

void arrumar()
{
  //updateTempoLancamento=true;
  travaCatapulta.write(180);
  servoPuxador.write(valorDestensionado);
}

