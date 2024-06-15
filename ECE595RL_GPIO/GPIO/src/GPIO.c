/**
 * @file GPIO.c
 * @brief Source code for the GPIO driver.
 *
 * This file contains the function definitions for the GPIO driver.
 * It interfaces with the following:
 *  - User buttons and LEDs of the TI MSP432 LaunchPad
 *  - PMOD SWT (4 Slide Switches)
 *  - PMOD 8LD (8 LEDs)
 *
 * To verify the pinout of the user buttons and LEDs, refer to the MSP432P401R SimpleLink Microcontroller LaunchPad Development Kit User's Guide
 * Link: https://docs.rs-online.com/3934/A700000006811369.pdf
 *
 * For more information regarding the PMODs used in this lab, visit the following links:
 *  - PMOD SWT: https://digilent.com/reference/pmod/pmodswt/reference-manual
 *  - PMOD 8LD: https://digilent.com/reference/pmod/pmod8ld/reference-manual
 *
 * @note The user buttons, located at P1.1 and P1.4, are configured with negative logic
 * as the default setting. When the buttons are pressed, they connect to GND. Refer to the
 * schematic found in the MSP432P401R LaunchPad User's Guide.
 *
 * @author Aaron Nanas, Srushti Wadekar, Arshia P
 *
 */

#include "../inc/GPIO.h"
#include "../inc/Clock.h"

// Constant definitions for the built-in red LED
const uint8_t RED_LED_OFF           =   0x00;
const uint8_t RED_LED_ON            =   0x01;

// Constant definitions for the RGB LED colors
const uint8_t RGB_LED_OFF           =   0x00;
const uint8_t RGB_LED_RED           =   0x01;
const uint8_t RGB_LED_GREEN         =   0x02;
const uint8_t RGB_LED_YELLOW        =   0x03;
const uint8_t RGB_LED_BLUE          =   0x04;
const uint8_t RGB_LED_PINK          =   0x05;
const uint8_t RGB_LED_SKY_BLUE      =   0x06;
const uint8_t RGB_LED_WHITE         =   0x07;

// Constant definitions for the PMOD 8LD module
const uint8_t PMOD_8LD_ALL_OFF      =   0x00;
const uint8_t PMOD_8LD_ALL_ON       =   0xFF;
const uint8_t PMOD_8LD_0_3_ON       =   0x0F;
const uint8_t PMOD_8LD_4_7_ON       =   0xF0;
const uint8_t PMOD_8LD_EVEN_ON      =   0x55;
const uint8_t PMOD_8LD_ODD_ON       =   0xAA;
const uint8_t PMOD_8LD_0_ON         =   0x01;
const uint8_t PMOD_8LD_1_ON         =   0x02;
const uint8_t PMOD_8LD_2_ON         =   0x04;
const uint8_t PMOD_8LD_3_ON         =   0x08;
const uint8_t PMOD_8LD_4_ON         =   0x10;
const uint8_t PMOD_8LD_5_ON         =   0x20;
const uint8_t PMOD_8LD_6_ON         =   0x30;
const uint8_t PMOD_8LD_7_ON         =   0x40;


void LED1_Init()
{
    P1->SEL0 &= ~0x01;
    P1->SEL1 &= ~0x01;
    P1->DIR |= 0x01;
}

void LED1_Output(uint8_t led_value)
{
    P1->OUT = (P1->OUT & 0xFE) | led_value;
}

uint8_t LED1_Status()
{
    uint8_t LED1_Status = P1->OUT & 0x01;
    return LED1_Status;
}

void LED2_Init()
{
    P2->SEL0 &= ~0x07;
    P2->SEL1 &= ~0x07;
    P2->DS |= 0x07;
    P2->DIR |= 0x07;
    P2->OUT &= ~0x07;
}

void LED2_Output(uint8_t led_value)
{
    P2->OUT = (P2->OUT & 0xF8) | led_value;
}

void LED2_Toggle(uint8_t led_value)
{
    P2->OUT ^= led_value;
}

uint8_t LED2_Status()
{
    uint8_t LED2_Status = P2->OUT & 0x07;
    return LED2_Status;
}

