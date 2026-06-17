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

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servoPuxador;
Servo servoGirador;
Servo travaCatapulta;
int contadorPosServo=0;
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
  pinMode(switchLancamento,INPUT);
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
}

int tempoLancamento = millis();
bool updateTempoLancamento = true;
String dif[3] = {"Facil", "Medio", "Dificil"};
int i = 0;
int tempoBotao = 0;
bool apertou = 0;
bool valorBotao = 0;
int velocidade=0;
void loop()
{
  unsigned long tempoInicialDificuldade=millis();
  lcd.print("Selecione: ");
  lcd.setCursor(0, 1);
  lcd.print("Facil");
  while(dificuldade == "nao"){
    lcd.setCursor(0, 0);
    valorBotao = !digitalRead(botao);
    while(valorBotao){
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
      if(tempoBotao <= 1000){
        tempoBotao = 0;
        i = i + 1;
        if(i==3){
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
  Serial.println("saiu");
  lcd.clear();
  lcd.home();
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
    servoPuxador.attach(servoPuxarLancador);
    servoGirador.attach(servoGiro);
    travaCatapulta.attach(servoTrava);
    //valorBotao = 0;
    bool travaSwitch = 0, valorBotaoAlvo = 0;
    valorBotao = !digitalRead(botao);
	valorBotaoAlvo = !digitalRead(botaoAlvo);
    travaSwitch = digitalRead(switchLancamento);
    //Serial.println(valorBotao);
    Serial.println(valorBotao);
    
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
          updateTempoLancamento=false;
          tempoLancamento=millis();
        }
        Serial.print("pontos ");
        Serial.println(pontos);
        valorBotaoAlvo=!digitalRead(botaoAlvo);
        if(millis()-tempoLancamento<2000){
          contadorPosServo = constrain(contadorPosServo,0,180);
          servoGirador.write(contadorPosServo);
          analogWrite(motorAlvoPos, 0);
          lancar();
          travaCatapulta.write(180);
        }
        else if (millis()-tempoLancamento<4000){
          lancando=false;
        }
        else if (millis()-tempoLancamento<6000){
          servoPuxador.write(0);
        }
        else if (millis()-tempoLancamento<8000){
          travaCatapulta.write(0);
        }
        else{
          botaoLancamentoApertado=false;
          analogWrite(motorAlvoPos, 255);
          delay(20);
        }
      }
      else if (!lancando){
        Serial.print("a");
        moverAlvo(velocidade);
        arrumar();
        girar();
      }

    }
    else{
      Serial.print("noy");
      moverAlvo(velocidade);
      arrumar();
      girar();
      if (valorBotao){
        lcd.clear();
        lcd.home();
        lcd.print("Trava acionada");
        delay(250);
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
  int t0 = millis()%3000;
  //Serial.println(contadorPosServo);
  if(t0<1500){
    contadorPosServo+=1;
    contadorPosServo = constrain(contadorPosServo,0,180);
  	servoGirador.write(contadorPosServo);
    //if(contadorPosServo==180){
    //  ladoGiro=false;    
    //}
  }
  else{
    contadorPosServo-=1;
    contadorPosServo = constrain(contadorPosServo,0,180);
  	servoGirador.write(contadorPosServo);
    //if(contadorPosServo==0){
    //  ladoGiro=true;    
    //}
  }
}

void lancar()
{
  lancando=true;
  analogWrite(motorAlvoPos, 0); // deixar parando o movimento do alvo durante o lançamento?
  analogWrite(motorAlvoNeg, 0);
  servoPuxador.write(180);
}

void arrumar()
{
  updateTempoLancamento=true;
  travaCatapulta.write(0);
  servoPuxador.write(180);
}
