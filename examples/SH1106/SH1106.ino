/*
 * This sketch is designed to interface with SH1106 based OLED LCD over
 * an I2C interface
 *
 * Based in part off of code from: http://wenku.baidu.com/view/43ebc40aba1aa8114431d940.html
 */

// Pull in the wire library to communicate with the LCD over I2C
#include <Wire.h>
#include <sh1106.h>
 


volatile boolean g_writeValue = false;
volatile int g_displayValue = 0;

sh1106_lcd *g_pLCD = NULL;
void setup()
{
  Serial.begin(9600);
  
  g_pLCD = sh1106_lcd::getInstance();

  if (g_pLCD != NULL)
  {
    g_pLCD->ClearScreen();
  }

  g_writeValue = true;
}

// Quick sketch showing functionality of library				
void loop()
{
  if (g_writeValue == true)
  {
    g_pLCD->DrawRectangle(0, 0, 100, 48, 2); // draw a box from 0, 0 to 100, 48
    g_pLCD->FillRectangle(25, 25, 30, 30);
    g_pLCD->Show();
    delay(500);
    
    g_pLCD->PrintLine("0123456789");
    g_pLCD->PrintLine("aBcDEF GHIJK");
    g_pLCD->PrintLine("LMNOP QRSTUV");
    g_pLCD->PrintLine("WXYZ");
    g_pLCD->PrintLine("AAAA");
    g_pLCD->PrintLine("EEEE");
    g_pLCD->PrintLine("FFFF");
    g_pLCD->PrintLine("GGGG");
    delay(250);
    g_pLCD->PrintLine("HHHH");
    
    g_pLCD->DrawPixel(0, 0, true);
    g_pLCD->DrawPixel(127, 0, true);
    g_pLCD->DrawPixel(127, 63, true);
    g_pLCD->DrawPixel(0, 63, true);
    g_pLCD->Show();
    delay(2000);
    g_writeValue = false;
  }
  else
  {
    g_pLCD->ClearScreen();
    g_pLCD->DrawLine(20, 31, 64, 31);
    g_pLCD->DrawLine(0, 0, 50, 50);
    g_pLCD->DrawLine(50, 50, 20, 20);
    g_pLCD->DrawLine(0, 50, 0, 20);
    
    g_pLCD->Show();
    while(1);
  }
}
