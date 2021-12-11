// NOTA -------------------------------------------------------------
// FUNCIONA MEDICION Y TOMA DE DECISION POR ITH
// FUNCIONA ACTICACION DE LEDs (REEMPLAZAR POR GPIO RELAYS)
// FUNCIONA MAQUINA DE ESTADOS Y TRANSICIONES POR EVENTOS
// FUNCIONA DISPLAY CON IMPRESION DE DATOS

//RESTA CONDICION MEDICION CADA 30MIN

#include "sapi.h"        // <= Inclusion de la Biblioteca sAPI


/*=====================[Logos LCD]=================================*/
// Definir caracteres personalizados: https://omerk.github.io/lcdchargen/
//Temperatura - Termometro
const char tempChar[8] = {
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
const char humChar[8] = {
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
const char graChar[8] = {
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
   EST_CHAR  = 2,
   GRA_CHAR  = 3,
};

/*==================[Variables DHT11]==============================*/
	float UMBRAL = 68; //Valor limitante de estres por calor para ganado lechero
	float humidity = 0, temperature = 0;
    float ITH;
    char ONString[] = "ON";
    char OFFString[] = "OFF";


/*===================[Variables Maquina de estados]==========================*/
      enum{
        INICIANDO,                        //Variable de estado iniciacion
        MIDIENDO,                      //Variable de estado medicion
        APAGADO,                       //Variable de estado apagado
        ENCENDIDO,                     //Variable de estado encendido
       };
      int ESTADO = 0;                  //Variable de estado control (Maquina de Estados)


/*=========================[Configuraciones]=================================*/
void CONFIG_DHT11(void){
   // ---------- CONFIGURACIONES ----------------------------------------------
   //DHT11 --------------------------------------------------------------------
      boardConfig(); // Inicializar y configurar la plataforma
      uartConfig( UART_USB, 115200 ); // Inicializar periferico UART_USB
      dht11Init( GPIO8 ); // Inicializo el sensor DHT11
	}

void CONFIG_LCD(void){
   i2cInit( I2C0, 100000 );
   delay( LCD_STARTUP_WAIT_MS );   // Wait for stable power (some LCD need that)
   // Inicializar LCD de 16x2 (caracteres x lineas) con cada caracter de 5x8 pixeles
   lcdInit( 16, 2, 5, 8 );
   // Cargar el caracter a CGRAM
   // El primer parametro es el codigo del caracter (0 a 7).
   // El segundo es el puntero donde se guarda el bitmap (el array declarado anteriormente)
   lcdCreateChar( TEMP_CHAR, tempChar );
   lcdCreateChar( HUM_CHAR, humChar );
   lcdCreateChar( GRA_CHAR, graChar );
   lcdCursorSet( LCD_CURSOR_OFF ); // Apaga el cursor
   lcdClear();                     // Borrar la pantalla
   }

/*============================[Funciones]====================================*/
float CONTROL_ITH(float t, float h){
   //DHT11 -------------------------------------------------------------
   // Lectura del sensor DHT11, devuelve true si pudo leer correctamente
      float ith;
      if( dht11Read(&h, &t)) {
          humidity = h;
          temperature = t;
          ith = ((0.8*t)+(h/100)*(t-14.4)+46.4);
         	 }
     else {
            printf( "Error al leer DHT11.\r\n\r\n");
         	 }

      	 delay(1000); // Espero 1 segundo.
         return ith;
	}



// Valores por LCD -------------------------------------------------
void SALUDO_LCD(void){
   lcdCursorSet( LCD_CURSOR_OFF ); // Apaga el cursor
   lcdClear();                     // Borrar la pantalla
   lcdGoToXY( 0, 0 ); // Poner cursor en 0, 0
   lcdSendStringRaw( "Bienvenido a" );
   lcdGoToXY( 0, 1 ); // Poner cursor en 0, 1
   lcdSendStringRaw( "OSCORP" );
   delay(2000);
   lcdClear();                     // Borrar la pantalla
}

void INICIADO_LCD(void){
   lcdCursorSet( LCD_CURSOR_OFF ); // Apaga el cursor
   //lcdClear();                     // Borrar la pantalla
   lcdGoToXY( 0, 0 ); // Poner cursor en 0, 0
   lcdSendStringRaw( "Control ITH" );
   lcdGoToXY( 0, 1 ); // Poner cursor en 0, 0
   lcdSendStringRaw( "Ing. Massone" );
   //delay(2000);
   //lcdClear();                     // Borrar la pantalla
}


void PLOT_LCD(float t, float h, float ITH){

   //  lcdClear();        // Borrar la pantalla
     lcdGoToXY( 0, 0 ); // Poner cursor en 0, 0
     lcdSendStringRaw( "Estado" );
     lcdSendStringRaw( " Tmp" );
     lcdData(TEMP_CHAR);
     lcdSendStringRaw( " Hum" );
     lcdData(HUM_CHAR);

     if(ITH<UMBRAL){
     lcdGoToXY( 0, 1 );
     lcdSendStringRaw( OFFString );}
     if(ITH>=UMBRAL){
     lcdGoToXY( 0, 1 );
     lcdSendStringRaw( ONString );}

     lcdGoToXY( 6, 1 );
     lcdSendInt( t );
     lcdData(GRA_CHAR);
     lcdSendStringRaw( "C" );
     lcdGoToXY( 12, 1 );
     lcdSendInt( h );
     lcdSendStringRaw( "%" );
     delay(1000);
     }



// Valores por serie -------------------------------------------------
void SALUDO_DATA(void){
   printf("Bienvenido a OSCORP \r\n\r\n");
}



void PLOT_DATA(float t, float h, float ith){


    //Muestro por serie diversas variables
    printf("############## VALORES POR SERIE ############### \r\n");
    printf("ESTADO: ");
    if(ESTADO ==0){printf("INICIANDO\r\n");}
    if(ESTADO ==1){printf("MIDIENDO\r\n");}
    if(ESTADO ==2){printf("APAGADO\r\n");}
    if(ESTADO ==3){printf("ENCENDIDO\r\n");}

  //Impresion de mediciones
    printf("% Temperatura: %.2f C\r\n",t);
    printf("Humedad: %.2f .%\r\n", h);

  //Impresion por serial resultado indice ITH
    printf( "ITH: %.2f \r\n", ith);
    printf("################################################\r\n");
    printf( "\r\n");
	}



//Funciones de maquinas de estados -------------------------------------------------------------------------------------------
void ITH_INI(void){
        //gpioWrite(FANPIN,      HIGH);         //Desactiva salida reley FAN 1
        //gpioWrite(FANPIN2,     HIGH);         //Desactiva salida reley FAN 2
        //gpioWrite(FANPIN3,     HIGH);         //Desactiva salida reley FAN 3
        //gpioWrite(FANPIN4,     HIGH);         //Desactiva salida reley FAN 4
       gpioWrite(LEDB,     LOW);         //Desactiva salida reley FAN 1
       gpioWrite(LED1,     LOW);         //Desactiva salida reley FAN 2
       gpioWrite(LED2,     LOW);         //Desactiva salida reley FAN 3
       gpioWrite(LED3,     LOW);         //Desactiva salida reley FAN 4
       INICIADO_LCD();

        if(gpioRead(TEC1)==LOW){
          lcdClear();                     // Borrar la pantalla
          ESTADO=MIDIENDO;
       }



    //    if(gpioRead(SELECTOR)==LOW){
    //          ESTADO=MIDIENDO;}



        }

 void ITH_MED(void){
	 	 ITH = CONTROL_ITH(temperature, humidity);
         if(ITH<UMBRAL){             //Estado del sistema APAGADO
          ESTADO = APAGADO;
          lcdClear();                     // Borrar la pantalla
          PLOT_DATA(temperature, humidity, ITH);
          PLOT_LCD(temperature, humidity, ITH);
          return;}


        if(ITH>UMBRAL){         //Estado del sistema ENCENDIDO
          ESTADO = ENCENDIDO;
          lcdClear();                     // Borrar la pantalla
          PLOT_DATA(temperature, humidity, ITH);
          PLOT_LCD(temperature, humidity, ITH);
          return;}
        }


void ITH_APAGADO(void){
         //gpioWrite(FANPIN,      HIGH);         //Desactiva salida reley FAN 1
        //gpioWrite(FANPIN2,     HIGH);         //Desactiva salida reley FAN 2
        //gpioWrite(FANPIN3,     HIGH);         //Desactiva salida reley FAN 3
        //gpioWrite(FANPIN4,     HIGH);         //Desactiva salida reley FAN 4
        gpioWrite(LEDB,     LOW);         //Desactiva salida reley FAN 1
        gpioWrite(LED1,     LOW);         //Desactiva salida reley FAN 2
        gpioWrite(LED2,     LOW);         //Desactiva salida reley FAN 3
        gpioWrite(LED3,     LOW);         //Desactiva salida reley FAN 4
        PLOT_LCD(temperature, humidity, ITH);

        if(gpioRead(TEC1)==HIGH){
                 ESTADO=INICIANDO;
                 lcdClear();                     // Borrar la pantalla
                 INICIADO_LCD();
                 printf("ESTADO: INICIANDO\r\n\r\n");
              }

    //   if(millis()- tiempo_med > tiempo_medicion){
    //    ESTADO=MIDIENDO;
    //   }

    //    if(gpioRead(SELECTOR)==HIGH){
    //          ESTADO=INICIANDO;
    //         }

  }



void ITH_ENCENDIDO(void){
       // gpioWrite(FANPIN,      LOW);         //Activa salida reley FAN 1
       // gpioWrite(FANPIN2,     LOW);         //Activa salida reley FAN 2
       // gpioWrite(FANPIN3,     LOW);         //Activa salida reley FAN 3
       // gpioWrite(FANPIN4,     LOW);         //Activa salida reley FAN 4
        gpioWrite(LEDB,     HIGH);         //Activa salida reley FAN 1
        gpioWrite(LED1,     HIGH);         //Activa salida reley FAN 2
        gpioWrite(LED2,     HIGH);         //Activa salida reley FAN 3
        gpioWrite(LED3,     HIGH);         //Activa salida reley FAN 4
        PLOT_LCD(temperature, humidity, ITH);

        if(gpioRead(TEC1)==HIGH){
                      ESTADO=INICIANDO;
                      lcdClear();                     // Borrar la pantalla
                      INICIADO_LCD();
                      printf("ESTADO: INICIANDO\r\n\r\n");
                    }

    //   if(millis()- tiempo_med > tiempo_medicion){
    //    ESTADO=MIDIENDO;
    //   }

    //    if(gpioRead(SELECTOR)==HIGH){
    //          ESTADO=INICIANDO;
    //         }

  }



/*==================[Programa]==============================*/
// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void )
{
   // ---------- CONFIGURACIONES ----------------------------
   CONFIG_DHT11(); //Conf. DHT11
   CONFIG_LCD();   //Conf. LCD
   SALUDO_LCD();
   SALUDO_DATA();

   // ---------- REPETIR POR SIEMPRE ------------------------
   while( TRUE ) {
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
   return 0;
}