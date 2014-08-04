//
//  sh1106.cpp
//  
//
//  Created by Daniel Milligan on 8/3/14.
//
//
#include <stdio.h>

// Pull in the wire library to communicate with the LCD over I2C
#include <Wire.h>

#include "sh1106.h"

typedef enum
{
    HORIZONTAL = 0,
    VERTICAL,
    PAGE,
    INVALID,
    END_MEMORY_ADDRESSING_MODES
} MEMORY_ADDRESSING_MODES;

typedef enum
{
    Success = 0,
    DataTooLong,
    NackAddress,
    NackData,
    OtherError,
    END_I2C_STATUS
} I2CStatus;

/* Numbers 0 to 9 each is 8x8 pixels */
#define CHARACTER_WIDTH 8

byte numbers[][CHARACTER_WIDTH]=
{
    {0x00, 0x7E, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7E}, /* 0 */
    {0x00, 0x86, 0x84, 0x82, 0xFF, 0x80, 0x80, 0x80}, /* 1 */
    {0x00, 0xC2, 0xA1, 0x91, 0x89, 0x85, 0x83, 0x83}, /* 2 */
    {0x00, 0x62, 0x81, 0x81, 0x89, 0x89, 0x89, 0x76}, /* 3 */
    {0x00, 0x0C, 0x0A, 0x09, 0xFF, 0x08, 0x08, 0x08}, /* 4 */
    {0x00, 0x4F, 0x89, 0x89, 0x89, 0x89, 0x89, 0x79}, /* 5 */
    {0x00, 0x7F, 0x89, 0x89, 0x89, 0x89, 0x89, 0x71}, /* 6 */
    {0x00, 0x01, 0x01, 0xE1, 0x11, 0x09, 0x05, 0x03}, /* 7 */
    {0x00, 0x76, 0x89, 0x89, 0x89, 0x89, 0x89, 0x76}, /* 8 */
    {0x00, 0x86, 0x89, 0x89, 0x89, 0x89, 0x89, 0x7E}  /* 9 */
};

byte letters[][CHARACTER_WIDTH] =
{
    {0x00, 0xFE, 0x09, 0x09, 0x09, 0x09, 0x09, 0xFE}, /* A */
    {0x00, 0xFF, 0x89, 0x89, 0x89, 0x89, 0x89, 0x76}, /* B */
    {0x00, 0x7E, 0x81, 0x81, 0x81, 0x81, 0x81, 0x42}, /* C */
    {0x00, 0xFF, 0x81, 0x81, 0x81, 0x81, 0x42, 0x3C}, /* D */
    {0x00, 0xFF, 0x89, 0x89, 0x89, 0x89, 0x81, 0x81}, /* E */
    {0x00, 0xFF, 0x09, 0x09, 0x09, 0x09, 0x01, 0x01}, /* F */
    {0x00, 0x7E, 0x81, 0x81, 0x91, 0x91, 0x92, 0x64}, /* G */
    {0x00, 0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0xFF}, /* H */
    {0x00, 0x81, 0x81, 0x81, 0xFF, 0x81, 0x81, 0x81}, /* I */
    {0x00, 0x61, 0x41, 0x81, 0xFF, 0x01, 0x01, 0x01}, /* J */
    {0x00, 0xFF, 0x18, 0x18, 0x14, 0x24, 0x44, 0xC3}, /* K */
    {0x00, 0xFF, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}, /* L */
    {0x00, 0xFF, 0x01, 0x06, 0x80, 0x06, 0x01, 0xFF}, /* M */
    {0x00, 0xFF, 0x02, 0x06, 0x08, 0x30, 0x40, 0xFF}, /* N */
    {0x00, 0x7E, 0x81, 0x81, 0x81, 0x81, 0x81, 0x7E}, /* O */
    {0x00, 0xFF, 0x09, 0x09, 0x09, 0x09, 0x09, 0x06}, /* P */
    {0x00, 0x7E, 0x81, 0x81, 0xA1, 0xA1, 0x41, 0xBE}, /* Q */
    {0x00, 0xFF, 0x09, 0x09, 0x19, 0x29, 0x49, 0xC6}, /* R */
    {0x00, 0x46, 0x89, 0x89, 0x89, 0x89, 0x89, 0x72}, /* S */
    {0x00, 0x01, 0x01, 0x01, 0xFF, 0x01, 0x01, 0x01}, /* T */
    {0x00, 0x7F, 0x80, 0x80, 0x80, 0x80, 0x80, 0x7F}, /* U */
    {0x00, 0x07, 0x38, 0x60, 0x80, 0x60, 0x38, 0x07}, /* V */
    {0x00, 0x7F, 0x80, 0x60, 0xF0, 0x60, 0x80, 0x7F}, /* W */
    {0x00, 0xC3, 0x66, 0x18, 0x08, 0x18, 0x66, 0xC3}, /* X */
    {0x00, 0x03, 0x06, 0x08, 0xF0, 0x08, 0x06, 0x03}, /* Y */
    {0x00, 0x81, 0xC1, 0xA1, 0x91, 0x89, 0x85, 0x83}  /* Z */
};

sh1106_lcd *sh1106_lcd::m_pInstance = NULL;

sh1106_lcd *sh1106_lcd::getInstance()
{
    if (m_pInstance == NULL)
    {
        m_pInstance = (sh1106_lcd *)malloc(sizeof(sh1106_lcd));
        m_pInstance->Initialize();
    }
    return m_pInstance;
}

