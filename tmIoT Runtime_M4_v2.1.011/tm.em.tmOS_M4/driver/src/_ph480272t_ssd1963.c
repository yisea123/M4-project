//*****************************************************************************
//
// Display driver for the Powertip PH480272T-006-106Q.
//
// Copyright (c) 2010-2015 Ningbo TECHMATION Software Co.,Ltd.
// All rights reserved. Software License Agreement
//
// SSD1963 Features
//
// Display feature
//  - Built-in 1215K bytes frame buffer. Support up to 864 x 480 at 24bpp display
//  - Support TFT 18/24-bit generic RGB interface panel
//  - Support 8-bit serial RGB interface
//  - Hardware rotation of 0, 90, 180, 270 degree
//  - Hardware display mirroring
//  - Hardware windowing
//  - Programmable brightness, contrast and saturation control
//  - Dynamic Backlight Control (DBC) via PWM signal
//
// MCU connectivity
//  - 8/9/16/18/24-bit MCU interface
//  - Tearing effect signal
//
// I/O Connectivity
//  - 4 GPIO pins
//
// Built-in clock generator
// Deep sleep mode for power saving
// Core supply power (V DDPLL and V DDD ): 1.2V±0.1V
// I/O supply power(V DDIO ): 1.65V to 3.6V
// LCD interface supply power (V DDLCD ): 1.65V to 3.6V
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/rom.h"
#include "driverlib/lcd.h"
#include "_ph480272t_ssd1963.h"
//*****************************************************************************
//
// This driver operates in four different screen orientations.  They are:
//
// * Portrait - The screen is taller than it is wide, and the flex connector is
//              on the left of the display.  This is selected by defining
//              PORTRAIT.
//
// * Landscape - The screen is wider than it is tall, and the flex connector is
//               on the bottom of the display.  This is selected by defining
//               LANDSCAPE.
//
// * Portrait flip - The screen is taller than it is wide, and the flex
//                   connector is on the right of the display.  This is
//                   selected by defining PORTRAIT_FLIP.
//
// * Landscape flip - The screen is wider than it is tall, and the flex
//                    connector is on the top of the display.  This is
//                    selected by defining LANDSCAPE_FLIP.
//
// These can also be imagined in terms of screen rotation; if portrait mode is
// 0 degrees of screen rotation, landscape is 90 degrees of counter-clockwise
// rotation, portrait flip is 180 degrees of rotation, and landscape flip is
// 270 degress of counter-clockwise rotation.
//
// If no screen orientation is selected, landscape mode will be used.
//
//*****************************************************************************
#if ! defined(PORTRAIT) && ! defined(PORTRAIT_FLIP) &&                        \
    ! defined(LANDSCAPE) && ! defined(LANDSCAPE_FLIP)
#define LANDSCAPE
#endif

//*****************************************************************************
//
// Various definitions controlling coordinate space mapping and drawing
// direction in the four supported orientations.
//
//*****************************************************************************
#ifdef PORTRAIT
// #define HORIZ_DIRECTION         0x28
// #define VERT_DIRECTION          0x20
// #define MAPPED_X(x, y)          (479 - (y))
// #define MAPPED_Y(x, y)          (x)
#endif
#ifdef LANDSCAPE
#define HORIZ_DIRECTION         0x00
#define VERT_DIRECTION          0x00
#define MAPPED_X(x, y)          (x)
#define MAPPED_Y(x, y)          (y)
#endif
#ifdef PORTRAIT_FLIP
// #define HORIZ_DIRECTION         0x18
// #define VERT_DIRECTION          0x10
// #define MAPPED_X(x, y)          (y)
// #define MAPPED_Y(x, y)          (271 - (x))
#endif
#ifdef LANDSCAPE_FLIP
#define HORIZ_DIRECTION         0x03
#define VERT_DIRECTION          0x03
#define MAPPED_X(x, y)          (479 - (x))
#define MAPPED_Y(x, y)          (271 - (y))
#endif

void 		PH480272T_ssd1963Init(uint32_t ui32SysClock);
ERROR_T		_PH480272T_ssd1963_Open     (   void      *pArg   );

DEV_ENGINE_T            g_PH480272T_ssd1963_Engine  =
{
	_PH480272T_ssd1963_Open,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};