void Buttons_Init()
{
    P1->SEL0 &= ~0x12;
    P1->SEL1 &= ~0x12;
    P1->DIR |= ~0x12;
    P1->REN |= 0x12;
    P1->OUT |= 0x12;
}

uint8_t Get_Buttons_Status()
{
    uint8_t button_status = (P1->IN & 0x12);
    return button_status;
}

void PMOD_8LD_Init()
{
    P9->SEL0 &= ~0xFF;
    P9->SEL1 &= ~0xFF;
    P9->DS |= 0xFF;
    P9->DIR |= 0xFF;
    P9->OUT &= ~0xFF;
}

uint8_t PMOD_8LD_Output(uint8_t led_value)
{
    P9->OUT = led_value;
    uint8_t PMOD_8LD_value = P9->OUT;
    return PMOD_8LD_value;
}

void PMOD_SWT_Init()
{
    P10->SEL0 &= ~0xF;
    P10->SEL1 &= ~0xF;
    P10->DIR &= ~0xF;
}

uint8_t Get_PMOD_SWT_Status()
{
    uint8_t switch_status = P10->IN & 0xF;
    return switch_status;
}
/* Function Name: LED_Pattern_1
 * case     input                   LED1               RGB LED                 PMOD 8LD
 *
 * 0        button 1 pressed        ON                 OFF                     LED 0,2,4,6 ON
 *          button 2 not pressed                                               LED 1,3,4,7 OFF
 *
 * 1        button 1 not pressed    OFF                PINK                    LED 0,2,4,6 OFF
 *          button 2 pressed                                                   LED 1,3,5,7 ON
 *
 * 2        button 1 pressed        TOGGLE EVERY 1s    TOGGLE GREEN EVERY 1s   LED 0-7 OFF
 *          button 2 pressed
 *
 * 3        button 1 not pressed    OFF                OFF                     LED 0-7 ON
 *          button 2 not pressed
 */
void LED_Pattern_1(uint8_t button_status)
{
    switch(button_status)
    {
        // Button 1 and Button 2 are pressed
        case 0x00:
        {
            PMOD_8LD_Output(PMOD_8LD_ALL_OFF); //Disable all LEDs on PMOD 8LD
            LED1_Output(RED_LED_ON); //Turn on Red LED
            LED2_Output(RGB_LED_GREEN); //Turn on RGB LED and set color to GREEN
            Clock_Delay1ms(1000); //1s delay
            LED1_Output(RED_LED_OFF); //Turn off Red LED
            LED2_Output(RGB_LED_OFF); //Turn off RGB LED
            Clock_Delay1ms(1000); //1s delay
            break;
        }

        // Button 1 is pressed
        // Button 2 is not pressed
        case 0x10:
        {
            LED1_Output(RED_LED_ON); // Turn on Red LED
            LED2_Output(RGB_LED_OFF); //Turn off RGB LED
            PMOD_8LD_Output(0x55); //Turn on PMOD 8LD LEDs 0,2,4,6
            break;
        }

        // Button 1 is not pressed
        // Button 2 is pressed
        case 0x02:
        {
            LED1_Output(RED_LED_OFF); //Turn off Red LED
            LED2_Output(RGB_LED_PINK); //Turn on RGB LED and set color to PINK
            PMOD_8LD_Output(0xAA); //Turn on PMOD 8LD LEDs 1,3,5,7
            break;
        }

        // Button 1 and Button 2 are not pressed
        case 0x12:
        {
            LED1_Output(RED_LED_OFF); //Turn off Red LED
            LED2_Output(RGB_LED_OFF); //Turn off RGB LED
            PMOD_8LD_Output(PMOD_8LD_ALL_ON); //Enable all LEDs on PMOD 8LD
            break;
        }
    }
}

