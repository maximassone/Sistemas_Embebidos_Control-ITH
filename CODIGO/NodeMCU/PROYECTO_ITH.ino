//*************************************************************************************************************************//
//                                           FIUBA   - Navarro Pcia. Bs As, Argentina                                      //
//                                           Desarrollo: Ing.Carlos Maximiliano Massone                                    //
//                                                 Control ITH para Tambo                                                  //
//                                                         Version 1.0                                                     //
//*************************************************************************************************************************//

//Liberias
#include "Wire.h"
#include <LiquidCrystal_I2C.h>
#include "DHT.h"    //Sensor DHT11
#define DHTPIN 13   //Puerto seleccionado D7 (Lolin)
#define DHTTYPE DHT11 
#define ADRESS_LCD 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Definicion de puertos y sensores
DHT dht(DHTPIN, DHTTYPE);
#define FANPIN        0                                        // Puerto seleccionado D3 (Lolin)   ASPER
#define FANPIN2       2                                        // Puerto seleccionado D4 (Lolin)   FAN1  
#define FANPIN3      14                                        // Puerto seleccionado D5 (Lolin)   FAN2  
#define FANPIN4      12                                        // Puerto seleccionado D6 (Lolin)   FAN3  
#define PALANCA      15                                        // Puerto seleccionado D8 (Lolin)   PALANCA  

//Definicion de los intervalos de decision 
const int UMBRAL      = 68;                                    //Valor Umbral para el indice ITH 68

//Varibales Maquina de estados
enum{
  INICIANDO,                                                   //Variable de estado iniciacion
  MIDIENDO,                                                    //Variable de estado medicion
  APAGADO,                                                     //Variable de estado apagado
  ENCENDIDO,                                                   //Variable de estado encendido 
 };

//Definicion de varibales                                       
int ESTADO = 0;                                                //Variable de estado control (Maquina de Estados)
float t =0;                                                    //Variable Temperatura
float h =0;                                                    //Variable Humedad
float ith =0;                                                  //Variable Indice ITH

//Definicion de tiempos
const unsigned long  tiempo_medicion=180000;                  //Tiempo de espera entre mediciones (30min=1800000) 
unsigned long  tiempo_ini;                                    //Variable para tiempo de espera inicio
unsigned long  tiempo_med;                                    //Variable para tiempo de espera medicion 

/*=====================[Logos LCD]=================================*/
// Definir caracteres personalizados: https://omerk.github.io/lcdchargen/
//Temperatura - Termometro
byte  tempChar[8] = {
   0b01110,
   0b01010,
   0b01010,
   0b01110,
   0b01110,
   0b10111,
   0b11111,
   0b01110
};

// Humedad - Gota
byte humChar[8] = {
   0b00100,
   0b00100,
   0b01110,
   0b10111,
   0b10111,
   0b10011,
   0b01110,
   0b00000
};


// Simbolo grados
byte  graChar[8] = {
   0b01110,
   0b01010,
   0b01110,
   0b00000,
   0b00000,
   0b00000,
   0b00000,
   0b00000
};

enum{
   TEMP_CHAR = 0,
   HUM_CHAR  = 1,
   GRA_CHAR  = 2,
};

/*=========================[Configuraciones]=================================*/
void CONFIG_LCD(void){
    lcd.init();
    lcd.backlight();
    lcd.clear();
    }    

/*============================[Funciones]====================================*/
float CONTROL_ITH(float t, float h){
   //DHT11 -------------------------------------------------------------  
          h = dht.readHumidity();
          t = dht.readTemperature();
          ith = ((0.8*t)+(h/100)*(t-14.4)+46.4);
          delay(1000); // Espero 1 segundo.
          return ith;
          }


void MONITOREO_TH(float t, float h){
   //DHT11 -------------------------------------------------------------
          h = dht.readHumidity();
          t = dht.readTemperature();
          delay(1000); // Espero 1 segundo.
          }