//*****************************************************************************
//
// SSD1963 Command Table
//
//*****************************************************************************
#define nop                         0x00
#define soft_reset                  0x01
#define get_power_mode              0x0A
#define get_address_mode            0x0B
#define get_pixel_format            0x0C
#define get_display_mode            0x0D
#define get_signal_mode             0x0E
#define enter_sleep_mode            0x10
#define exit_sleep_mode             0x11
#define enter_partial_mode          0x12
#define enter_normal_mode           0x13
#define exit_invert_mode            0x20
#define enter_invert_mode           0x21
#define set_gamma_curve             0x26
#define set_display_off             0x28
#define set_display_on              0x29
#define set_column_address          0x2A
#define set_page_address            0x2B
#define write_memory_start          0x2C
#define read_memory_start           0x2E
#define set_partial_area            0x30
#define set_scroll_area             0x33
#define set_tear_off                0x34
#define set_tear_on                 0x35
#define set_address_mode            0x36
#define set_scroll_start            0x37
#define exit_idle_mode              0x38
#define enter_idle_mode             0x39
#define set_pixel_format            0x3A
#define write_memory_continue       0x3C
#define read_memory_continue        0x3E
#define set_tear_scanline           0x44
#define get_scanline                0x45
#define read_ddb                    0xA1
#define set_lcd_mode_               0xB0
#define get_lcd_mode                0xB1
#define set_hori_period             0xB4
#define get_hori_period             0xB5
#define set_vert_period             0xB6
#define get_vert_period             0xB7
#define set_gpio_conf               0xB8
#define get_gpio_conf               0xB9
#define set_gpio_value              0xBA
#define get_gpio_status             0xBB
#define set_post_proc               0xBC
#define get_post_proc               0xBD
#define set_pwm_conf                0xBE
#define get_pwm_conf                0xBF
#define set_lcd_gen0                0xC0
#define get_lcd_gen0                0xC1
#define set_lcd_gen1                0xC2
#define get_lcd_gen1                0xC3
#define set_lcd_gen2                0xC4
#define get_lcd_gen2                0xC5
#define set_lcd_gen3                0xC6
#define get_lcd_gen3                0xC7
#define set_gpio0_rop               0xC8
#define get_gpio0_rop               0xC9
#define set_gpio1_rop               0xCA
#define get_gpio1_rop               0xCB
#define set_gpio2_rop               0xCC
#define get_gpio2_rop               0xCD
#define set_gpio3_rop               0xCE
#define get_gpio3_rop               0xCF
#define set_dbc_conf                0xD0
#define get_dbc_conf                0xD1
#define set_dbc_th                  0xD4
#define get_dbc_th                  0xD5
#define set_pll                     0xE0
#define set_pll_mn                  0xE2
#define get_pll_mn                  0xE3
#define get_pll_status              0xE4
#define set_deep_sleep              0xE5
#define set_lshift_freq             0xE6
#define get_lshift_freq             0xE7
#define set_pixel_data_interface    0xF0
#define get_pixel_data_interface    0xF1


