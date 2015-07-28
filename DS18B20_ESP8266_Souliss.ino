/**************************************************************************
    Souliss - Hello World for Expressif ESP8266
    
    This is the basic example, create a software push-button on Android
    using SoulissApp (get it from Play Store).  
    
    Load this code on ESP8266 board using the porting of the Arduino core
    for this platform.
        
***************************************************************************/

// Configure the framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/IPBroadcast.h"

// **** Define the WiFi name and password ****
#define WIFICONF_INSKETCH
#define WiFi_SSID               "BabyAieNiMua"
#define WiFi_Password           "ABCDEDCBA"    

// Include framework code and libraries
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "Souliss.h"

// This identify the number of the LED logic
#define MYLEDLOGIC          0        
// This identify the number of the SLOT logic
#define DALLAS              1       

#include <OneWire.h>
#include <DallasTemperature.h>
#define DALLASPIN 2 //Se declara el pin donde se conectará la DATA
OneWire ourWire(DALLASPIN); //Se establece el pin declarado como bus para la comunicación OneWire
DallasTemperature sensors(&ourWire); //Se instancia la librería DallasTemperature

// **** Define here the right pin for your ESP module **** 
#define	OUTPUTPIN			BUILTIN_LED

void setup()
{   
    Initialize();

      Serial.begin(115200);
    sensors.begin(); //Se inician los sensores DS18B20


    // Connect to the WiFi network and get an address from DHCP
    GetIPAddress();                           
    SetAsGateway(myvNet_dhcp);       // Set this node as gateway for SoulissApp  

    // This is the vNet address for this node, used to communicate with other
	// nodes in your Souliss network
    SetAddress(0xAB01, 0xFF00, 0x0000);
    SetAsPeerNode(0xAB02, 1);
	
    Set_SimpleLight(MYLEDLOGIC);        // Define a simple LED light logic

    Set_Temperature(DALLAS);

	
    pinMode(OUTPUTPIN, OUTPUT);         // Use pin as output 
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   

        FAST_910ms()    {
          // Acquire temperature from the microcontroller ADC
          sensors.requestTemperatures(); //Prepara el sensor para la lectura
          float dallas = sensors.getTempCByIndex(0);
          Souliss_ImportAnalog(memory_map, DALLAS, &dallas);
        } 

                FAST_2110ms()
        {
           Logic_Temperature(DALLAS);
        }
        
        FAST_50ms() {   // We process the logic and relevant input and output every 50 milliseconds
            Logic_SimpleLight(MYLEDLOGIC);
            DigOut(OUTPUTPIN, Souliss_T1n_Coil,MYLEDLOGIC);
        } 
              
        // Here we handle here the communication with Android
        FAST_GatewayComms();                                        
    }
} 
