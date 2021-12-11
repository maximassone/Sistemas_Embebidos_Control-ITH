#include "sapi.h"        // <= Inclusion de la Biblioteca sAPI


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




// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void )
{
   // ---------- CONFIGURACIONES ----------------------------------------------
   //DHT11 --------------------------------------------------------------------
   boardConfig(); // Inicializar y configurar la plataforma
   uartConfig( UART_USB, 115200 ); // Inicializar periferico UART_USB

   dht11Init( GPIO1 ); // Inicializo el sensor DHT11

   // Variables para almacenar humedad y temperatura
   float humidity = 0, temperature = 0;
   float ITH;
  

   //LCD -----------------------------------------------------------------------
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

   //Parametros de Prueba
   char ONString[] = "ON";
   char OFFString[] = "OFF";
   int UMBRAL =  68;


   lcdCursorSet( LCD_CURSOR_OFF ); // Apaga el cursor
   lcdClear();                     // Borrar la pantalla




   // ---------- REPETIR POR SIEMPRE --------------------------------------
   while( TRUE ) {

      //DHT11 -------------------------------------------------------------
      // Lectura del sensor DHT11, devuelve true si pudo leer correctamente
      if( dht11Read(&humidity, &temperature) ) {
         // Si leyo bien prendo el LEDG y enciendo el LEDR
         gpioWrite( LEDG, ON );
         gpioWrite( LEDR, OFF );
         // Informo los valores de los sensores
         printf( "Temperatura: %.2f grados C.\r\n", temperature );
         printf( "Humedad: %.2f  %.\r\n", humidity );
         ITH = ((0.8*temperature)+(humidity/100)*(temperature-14.4)+46.4);
               printf( "ITH: %.2f \r\n\r", ith );
               printf( "\r\n");
      } else {
         // Si leyo mal apago el LEDG y enciendo el LEDR
         gpioWrite( LEDG, OFF );
         gpioWrite( LEDR, ON );
         // Informo el error de lectura
         printf( "Error al leer DHT11.\r\n\r\n");
      }
      delay(1000); // Espero 1 segundo.

    //LCD --------------------------------------------------------------------
      lcdGoToXY( 0, 0 ); // Poner cursor en 0, 0
      lcdSendStringRaw( "Tmp" );
      lcdData(TEMP_CHAR);
      lcdSendStringRaw( " Hum" );
      lcdData(HUM_CHAR);
      lcdSendStringRaw( " Estado" );
      lcdData(EST_CHAR);

      lcdGoToXY( 0, 1 );
      lcdSendInt( temperature );
      lcdData(GRA_CHAR);
      lcdSendStringRaw( "C" );

      lcdGoToXY( 5, 1 );
      lcdSendInt( humidity );
      lcdSendStringRaw( "%" );

   if(ITH<UMBRAL){
      lcdGoToXY( 10, 1 );
      lcdSendStringRaw( OFFString );
      }

   if(ITH>=UMBRAL){
      lcdGoToXY( 10, 1 );
      lcdSendStringRaw( ONString );
      }
      delay(1000);



   }
   // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
   // directamente sobre un microcontrolador y no es llamado por ningun
   // Sistema Operativo, como en el caso de un programa para PC.
   return 0;
}