ERROR_T		_PH480272T_ssd1963_Open     (   void      *pArg   )
{
	uint32_t *ui32SysClock = pArg;

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOR);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOS);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOT);
    
    ROM_GPIOPinConfigure(GPIO_PJ6_LCDAC);
    ROM_GPIOPinConfigure(GPIO_PR0_LCDCP);
    ROM_GPIOPinConfigure(GPIO_PR1_LCDFP);
    ROM_GPIOPinConfigure(GPIO_PR2_LCDLP);
    ROM_GPIOPinConfigure(GPIO_PR4_LCDDATA00);
    ROM_GPIOPinConfigure(GPIO_PR5_LCDDATA01);
    ROM_GPIOPinConfigure(GPIO_PF7_LCDDATA02);
    ROM_GPIOPinConfigure(GPIO_PR3_LCDDATA03);
    ROM_GPIOPinConfigure(GPIO_PR6_LCDDATA04);
    ROM_GPIOPinConfigure(GPIO_PR7_LCDDATA05);
    ROM_GPIOPinConfigure(GPIO_PS4_LCDDATA06);
    ROM_GPIOPinConfigure(GPIO_PS5_LCDDATA07);
    ROM_GPIOPinConfigure(GPIO_PS6_LCDDATA08);
    ROM_GPIOPinConfigure(GPIO_PS7_LCDDATA09);
    ROM_GPIOPinConfigure(GPIO_PT0_LCDDATA10);
    ROM_GPIOPinConfigure(GPIO_PT1_LCDDATA11);
    ROM_GPIOPinConfigure(GPIO_PN7_LCDDATA12);
    ROM_GPIOPinConfigure(GPIO_PN6_LCDDATA13);
    ROM_GPIOPinConfigure(GPIO_PJ2_LCDDATA14);
    ROM_GPIOPinConfigure(GPIO_PJ3_LCDDATA15);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_6);
    // ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_6, GPIO_PIN_6);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTT_BASE, GPIO_PIN_2);
    ROM_GPIOPinWrite(GPIO_PORTT_BASE, GPIO_PIN_2, GPIO_PIN_2);

    GPIOPinTypeLCD(GPIO_PORTF_BASE, GPIO_PIN_7);
    GPIOPinTypeLCD(GPIO_PORTJ_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6);
    GPIOPinTypeLCD(GPIO_PORTR_BASE,
                       (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                        GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7));
    GPIOPinTypeLCD(GPIO_PORTN_BASE, GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinTypeLCD(GPIO_PORTS_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinTypeLCD(GPIO_PORTT_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	PH480272T_ssd1963Init(*ui32SysClock);
    return  ERROR_NONE;
}
//*****************************************************************************
//
// Translates a 24-bit RGB color to a display driver-specific color.
//
// \param c is the 24-bit RGB color.  The least-significant byte is the blue
// channel, the next byte is the green channel, and the third byte is the red
// channel.
//
// This macro translates a 24-bit RGB color into a value that can be written
// into the display's frame buffer in order to reproduce that color, or the
// closest possible approximation of that color.
//
// \return Returns the display-driver specific color.
//
//*****************************************************************************
#define DPYCOLORTRANSLATE(c)    ((((c) & 0x00f80000) >> 8) |                  \
                                 (((c) & 0x0000fc00) >> 5) |                  \
                                 (((c) & 0x000000f8) >> 3))

//*****************************************************************************
//
// Writes a data byte to the SSD1963.
//
//*****************************************************************************
static inline void WriteData8(uint8_t ui8Data)
{
    //
    // Split the write into two bytes and pass them to the LCD controller.
    //
    LCDIDDDataWrite(LCD0_BASE, 0, ui8Data & 0xff);
}


//*****************************************************************************
//
// Writes a data word to the SSD1963.
//
//*****************************************************************************
static inline void WriteData(uint16_t ui16Data)
{
    //
    // Split the write into two bytes and pass them to the LCD controller.
    //
    LCDIDDDataWrite(LCD0_BASE, 0, ui16Data);
}

//*****************************************************************************
//
// Writes a command to the SSD1963.
//
//*****************************************************************************
static inline void WriteCommand(uint8_t ui8Data)
{
    //
    // Pass the write on to the controller.
    //
    LCDIDDCommandWrite(LCD0_BASE, 0, (uint16_t)ui8Data);
}

//*****************************************************************************
//
//! Draws a pixel on the screen.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param i32X is the X coordinate of the pixel.
//! \param i32Y is the Y coordinate of the pixel.
//! \param ui32Value is the color of the pixel.
//!
//! This function sets the given pixel to a particular color.  The coordinates
//! of the pixel are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void PH480272T_ssd1963PixelDraw(void *pvDisplayData, int32_t i32X,
                                  int32_t i32Y, uint32_t ui32Value)
{
    //
    // Set the starting X address of the display cursor.
    //
    WriteCommand(set_column_address);
    WriteData8(MAPPED_X(i32X, i32Y)>>8);
    WriteData8(MAPPED_X(i32X, i32Y));
    WriteData8(MAPPED_X(i32X, i32Y)>>8);
    WriteData8(MAPPED_X(i32X, i32Y));

    //
    // Set the starting Y address of the display cursor.
    //
    WriteCommand(set_page_address);
    WriteData8(MAPPED_Y(i32X, i32Y)>>8);
    WriteData8(MAPPED_Y(i32X, i32Y));
    WriteData8(MAPPED_Y(i32X, i32Y)>>8);
    WriteData8(MAPPED_Y(i32X, i32Y));

    WriteCommand(write_memory_start);
    WriteData((unsigned short)ui32Value);
}

