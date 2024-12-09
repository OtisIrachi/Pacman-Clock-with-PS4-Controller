 //*******************************************************************
// PacManClockPS4.ino
// Displays animated game sequence on 64x64 Matrix Display
// Program is modified from "jnthas" Mario Bros. Clock
//
// by RCI
//
// 1-24-2024
// Added Clock, Month Day, Date, changed FONTS
//
// 2-3-2024
// Created 4 'Ghost' bitmaps and added to game display 
// 
// 2/19/2024
// Added drawRGBBitmapDy to "Chomper.cpp"  which contains a delay to show the bitmap one line at a time
// 
// 3/15/2024
// Created new program to add Joystick Control for Game Play
// Trigger button selects Auto Clock Display or Manual Game play.
//
// 5-29-2024
// Added Bluepad32.h software to utilize a generic PS4 Bluetooth Controller
// for PACMAN control.
// Must Select "ESP Arduino + BluePad32 Board Manager for this to work!
//
// 8-26-2014
// Substituted Hex "Green" Color for Orange Hex Color in "Game.h"
//
//*******************************************************************
#include <WiFi.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include "Clockface.h"
#include "CWDateTime.h"
#include <ezTime.h>
#include <Bluepad32.h>

MatrixPanel_I2S_DMA *dma_display = nullptr;
ControllerPtr myControllers[BP32_MAX_GAMEPADS];

CWDateTime cwDateTime;
Clockface *clockface;
Timezone myTZ;

// Replace with your network credentials
const char *ssid = "RCINet2.4";
const char *password = "cae4844bb2";


extern int UP_DIR;
extern int DN_DIR;
extern int LT_DIR;
extern int RT_DIR;
extern int TRG_PB;

#define displayBrightness 127

#define PANEL_RES_X 64     // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 64     // Number of pixels tall of each INDIVIDUAL panel module.

#define PANEL_CHAIN 1      // Total number of panels chained one to another

//**********************************************
//   WEMOS D1 MINI 32 Daughterboard PINOUTS
//**********************************************
//        VAR   IO PIN   BOARD LABEL    COLOR        DIN
#define R1_PIN  33    // IO33           BROWN 1       1        
#define G1_PIN  26    // IO26-D0        RED 1         2
#define B1_PIN  27    // IO27           ORANGE 1      3
                      // GND            YELLOW 1      4
#define R2_PIN  0     // IO0            GREEN 1       5
#define G2_PIN  18    // IO18-D5-SCK    BLUE 1        6
#define B2_PIN  19    // IO19-D6-MISO   PURPLE        7
#define E_PIN   32    // IO32           GREY          8
#define A_PIN   23    // IO23-D7-MOSI   WHITE         9   
#define B_PIN   12    // IO12-TDI       BLACK         10
#define C_PIN   5     // IO5-D8-CS      BROWN 2       11
#define D_PIN   17    // IO17           RED 2         12         
#define CLK_PIN 16    // IO16           ORANGE 2      13
#define LAT_PIN 4     // IO4            YELLOW 2      14
#define OE_PIN  25    // IO25           GREEN 2       15
                      //                BLUE 2 SPARE  16
#define UP_PB   21    // Joystick Pin 1
#define DN_PB   14    // Joystick Pin 2  TMS
#define LT_PB   13    // Joystick Pin 3  TCK
#define RT_PB   15    // Joystick Pin 4  TD0
#define TRIGG   22    // Joystick PB Pin 6