void sh1106_lcd::Initialize()
{
    Wire.begin(); // begin without address as a master device
    
    SendCommand(DISPLAY_OFF);
    SendCommand(SET_MEMORY_ADDRESSING_MODE);
    SendCommand(PAGE);
    SendCommand(SET_PAGE_ADDRESS); // start at page address 0
    SendCommand(SET_COM_OUTPUT_SCAN_DIRECTION);
    SendCommand(LOW_COLUMN_ADDRESS);
    SendCommand(HIGH_COLUMN_ADDRESS);
    SendCommand(START_LINE_ADDRESS);
    SendCommand(SET_CONTRAST_CTRL_REG);
    SendCommand(0x7F);
    SendCommand(SET_SEGMENT_REMAP);
    SendCommand(SET_NORMAL_DISPLAY);
    SendCommand(SET_MULTIPLEX_RATIO);
    SendCommand(0x3F);
    SendCommand(OUTPUT_FOLLOWS_RAM);
    SendCommand(SET_DISPLAY_OFFSET);
    SendCommand(0x00); // no offset
    SendCommand(SET_DISPLAY_CLOCK_DIVIDE);
    SendCommand(0xF0);
    SendCommand(SET_PRE_CHARGE_PERIOD);
    SendCommand(0x22);
    SendCommand(SET_COM_PINS_HARDWARE_CONFIG);
    SendCommand(0x12);
    SendCommand(SET_VCOMH);
    SendCommand(0x20); // 0.77xVcc
    SendCommand(SET_DC_DC_ENABLE);
    SendCommand(0x14);
    
    SendCommand(DISPLAY_ON);
    
    memset(m_screen, 0, SCREEN_WIDTH * MAX_PAGE_COUNT);
    m_currentLine = 0;
    m_cursor = 2;
}

void sh1106_lcd::Show()
{
    for (int index = 0; index < MAX_PAGE_COUNT; index++)
    {
        SendCommand(SET_PAGE_ADDRESS + index);
        SendCommand(0x00); // low column start address
        SendCommand(0x10); // high column start address
        for (int pixel = 0; pixel < SCREEN_WIDTH; pixel++)
        {
            SendData(m_screen[index][pixel]);
        }
    }
}

void sh1106_lcd::FillScreen(byte fillData)
{
    for (int index = 0; index < MAX_PAGE_COUNT; index++)
    {
        for (int pixel = 0; pixel < SCREEN_WIDTH; pixel++)
        {
            m_screen[index][pixel] = fillData;
        }
    }

    m_currentLine = 0;
    m_cursor = 2;
    
    Show();
}

void sh1106_lcd::ClearScreen()
{
    FillScreen(0x00);
}

/* 
 * x can be from 0 to 131
 * y can be from 0 to 63
 *
 * Show must be called after all pixel's are drawn
 */
void sh1106_lcd::DrawPixel(byte x, byte y, bool on)
{
    byte pageId = y / MAX_PAGE_COUNT;       // convert from byte to page
    byte bitOffset = y % MAX_PAGE_COUNT;    // establish the bit offset
    
    if (pageId < MAX_PAGE_COUNT)
    {
        if (x < SCREEN_WIDTH)
        {
            if (on == true)
            {
                m_screen[pageId][x] |= 1 << bitOffset; // turn this bit on
            }
            else
            {
                m_screen[pageId][x] &= ~(1 << bitOffset); // turn this bit off
            }
        }
    }
}

void sh1106_lcd::Print(char *data)
{
    PrintData(data, false);
    Show();
}

void sh1106_lcd::PrintLine(char *data)
{
    PrintData(data, true);
    Show();
}

void sh1106_lcd::PrintData(char *data, bool incrementLine)
{
    bool eol = false;
    int index = 0;
    int lineIndex = 0;
    
    // time to scroll
    if (m_currentLine == MAX_PAGE_COUNT)
    {
        while (lineIndex < (MAX_PAGE_COUNT - 1))
        {
            // Copy the lines up scrolling off the top
            memcpy(m_screen[lineIndex], m_screen[lineIndex + 1], SCREEN_WIDTH);
            lineIndex++;
        }
        // Line index is now MAX_PAGE_COUNT - 1
        memset(m_screen[lineIndex], 0, SCREEN_WIDTH); // clear this line
    }
    else
    {
        lineIndex = m_currentLine;
    }
    
    while (eol == false)
    {
        if (data[index] != NULL)
        {
            byte *pPtr = NULL;
            byte dataByte = data[index++];

            if (dataByte >= '0' && dataByte <= '9')
            {
                dataByte -= '0';// normalize
                pPtr = (byte *)&numbers[dataByte];
            }
            else if ((dataByte >= 'A') && (dataByte <= 'Z'))
            {
                dataByte -= 'A'; // Normalize
                pPtr = (byte *)&letters[dataByte];
            }
            
            if (pPtr != NULL)
            {
                for (int numberIndex = 0; numberIndex < CHARACTER_WIDTH; numberIndex++)
                {
                    if (m_cursor < SCREEN_WIDTH)
                    {
                        m_screen[lineIndex][m_cursor++] = pPtr[numberIndex];
                    }
                    else
                    {
                        eol = true;
                        break; // get out
                    }
                }
            }
        }
        else
        {
            eol = true; // done
        }
    }
    
    if (incrementLine == true)
    {
        if (m_currentLine < MAX_PAGE_COUNT)
        {
            m_currentLine++;
        }
        m_cursor = 2;
    }
}

byte sh1106_lcd::SendCommand(byte command)
{
    Wire.beginTransmission(SH1106_ADDR1);
    Wire.write(SH1106_COMMAND);
    Wire.write(command);
    
    byte transmissionStatus = Wire.endTransmission();
    
    return transmissionStatus;
}

byte sh1106_lcd::SendData(byte data)
{
    Wire.beginTransmission(SH1106_ADDR1);
    Wire.write(SH1106_DATAA);
    Wire.write(data);
    
    byte transmissionStatus = Wire.endTransmission();
    
    return transmissionStatus;
}