//*****************************************************************************
//
//! Draws a horizontal sequence of pixels on the screen.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param i32X is the X coordinate of the first pixel.
//! \param i32Y is the Y coordinate of the first pixel.
//! \param i32X0 is sub-pixel offset within the pixel data, which is valid for
//! 1 or 4 bit per pixel formats.
//! \param i32Count is the number of pixels to draw.
//! \param i32BPP is the number of bits per pixel; must be 1, 4, or 8.
//! \param pui8Data is a pointer to the pixel data.  For 1 and 4 bit per pixel
//! formats, the most significant bit(s) represent the left-most pixel.
//! \param pui8Palette is a pointer to the palette used to draw the pixels.
//!
//! This function draws a horizontal sequence of pixels on the screen, using
//! the supplied palette.  For 1 bit per pixel format, the palette contains
//! pre-translated colors; for 4 and 8 bit per pixel formats, the palette
//! contains 24-bit RGB values that must be translated before being written to
//! the display.
//!
//! \return None.
//
//*****************************************************************************
static void PH480272T_ssd1963PixelDrawMultiple(void *pvDisplayData, int32_t i32X,
                                          int32_t i32Y, int32_t i32X0,
                                          int32_t i32Count, int32_t i32BPP,
                                          const uint8_t *pui8Data,
                                          const uint8_t *pui8Palette)
{
    uint32_t ulByte;
    uint32_t uiValue;

    //
    // Set the cursor increment to left to right, followed by top to bottom.
    //
    WriteCommand(set_address_mode);
    WriteData(HORIZ_DIRECTION);

    //
    // Set the starting X address of the display cursor.
    //
    WriteCommand(set_column_address);
    WriteData8(MAPPED_X(i32X, i32Y)>>8);
    WriteData8(MAPPED_X(i32X, i32Y));
    WriteData8(MAPPED_X(i32X+i32Count, i32Y)>>8);
    WriteData8(MAPPED_X(i32X+i32Count, i32Y));

    //
    // Set the Y address of the display cursor.
    //
    WriteCommand(set_page_address);
    WriteData8(MAPPED_Y(i32X, i32Y)>>8);
    WriteData8(MAPPED_Y(i32X, i32Y));
    WriteData8(MAPPED_Y(i32X+i32Count, i32Y)>>8);
    WriteData8(MAPPED_Y(i32X+i32Count, i32Y));

    //
    // Write the data RAM write command.
    //
    WriteCommand(write_memory_start);

    //
    // Determine how to interpret the pixel data based on the number of bits
    // per pixel.
    //
    switch(i32BPP)
    {
        //
        // The pixel data is in 1 bit per pixel format.
        //
        case 1:
        {
            //
            // Loop while there are more pixels to draw.
            //
            while(i32Count)
            {
                //
                // Get the next byte of image data.
                //
                ulByte = *pui8Data++;

                //
                // Loop through the pixels in this byte of image data.
                //
                for(; (i32X0 < 8) && i32Count; i32X0++, i32Count--)
                {
                    //
                    // Draw this pixel in the appropriate color.
                    //
                    uiValue = ((uint32_t *)pui8Palette) [(ulByte >>(7-i32X0))&1];

                    WriteData((unsigned short)uiValue);
                }

                //
                // Start at the beginning of the next byte of image data.
                //
                i32X0 = 0;
            }

            //
            // The image data has been drawn.
            //
            break;
        }

        //
        // The pixel data is in 4 bit per pixel format.
        //
        case 4:
        {
            //
            // Loop while there are more pixels to draw.  "Duff's device" is
            // used to jump into the middle of the loop if the first nibble of
            // the pixel data should not be used.  Duff's device makes use of
            // the fact that a case statement is legal anywhere within a
            // sub-block of a switch statement.  See
            // http://en.wikipedia.org/wiki/Duff's_device for detailed
            // information about Duff's device.
            //
            switch(i32X0 & 1)
            {
                case 0:
                    while(i32Count)
                    {
                        //
                        // Get the upper nibble of the next byte of pixel data
                        // and extract the corresponding entry from the
                        // palette.
                        //
                        ulByte = (*pui8Data >> 4) * 3;
                        ulByte = (*(uint32_t *)(pui8Palette + ulByte) &
                                  0x00ffffff);

                        //
                        // Translate this palette entry and write it to the
                        // screen.
                        //
                        uiValue = DPYCOLORTRANSLATE(ulByte);

                        WriteData((unsigned short)uiValue);

                        //
                        // Decrement the count of pixels to draw.
                        //
                        i32Count--;

                        //
                        // See if there is another pixel to draw.
                        //
                        if(i32Count)
                        {
                case 1:
                            //
                            // Get the lower nibble of the next byte of pixel
                            // data and extract the corresponding entry from
                            // the palette.
                            //
                            ulByte = (*pui8Data++ & 15) * 3;
                            ulByte = (*(uint32_t *)(pui8Palette + ulByte) &
                                      0x00ffffff);

                            //
                            // Translate this palette entry and write it to the
                            // screen.
                            //
                            uiValue = DPYCOLORTRANSLATE(ulByte);

                            WriteData((unsigned short)uiValue);

                            //
                            // Decrement the count of pixels to draw.
                            //
                            i32Count--;
                        }
                    }
            }

            //
            // The image data has been drawn.
            //
            break;
        }

        //
        // The pixel data is in 8 bit per pixel format.
        //
        case 8:
        {
            //
            // Loop while there are more pixels to draw.
            //
            while(i32Count--)
            {
                //
                // Get the next byte of pixel data and extract the
                // corresponding entry from the palette.
                //
                ulByte = *pui8Data++ * 3;
                ulByte = *(uint32_t *)(pui8Palette + ulByte) & 0x00ffffff;

                //
                // Translate this palette entry and write it to the screen.
                //
                uiValue = DPYCOLORTRANSLATE(ulByte);

                WriteData((unsigned short)uiValue);
            }

            //
            // The image data has been drawn.
            //
            break;
        }

        //
        // We are being passed data in the display's native format.  Merely
        // write it directly to the display.  This is a special case which is
        // not used by the graphics library but which is helpful to
        // applications which may want to handle, for example, JPEG images.
        //
        case 16:
        {
            unsigned short usByte;

            //
            // Loop while there are more pixels to draw.
            //
            while(i32Count--)
            {
                //
                // Get the next byte of pixel data and extract the
                // corresponding entry from the palette.
                //
                usByte = *((unsigned short *)pui8Data);
                pui8Data += 2;

                //
                // Translate this palette entry and write it to the screen.
                //
                uiValue = usByte;

                WriteData((unsigned short)uiValue);
            }
        }
    }
}