uint16_t myBLACK = dma_display->color565(0, 0, 0);
uint16_t myWHITE = dma_display->color565(255, 255, 255);
uint16_t myBLUE = dma_display->color565(0, 0, 255);
//***********************************************************************************************
// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) 
{
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) 
        {
        if (myControllers[i] == nullptr) 
            {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
            // Additionally, you can get certain gamepad properties like:
            // Model, VID, PID, BTAddr, flags, etc.
            ControllerProperties properties = ctl->getProperties();
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                           properties.product_id);
            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
            }
        }
    if (!foundEmptySlot) 
        {
        Serial.println("CALLBACK: Controller connected, but could not found empty slot");
        }
}
//***********************************************************************************************
void onDisconnectedController(ControllerPtr ctl) 
{
    bool foundController = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) 
        {
        if (myControllers[i] == ctl) 
            {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
            }
        }

    if (!foundController) 
        {  
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
        }
}//***********************************************************************************************
void processGamepad(ControllerPtr ctl) 
{

  if (ctl->axisY() <= -500)
    {
    Serial.println("UP");
    UP_DIR = 1;
    }
  else
    {
    UP_DIR = 0;
    }
       
  if (ctl->axisY() >= 510)
    {
    Serial.println("DOWN");
    DN_DIR = 1; 
    }
  else
    {
    DN_DIR = 0;
    }
    
  if (ctl->axisX() >= 510)
    {
    Serial.println("RIGHT");
    RT_DIR = 1;
    }
  else
    {
    RT_DIR = 0;
    }
    
  if (ctl->axisX() <= -500)
    {
    Serial.println("LEFT");
    LT_DIR = 1;
    }
  else
    {
    LT_DIR = 0; 
    }
    
  if (ctl->r1())
    {
    Serial.println("Trigger");
    TRG_PB = 1; 
    }
  else
    {
    TRG_PB = 0;
    }
    
      
}
/*
//***********************************************************************************************
void processGamepad(ControllerPtr ctl) 
{

  if (ctl->y()) 
    {
    Serial.println("UP");
    UP_DIR = 1;
    }
  else
    {
    UP_DIR = 0;
    }
       
  if (ctl->a()) 
    {
    Serial.println("DOWN");
    DN_DIR = 1; 
    }
  else
    {
    DN_DIR = 0;
    }
    
  if (ctl->b()) 
    {
    Serial.println("RIGHT");
    RT_DIR = 1;
    }
  else
    {
    RT_DIR = 0;
    }
    
  if (ctl->x()) 
    {
    Serial.println("LEFT");
    LT_DIR = 1;
    }
  else
    {
    LT_DIR = 0; 
    }
    
  if (ctl->l1())
    {
    Serial.println("Trigger");
    TRG_PB = 1; 
    }
  else
    {
    TRG_PB = 0;
    }
    
      
}
*/
//***********************************************************************************************
void getGamepad() 
{
    BP32.update();

    ControllerPtr myController = myControllers[0];

    if (myController && myController->isConnected()) 
       {
       if (myController->isGamepad()) 
          {
          processGamepad(myController);
          }   
       }
 
}
//**********************************************************************************************
void displaySetup() 
{
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,   // module width
    PANEL_RES_Y,   // module height
    PANEL_CHAIN    // Chain length
    );

  //******* 64x64 RGB MAtirx Display Setup *******
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);

  // ***** Use this if you want to use your own Pins ****
  dma_display->begin(R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN);   
  dma_display->setBrightness8(displayBrightness); //0-255
  dma_display->clearScreen();
}

//**********************************************************************************************
void setup() 
{
  Serial.begin(115200);

  pinMode(DN_PB, INPUT_PULLUP);
  pinMode(UP_PB, INPUT_PULLUP);
  pinMode(LT_PB, INPUT_PULLUP);
  pinMode(RT_PB, INPUT_PULLUP);
  pinMode(TRIGG, INPUT_PULLUP);

  displaySetup();
  clockface = new Clockface(dma_display);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) 
       {
       dma_display->setTextSize(1);
       dma_display->setTextColor(myWHITE);
       dma_display->setCursor(4, 4);
       dma_display->println("Pac-Man");
       dma_display->println(" Clock");
 
       dma_display->setTextColor(myBLUE);
       dma_display->setCursor(4, 25);
       dma_display->println("Waiting");
       dma_display->println("for WiFi");
       delay(2000);
       Serial.print(".");       
       }
  
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

  dma_display->fillRect(0, 0, 64, 64, 0x0000);

  // Setup the Bluepad32 callbacks
  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(false);

  dma_display->setTextColor(myBLUE);
  dma_display->setCursor(0, 25);
  dma_display->println("Pair");
  dma_display->println("Bluetooth");
  dma_display->println("Controller");

  //delay(5000);

//  wifi.connect();  // This is used if you wish to connect to a foriegn network

  cwDateTime.begin();
  clockface->setup(&cwDateTime);

}
//**********************************************************************************************
void loop() 
{
  
  clockface->update();

  getGamepad();
  vTaskDelay(1);
   
}
//**********************************************************************************************
//**********************************************************************************************