// Inicializacion Pines ----------------------------------------------------------------------------
void CONFIG_PINS(void){
    pinMode(PALANCA, INPUT);                                  //PALANCA definido como entrada (Reley)     
    pinMode(FANPIN,OUTPUT);                                   //FANPIN definido como salidad (Reley)
    digitalWrite(FANPIN,    HIGH);                            //Desactiva salida reley FAN 1
    pinMode(FANPIN2,OUTPUT);                                  //FANPIN2 definido como salidad (Reley)
    digitalWrite(FANPIN2,   HIGH);                            //Desactiva salida reley FAN 2
    pinMode(FANPIN3,OUTPUT);                                  //FANPIN definido como salidad (Reley)
    digitalWrite(FANPIN3,    HIGH);                           //Desactiva salida reley FAN 3
    pinMode(FANPIN4,OUTPUT);                                  //FANPIN4 definido como salidad (Reley)
    digitalWrite(FANPIN4,   HIGH);                            //Desactiva salida reley FAN 4
}



// Valores por LCD ---------------------------------------------------------------------------------
void CREACION_CARACTERES(void){
  //creamos los nuevos caracteres
  lcd.createChar (TEMP_CHAR,tempChar);
  lcd.createChar (HUM_CHAR,humChar);
  lcd.createChar (GRA_CHAR,graChar);
  }

void SALUDO_LCD(void){
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("Bienvenido a");
     lcd.setCursor(0, 1);
     lcd.print("OSCORP");
     delay(2000);
     lcd.clear();
}

void INICIADO_LCD(void){
     lcd.setCursor(0, 0);
     lcd.print( "Control ITH");
     lcd.setCursor(0, 1);
     lcd.print("Ing. Massone");
     delay(1000);
}

void PLOT_LCD(float t, float h, float ITH){
     h = dht.readHumidity();
     t = dht.readTemperature();
     int T=t, H=h;
     lcd.setCursor(0, 0);
     lcd.print( "Estado");
     lcd.setCursor(5, 0);
     lcd.print( " Tmp");
     lcd.write (TEMP_CHAR);
     lcd.setCursor(10, 0);
     lcd.print( " Hum");
     lcd.write (HUM_CHAR);

     if(ITH<UMBRAL){
     lcd.setCursor(0, 1);
     lcd.print("OFF");}
     if(ITH>=UMBRAL){
     lcd.setCursor(0, 1);
     lcd.print("ON");}

     lcd.setCursor(6, 1);
     lcd.print(T);
     lcd.write (GRA_CHAR);
     lcd.print( "C" );
     lcd.setCursor(12, 1);
     lcd.print( H );
     lcd.print( "%" );
     delay(1000);
     }


// Valores por serie -------------------------------------------------------------------------------
void SALUDO_DATA(void){
   Serial.println("Bienvenido a OSCORP");
}


void PLOT_DATA(float t, float h, float ith){

    //Muestro por serie diversas variables
    Serial.println("############## VALORES POR SERIE ###############");
    Serial.print("ESTADO: ");
    if(ESTADO ==0){Serial.println("INICIANDO");}
    if(ESTADO ==1){Serial.println("MIDIENDO");}
    if(ESTADO ==2){Serial.println("APAGADO");}
    if(ESTADO ==3){Serial.println("ENCENDIDO");}

    h = dht.readHumidity();
    t = dht.readTemperature();
  //Impresion de mediciones
    Serial.print("Temperatura: ");
    Serial.print(t);
    Serial.print(" °C");  
    Serial.print(" Humedad: ");
    Serial.print(h);
    Serial.println(" %");

  //Impresion por serial resultado indice ITH
    Serial.print( "ITH: "); 
    Serial.println(ith);
    Serial.println("################################################");

   }