//*****************************************************************************
//
//! Draws a horizontal line.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param i32X1 is the X coordinate of the start of the line.
//! \param i32X2 is the X coordinate of the end of the line.
//! \param i32Y is the Y coordinate of the line.
//! \param ui32Value is the color of the line.
//!
//! This function draws a horizontal line on the display.  The coordinates of
//! the line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void PH480272T_ssd1963LineDrawH(void *pvDisplayData, int32_t i32X1,
                                  int32_t i32X2, int32_t i32Y,
                                  uint32_t ui32Value)
{
    //
    // Set the cursor increment to left to right, followed by top to bottom.
    //
    WriteCommand(set_address_mode); // SET Address mode
    WriteData(HORIZ_DIRECTION);

    //
    // Set the starting X address of the display cursor.
    //
    WriteCommand(set_column_address);
    WriteData8(MAPPED_X(i32X1, i32Y)>>8);
    WriteData8(MAPPED_X(i32X1, i32Y));
    WriteData8(MAPPED_X(i32X2, i32Y)>>8);
    WriteData8(MAPPED_X(i32X2, i32Y));

    //
    // Set the Y address of the display cursor.
    //
    WriteCommand(set_page_address);
    WriteData8(MAPPED_Y(i32X1, i32Y)>>8);
    WriteData8(MAPPED_Y(i32X1, i32Y));
    WriteData8(MAPPED_Y(i32X2, i32Y)>>8);
    WriteData8(MAPPED_Y(i32X2, i32Y));

    //
    // Write the data RAM write command.
    //
    WriteCommand(write_memory_start);

    //
    // Loop through the pixels of this horizontal line.
    //
    while(i32X1++ <= i32X2)
    {
        //
        // Write the pixel value.
        //
        WriteData((unsigned short)ui32Value);
    }
}

