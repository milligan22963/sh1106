//
//  sh1106.h
//  
//
//  Created by Daniel Milligan on 8/3/14.
//
//

#ifndef _sh1106_h
#define _sh1106_h

#include <Arduino.h>

#define SH1106_ADDR1 0x3C // write data
#define SH1106_ADDR2 0x3D // read data
#define SH1106_COMMAND 0x00
#define SH1106_DATA 0x40

/*
 * Commands for the display
 */
#define SET_PAGE_ADDRESS                0xB0 /* sets the page address from 0 to 7 */
#define DISPLAY_OFF                     0xAE
#define DISPLAY_ON                      0xAF
#define SET_MEMORY_ADDRESSING_MODE      0x20
#define SET_COM_OUTPUT_SCAN_DIRECTION   0xC8
#define LOW_COLUMN_ADDRESS              0x00
#define HIGH_COLUMN_ADDRESS             0x10
#define START_LINE_ADDRESS              0x40
#define SET_CONTRAST_CTRL_REG           0x81
#define SET_SEGMENT_REMAP               0xA1 // 0 to 127
#define SET_NORMAL_DISPLAY              0xA6
#define SET_MULTIPLEX_RATIO             0xA8
#define OUTPUT_FOLLOWS_RAM              0xA4
#define OUTPUT_IGNORES_RAM              0xA5

#define SET_DISPLAY_OFFSET              0xD3
#define SET_DISPLAY_CLOCK_DIVIDE        0xD5
#define SET_PRE_CHARGE_PERIOD           0xD9
#define SET_COM_PINS_HARDWARE_CONFIG    0xDA
#define SET_VCOMH                       0xDB
#define SET_DC_DC_ENABLE                0x8D


/*
 * Screen related constants
 */
#define MAX_PAGE_COUNT  8
#define SCREEN_WIDTH    128  // document said 132 however testing puts it at 128
#define SCREEN_HEIGHT   64

typedef enum
{
    StartSend = 0,
    MidSend,
    FinishSend,
    Complete
} SendState;

class sh1106_lcd
{
public:
    static sh1106_lcd *getInstance();

public:
    void ClearScreen();
    void FillScreen(byte fillData);
    void DrawPixel(byte x, byte y, bool on);
    void DrawRectangle(byte x1, byte y1, byte x2, byte y2);
    void DrawRectangle(byte x1, byte y1, byte x2, byte y2, byte thickness);
    void FillRectangle(byte x1, byte y1, byte x2, byte y2);
    void DrawLine(byte x1, byte y1, byte x2, byte y2);
    void Print(char *data);
    void PrintLine(char *data);
    void Show();
    
private:
    void Initialize();
    byte SendByte(byte data, SendState state);
    byte SendCommand(byte command, SendState state = Complete);
    byte SendData(byte data, SendState state = Complete);
    void PrintData(char *data, bool incrementLine);
    
private:
    static sh1106_lcd *m_pInstance;
    byte m_currentLine;
    byte m_cursor;
    byte m_screen[MAX_PAGE_COUNT][SCREEN_WIDTH]; // 1024 bytes 8 pages of 128 bits
};

#endif