//Funciones de maquinas de estados --------------------------------------------------------------
void ITH_INI(void){
        tiempo_ini = millis();
        digitalWrite(FANPIN,      HIGH);                     //Desactiva salida reley FAN 1
        digitalWrite(FANPIN2,     HIGH);                     //Desactiva salida reley FAN 2
        digitalWrite(FANPIN3,     HIGH);                     //Desactiva salida reley FAN 3
        digitalWrite(FANPIN4,     HIGH);                     //Desactiva salida reley FAN 4
        INICIADO_LCD();                                      //Visualizacion 
         
        if(digitalRead(PALANCA)==LOW){
              lcd.clear();
              ESTADO=MIDIENDO;}       
        }

        
 void ITH_MED(void){
          tiempo_med = millis();
          ith = CONTROL_ITH(t, h);

          if(ith<UMBRAL){                                   //Estado del sistema APAGADO
            ESTADO = APAGADO;
            lcd.clear();
            PLOT_DATA(t, h, ith);
            PLOT_LCD(t, h, ith);
            lcd.clear();
            return;}

          if(ith>UMBRAL){                                   //Estado del sistema ENCENDIDO
            ESTADO = ENCENDIDO;
            lcd.clear();     
            PLOT_DATA(t, h, ith);
            PLOT_LCD(t, h, ith);
            lcd.clear();
            return;}  
          }


void ITH_APAGADO(void){
        digitalWrite(FANPIN,      HIGH);                  //Desactiva salida reley FAN 1
        digitalWrite(FANPIN2,     HIGH);                  //Desactiva salida reley FAN 2
        digitalWrite(FANPIN3,     HIGH);                  //Desactiva salida reley FAN 3
        digitalWrite(FANPIN4,     HIGH);                  //Desactiva salida reley FAN 4
        PLOT_LCD(t, h, ith);
        MONITOREO_TH(t, h);
      
       if(millis()- tiempo_med > tiempo_medicion){
        ESTADO=MIDIENDO;
        lcd.clear();      
        INICIADO_LCD();
        printf("ESTADO: INICIANDO POR TIMER\r\n\r\n");
       }

       if(digitalRead(PALANCA)==HIGH){
         ESTADO=INICIANDO;
         lcd.clear();     
         INICIADO_LCD();
         printf("ESTADO: INICIANDO\r\n\r\n");
         }

  }


void  ITH_ENCENDIDO(void){
        digitalWrite(FANPIN,   LOW);                     //Activa salida reley FAN 1
        digitalWrite(FANPIN2,  LOW);                     //Activa salida reley FAN 2
        digitalWrite(FANPIN3,  LOW);                     //Activa salida reley FAN 3
        digitalWrite(FANPIN4,  LOW);                     //Activa salida reley FAN 4
        PLOT_LCD(t, h, ith);
        MONITOREO_TH(t, h);          
          
        if((millis()- tiempo_med) > tiempo_medicion){
        ESTADO=MIDIENDO;
        lcd.clear(); 
        INICIADO_LCD();
        printf("ESTADO: INICIANDO POR TIMER\r\n\r\n");
       }

      if(digitalRead(PALANCA)==HIGH){
         ESTADO=INICIANDO;
         lcd.clear();
         INICIADO_LCD();
         printf("ESTADO: INICIANDO\r\n\r\n");
         }
  }    


//***************************************** Programa **************************************************
void setup() {
    Serial.begin(115200); 
    dht.begin();
    CONFIG_LCD();
    delay(250);
    CONFIG_PINS();
    SALUDO_LCD();
    CREACION_CARACTERES();
    SALUDO_DATA(); 
    tiempo_ini = millis();
    }

//******************************************* Loop *****************************************************
void loop() {
 switch(ESTADO){
       
       case(INICIANDO):
       ITH_INI();
       break;

      case(MIDIENDO):
       ITH_MED();
       break;
      
      case(APAGADO):
      ITH_APAGADO();
      break;

      case(ENCENDIDO):
      ITH_ENCENDIDO();
      break; 
      }
}