//*****************************************************************************
//
//! Draws a vertical line.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param i32X is the X coordinate of the line.
//! \param i32Y1 is the Y coordinate of the start of the line.
//! \param i32Y2 is the Y coordinate of the end of the line.
//! \param ui32Value is the color of the line.
//!
//! This function draws a vertical line on the display.  The coordinates of the
//! line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void PH480272T_ssd1963LineDrawV(void *pvDisplayData, int32_t i32X,
                                  int32_t i32Y1, int32_t i32Y2,
                                  uint32_t ui32Value)
{
    //
    // Set the cursor increment to top to bottom, followed by left to right.
    //
    WriteCommand(set_address_mode); // SET Address mode
    WriteData(VERT_DIRECTION);

    //
    // Set the X address of the display cursor.
    //
    WriteCommand(set_column_address);
    WriteData8(MAPPED_X(i32X, i32Y1)>>8);
    WriteData8(MAPPED_X(i32X, i32Y1));
    WriteData8(MAPPED_X(i32X, i32Y2)>>8);
    WriteData8(MAPPED_X(i32X, i32Y2));

    //
    // Set the starting Y address of the display cursor.
    //
    WriteCommand(set_page_address);
    WriteData8(MAPPED_Y(i32X, i32Y1)>>8);
    WriteData8(MAPPED_Y(i32X, i32Y1));
    WriteData8(MAPPED_Y(i32X, i32Y2)>>8);
    WriteData8(MAPPED_Y(i32X, i32Y2));

    //
    // Write the data RAM write command.
    //
    WriteCommand(write_memory_start);

    //
    // Loop through the pixels of this vertical line.
    //
    while(i32Y1++ <= i32Y2)
    {
        //
        // Write the pixel value.
        //
        WriteData((unsigned short)ui32Value);
    }
}

//*****************************************************************************
//
//! Fills a rectangle.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param psRect is a pointer to the structure describing the rectangle.
//! \param ui32Value is the color of the rectangle.
//!
//! This function fills a rectangle on the display.  The coordinates of the
//! rectangle are assumed to be within the extents of the display, and the
//! rectangle specification is fully inclusive (in other words, both i16XMin
//! and i16XMax are drawn, along with i16YMin and i16YMax).
//!
//! \return None.
//
//*****************************************************************************
static void PH480272T_ssd1963RectFill(void *pvDisplayData, const tRectangle *psRect,
                                 uint32_t ui32Value)
{
    int32_t lCount;

    WriteCommand(set_address_mode); // SET Address mode
    WriteData(HORIZ_DIRECTION);

    //
    // Write the X extents of the rectangle.
    //
    WriteCommand(set_column_address);
    WriteData8(MAPPED_X(psRect->i16XMin, psRect->i16YMin)>>8);
    WriteData8(MAPPED_X(psRect->i16XMin, psRect->i16YMin));
    WriteData8(MAPPED_X(psRect->i16XMax, psRect->i16YMax)>>8);
    WriteData8(MAPPED_X(psRect->i16XMax, psRect->i16YMax));

    //
    // Write the Y extents of the rectangle
    //
    WriteCommand(set_page_address);
    WriteData8(MAPPED_Y(psRect->i16XMin, psRect->i16YMin)>>8);
    WriteData8(MAPPED_Y(psRect->i16XMin, psRect->i16YMin));
    WriteData8(MAPPED_Y(psRect->i16XMax, psRect->i16YMax)>>8);
    WriteData8(MAPPED_Y(psRect->i16XMax, psRect->i16YMax));

    //
    // Tell the controller we are about to write data into its RAM.
    //
    WriteCommand(write_memory_start);

    //
    // Loop through the pixels of this filled rectangle.
    //
    for(lCount = ((psRect->i16XMax - psRect->i16XMin + 1) *
                  (psRect->i16YMax - psRect->i16YMin + 1)); lCount >= 0; lCount--)
    {
        //
        // Write the pixel value.
        //
        WriteData((unsigned short)ui32Value);
    }
}