void LED_Pattern_2()
{
    LED1_Output(RED_LED_ON);
    LED2_Output(RGB_LED_RED);

    for (int led_count = 0; led_count <= 0xFF; led_count++)
    {
        PMOD_8LD_Output(led_count);
        Clock_Delay1ms(100);
        uint8_t switch_status = Get_PMOD_SWT_Status();
        if (switch_status != 0x01)
        {
            break;
        }
    }
}
/* Function Name: LED_Pattern_3
 * case     input                   LED1    RGB LED     PMOD 8LD
 *
 * 0        ONLY SWT2 ENABLED       OFF     BLUE        BINARY DOWN COUNTER, RATE: 100ms
 */
void LED_Pattern_3()
{
    LED1_Output(RED_LED_OFF); //Turn off Red LED
    LED2_Output(RGB_LED_BLUE); //Turn on RGB LED and set color to BLUE
    for (int led_count = 0xFF; led_count >= 0; led_count--) //for loop iteration from 0xFF down to 0
    {
        PMOD_8LD_Output(led_count); //For every bit position in led_count that is 1, corresponding LEDs on PMOD 8LD are turned on
        Clock_Delay1ms(100); //100ms delay
        uint8_t switch_status = Get_PMOD_SWT_Status(); //binary configuration of the PMOD SWT switch positions stored switch_status
        if (switch_status != 0x02) //terminate if switch 2 is not enabled
        {
            break;
        }
    }
}
/* Function Name: LED_Pattern_4
 * case     input                   LED1                RGB LED                 PMOD 8LD
 *
 * 0        ONLY SWT3 ENABLED       TOGGLE EVERY 1s     TOGGLE BLUE EVERY 1s    LED 0-7 TOGGLE EVERY 1s
 */
void LED_Pattern_4()
{
    while(1) //loop forever
    {
        PMOD_8LD_Output(PMOD_8LD_ALL_ON);//Turn on all LEDs on PMOD 8LD
        LED1_Output(RED_LED_ON); //Turn on Red LED
        LED2_Output(RGB_LED_BLUE); //Turn on RGB LED and set color to BLUE
        Clock_Delay1ms(1000); //1s delay
        PMOD_8LD_Output(PMOD_8LD_ALL_OFF); //Turn off all LEDs on PMOD 8LD
        LED1_Output(RED_LED_OFF); //Turn off Red LED
        LED2_Output(RGB_LED_OFF); //Turn off RGB LED
        Clock_Delay1ms(1000); //1s delay
        uint8_t switch_status = Get_PMOD_SWT_Status(); //binary configuration of the PMOD SWT switch positions stored switch_status
        if (switch_status != 0x04) //terminate if switch 3 is not enabled
                {
                    break;
                }
    }
}
/* Function Name: LED_Pattern_5
 * case     input                   LED1    RGB LED     PMOD 8LD
 *
 * 0        ONLY SWT4 ENABLED       OFF     OFF         LSB LED ON, THEN SHIFT LEFT EVERY 500ms AND REPEAT
 */
void LED_Pattern_5()
{
        LED1_Output(RED_LED_OFF); //Turn off Red LED
        LED2_Output(RGB_LED_OFF); //Turn off RGB LED
        for (int led_count = 0x01; led_count <= 128; led_count = led_count * 2) //for loop iteration starting at 0x01 and multiplying by 2 until 128
        {
            PMOD_8LD_Output(led_count); //For every bit position in led_count that is 1, corresponding LEDs on PMOD 8LD are turned on
            Clock_Delay1ms(500); //500ms delay
            uint8_t switch_status = Get_PMOD_SWT_Status(); //binary configuration of the PMOD SWT switch positions stored switch_status
            if (switch_status != 0x08) //terminate if switch 4 is not enabled
            {
                break;
            }
        }
}

void LED_Controller(uint8_t button_status, uint8_t switch_status)
{
    switch(switch_status)
    {
        case 0x00:
        {
            LED_Pattern_1(button_status);
        }
        break;

        case 0x01:
        {
            LED_Pattern_2();
        }
        break;

        case 0x02:
        {
            LED_Pattern_3();
        }
        break;

        case 0x04:
        {
            LED_Pattern_4();
        }
        break;

        case 0x08:
        {
            LED_Pattern_5();
        }
        break;

        default:
        {
            LED_Pattern_1(button_status);
        }
    }
}
