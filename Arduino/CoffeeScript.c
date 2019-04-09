/*
* A simple Wireless hack for any coffee pot
* to add wireless control over various settings using the D1 Mini or 
* developed by Derek Sisco
*/

/*
 * TODO: 
 * I need more materials to setup further connections
 * and so that I can read data from the coffee pot to establish what settings it is on
 * this project requires 1 d1mini arduino, a computer to code on, and 3 wires...and a lot of boredom.
 * boredom is a must.
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>


#define PIN_BREW_NOW 16 //D0
#define PIN_BREW_BOLD LED_BUILTIN
#define PIN_BREW_ON 5 //D1 input pin
#define PIN_BREW_NOW_GROUND 15

const char *ssid = "PomeroySisco";
const char *pass = "NeonZebraChaos";
const int port = 1234;

#define STATUS_LIGHT LED_BUILTIN
#define CP_POWER_PIN 16

bool client_locked = false;


//Wifi Server Init on port 1234
WiFiServer server(port);
WiFiClient client;
void ToggleMachineState(){
    digitalWrite(PIN_BREW_NOW,LOW);
    delay(100);
    digitalWrite(PIN_BREW_NOW,HIGH);
}

int SetMachineBrewNow(){
    if(GetMachineState() == 1){
        return 1;
    }
    ToggleMachineState();
    return 0;
}
int SetMachineOff(){
    if(GetMachineState() == 0){
        return 1;
    }
    ToggleMachineState();
    return 0;
}

int GetMachineState(){
    int val = digitalRead(PIN_BREW_ON);
    return val;
}


void setup(){
    Serial.begin(115200);
    pinMode(PIN_BREW_BOLD,OUTPUT); 
    pinMode(PIN_BREW_NOW,OUTPUT); 
    pinMode(PIN_BREW_ON,INPUT); 
    pinMode(PIN_BREW_NOW_GROUND, INPUT);

    //Ensure the coffee pot is turned off on when orginally intitializing 
    digitalWrite(PIN_BREW_NOW_GROUND, LOW);
    SetMachineOff();
    
    WiFi.begin( ssid, pass );
    Serial.printf("Connecting to %s...\n", ssid);
    while(WiFi.status() != WL_CONNECTED){
        Serial.println("Establishing Connection, Standby..."); 
        delay(2000);
    }
    Serial.println("Starting Server...");
    server.begin();
    Serial.println("Server Finished...");
    Serial.print("Address:");
    Serial.print(WiFi.localIP());
    Serial.print(':');
    Serial.println(port);
    
}

bool process( char brew_now, char brew_bold)
{
    
    Serial.printf("Command: Brewnow[%c], BrewBold[%c], machine status[%d]", brew_now, brew_bold, GetMachineState());
    Serial.println();
    int result = 0;
    if(brew_bold == '1'){
        //pulse(PIN_BREW_BOLD);
    }
    if(brew_now == '1') {
        result = SetMachineBrewNow();
        if(result != 0) {
            client.write("Machine is Already Brewing\n");
            return false;
        } else {
            client.write("Machine has Brewing has Started\n");
        }
    }
    if(brew_now == '0'){
        result = SetMachineOff();
        if (result != 0){
            client.write("Machine is Already Off\n");
            return false;
        } else {
            client.write("Machine has been turned off\n");
        }
    }
    return true;
}


void loop(){
    client = server.available();
    if(client){
        int pack_len = 2;
        while(client.connected())
        {
            while(client.available())
            {
                String cmd = client.readStringUntil('\n');
                //cmd to tell coffee pot to turn on or off
                char bn = cmd.charAt(0);
                //cmd to set brew strength (not available yet)
                char bb = cmd.charAt(1);
                bool result = process(bn,bb);
            }
        }
    }
}