//*****************************************************************************
//
//! Translates a 24-bit RGB color to a display driver-specific color.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param ui32Value is the 24-bit RGB color.  The least-significant byte is
//! the blue channel, the next byte is the green channel, and the third byte is
//! the red channel.
//!
//! This function translates a 24-bit RGB color into a value that can be
//! written into the display's frame buffer in order to reproduce that color,
//! or the closest possible approximation of that color.
//!
//! \return Returns the display-driver specific color.
//
//*****************************************************************************
static uint32_t PH480272T_ssd1963ColorTranslate(void *pvDisplayData, uint32_t ui32Value)
{
    //
    // Translate from a 24-bit RGB color to a 5-6-5 RGB color.
    //
    return(DPYCOLORTRANSLATE(ui32Value));
}

//*****************************************************************************
//
//! Flushes any cached drawing operations.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//!
//! This functions flushes any cached drawing operations to the display.  This
//! is useful when a local frame buffer is used for drawing operations, and the
//! flush would copy the local frame buffer to the display.  For the SSD1963
//! driver, the flush is a no operation.
//!
//! \return None.
//
//*****************************************************************************
static void PH480272T_ssd1963Flush(void *pvDisplayData)
{
    //
    // There is nothing to be done.
    //
}


//*****************************************************************************
//
//! The display structure that describes the driver for the Kentec K350QVG-V2-F
//! TFT panel with an SSD1963 controller.
//
//*****************************************************************************
const tDisplay g_sPH480272T_ssd1963 =
{
    sizeof(tDisplay),
    0,
#if defined(PORTRAIT) || defined(PORTRAIT_FLIP)
    272,
    480,
#else
    480,
    272,
#endif
    PH480272T_ssd1963PixelDraw,
    PH480272T_ssd1963PixelDrawMultiple,
    PH480272T_ssd1963LineDrawH,
    PH480272T_ssd1963LineDrawV,
    PH480272T_ssd1963RectFill,
    PH480272T_ssd1963ColorTranslate,
    PH480272T_ssd1963Flush
};

//*****************************************************************************
//
//! Initializes the display driver.
//!
//! \param ui32SysClock is the frequency of the system clock.
//!
//! This function initializes the LCD controller and the SSD1963 display
//! controller on the panel, preparing it to display data.
//!
//! \return None.
//
//*****************************************************************************
/*
** According PROGRAMMING EXAMPLE from doc/SSD1961_2_3_Application_note_v1.7.pdf
*/
void PH480272T_ssd1963Init(uint32_t ui32SysClock)
{
    uint32_t ui32ClockMS;
    tLCDIDDTiming sTimings;
    int i;

    //
    // Determine the number of system clock cycles in 1mS
    //
    ui32ClockMS = CYCLES_FROM_TIME_US(ui32SysClock, 1000);

    //
    // Divide by 3 to get the number of SysCtlDelay loops in 1mS.
    //
    ui32ClockMS /= 3;

    //
    // Enable the LCD controller.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_LCD0);

    //
    // Assert the LCD reset signal.
    //
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_6, 0);

    //
    // Delay for 50ms.
    //
    SysCtlDelay(50 * ui32ClockMS);

    //
    // Deassert the LCD reset signal.
    //
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_6, GPIO_PIN_6);

    //
    // Delay for 50ms while the LCD comes out of reset.
    //
    SysCtlDelay(50 * ui32ClockMS);

    //
    // Configure the LCD controller for LIDD-mode operation.
    //
    LCDModeSet(LCD0_BASE, LCD_MODE_LIDD, ui32SysClock, ui32SysClock);

    //
    // Configure DMA-related parameters.
    //
    LCDDMAConfigSet(LCD0_BASE, LCD_DMA_BURST_4);

    //
    // Set control signal parameters and polarities.
    //
    LCDIDDConfigSet(LCD0_BASE, LIDD_CONFIG_SYNC_MPU80);

    //
    // Reference to 8080 Mode Timing
    //
    sTimings.ui8WSSetup = CYCLES_FROM_TIME_NS(ui32SysClock, 5);
    sTimings.ui8WSDuration = CYCLES_FROM_TIME_NS(ui32SysClock, 15);
    sTimings.ui8WSHold = CYCLES_FROM_TIME_NS(ui32SysClock, 2);
    sTimings.ui8RSSetup = CYCLES_FROM_TIME_NS(ui32SysClock, 5);
    sTimings.ui8RSDuration = CYCLES_FROM_TIME_NS(ui32SysClock, 40);
    sTimings.ui8RSHold = CYCLES_FROM_TIME_NS(ui32SysClock, 2);
    sTimings.ui8DelayCycles = CYCLES_FROM_TIME_NS(ui32SysClock, 10);
    LCDIDDTimingSet(LCD0_BASE, 0, &sTimings);

    // LCD Init
    WriteCommand(set_pll_mn);
    WriteData8(0x1D);
    WriteData8(0x02);
    WriteData8(0x54);

    // PLL enable
    WriteCommand(set_pll);
    WriteData8(0x01);

    SysCtlDelay(ui32ClockMS);

    WriteCommand(set_pll);
    WriteData8(0x03);

    SysCtlDelay(10 * ui32ClockMS);

    // Software reset
    WriteCommand(soft_reset);

    SysCtlDelay(5 * ui32ClockMS);

    WriteCommand(set_lshift_freq);
    WriteData8(0x03);
    WriteData8(0x85);
    WriteData8(0x1D);

    WriteCommand(set_lcd_mode_);     //LCD SPECIFICATION
    WriteData8(0x28);      // 24-bit, FRC enable
    WriteData8(0x00);      // TFT mode
    WriteData8(0x01);      // horizontal size=480-1 HightByte
    WriteData8(0xDF);      // horizontal size=480-1 LowByte
    WriteData8(0x01);      // vertical size=272-1 HightByte
    WriteData8(0x0F);      // vertical size=272-1 LowByte
    WriteData8(0x00);      // Panel with odd RGB & even RGB sequence

    WriteCommand(set_hori_period);   //HSYNC
    WriteData8(0x02);
    WriteData8(0x0C);      // total 1716 clocks
    WriteData8(0x00);
    WriteData8(0x2A);      // non-display period = 244 clocks (HPS)
    WriteData8(0x28);      // horizontal sync pulse width = 128 clocks(HPW)(0x2A+1)*3=128
    WriteData8(0x00);
    WriteData8(0x00);
    WriteData8(0x00);

    WriteCommand(set_vert_period);   //VSYNC
    WriteData8(0x01);
    WriteData8(0x1D);      // total Lline=255
    WriteData8(0x00);
    WriteData8(0x0B);      // non-display period=4lines
    WriteData8(0x09);      // vertical sync pulse witdth=4lines
    WriteData8(0x00);
    WriteData8(0x00);

    WriteCommand(exit_sleep_mode);

    WriteCommand(set_address_mode);
    WriteData8(HORIZ_DIRECTION);

    WriteCommand(set_pixel_data_interface);
    WriteData8(0x03);        // 16-bit (565 format)

    WriteCommand(set_pwm_conf);      //set PWM for B/L
    WriteData8(0x00);
    WriteData8(0x7F);      // PWM duty cycle (backlight brightness)
    WriteData8(0x01);      // PWM controlled by host

    WriteCommand(set_display_on);     //display on

    WriteCommand(set_column_address);
    WriteData8(0x00);      // SP: 0=0x0000
    WriteData8(0x00);
    WriteData8(0x01);      // EP: 480-1=479=0x01DF
    WriteData8(0xDF);

    WriteCommand(set_page_address);
    WriteData8(0x00);      // SC: 0=0x0000
    WriteData8(0x00);
    WriteData8(0x01);      // EC:272-1=271=0x010F
    WriteData8(0x0F);

    // Clear the contents of the display buffer.
    WriteCommand(write_memory_start);
    for (i = 0; i < (480*272); i++) {
        WriteData(0x0000);
    }
}
