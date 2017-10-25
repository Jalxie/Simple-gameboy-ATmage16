#ifndef _GAMECONSOLE_H
#define _GAMECONSOLE_H

#include <avr/io.h> 
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define byte unsigned char

/*ON OFF*/
#define ON 0xFF
#define OFF 0x00
#define HIGH 0xFF
#define LOW 0x00
#define IN 0x00
#define OUT 0xFF
#define ALL 0xFF
#define TRUE 1
#define FALSE 0
#define FORWARD 0x00
#define BACK 0xFF
#define INFINITE_LOOP 1
#define LOW_BATTARY 600//1.5/2.56*1024

/*LCD size*/
#define LCDcolumn 102
#define LCDrow 64
#define LCDpage 8

/*SET and GET MACRO*/
#define SET(PORT,MASK,VALUE) PORT = ((MASK & VALUE) | (PORT & ~MASK))
#define GET(PORT,MASK) !(PORT & MASK)        //read the value of mask

/*Define pin addresses on uC*/
//Buttons
#define BUTTON_UP           PA0
#define BUTTON_LEFT         PA2
#define BUTTON_DOWN         PA1
#define BUTTON_RIGHT        PA3
#define BUTTON_ACTIONA      PC7//A
#define BUTTON_ACTIONB      PC6//B
#define BUTTON_SELECT       PC1//Select
//LCD
#define LCD_CS0             PB3
#define LCD_CD              PB2
#define LCD_RST             PD6
#define LCD_MOSI            PB5
#define LCD_MISO            PB6
#define LCD_SCK             PB7
#define LCD_SS              PB4 
#define LCD_BL              PD5 //backlight control pin
#define RAM_HOLD 			PA6
#define RAM_CS 				PA4

//Others
#define LED_BAT_LOW         PD4

/*Devices Direction MACROs*/
#define BAT_LOW_LED_DIR(DIR)            SET(DDRD,_BV(PD4),DIR)

#define BUTTON_UP_DIR(DIR)              SET(DDRA,_BV(PA0),DIR)
#define BUTTON_LEFT_DIR(DIR)            SET(DDRA,_BV(PA2),DIR)
#define BUTTON_DOWN_DIR(DIR)            SET(DDRA,_BV(PA1),DIR)
#define BUTTON_RIGHT_DIR(DIR)           SET(DDRA,_BV(PA3),DIR)
#define BUTTON_ACTIONA_DIR(DIR)         SET(DDRC,_BV(PC7),DIR)
#define BUTTON_ACTIONB_DIR(DIR)         SET(DDRC,_BV(PC6),DIR)
#define BUTTON_SELECT_DIR(DIR)         	SET(DDRC,_BV(PC1),DIR)

#define BACKLIGHT_DIR(DIR)              SET(DDRD,_BV(PD5),DIR)
#define LCD_RST_DIR(DIR)                SET(DDRD,_BV(PD6),DIR)
#define LCD_CS0_DIR(DIR)                SET(DDRB,_BV(PB3),DIR)
#define LCD_CD_DIR(DIR)                	SET(DDRB,_BV(PB2),DIR)

#define RAM_HOLD_DIR(DIR)      			SET(DDRA,_BV(PA6),DIR)
#define RAM_CS_DIR(DIR)        			SET(DDRA,_BV(PA4),DIR)

#define SPI_PORTS_DIR(DIR)              SET(DDRB,(_BV(PB5)|_BV(PB7)|_BV(PB4)),DIR); SET(DDRA,_BV(PB2),DIR)//////////////////////////////////////////////
#define SPI_MOSI_DIR(DIR)      			SET(DDRB,_BV(PB5),DIR)
#define SPI_MISO_DIR(DIR)      			SET(DDRB,_BV(PB6),DIR)
#define SPI_SCK_DIR(DIR)      			SET(DDRB,_BV(PB7),DIR)

/*Define I/O*/
//Devices Inputs
#define BUTTON_UP_INIT(STATE)           SET(PORTA,_BV(PA0),STATE)
#define BUTTON_LEFT_INIT(STATE)         SET(PORTA,_BV(PA2),STATE)
#define BUTTON_DOWN_INIT(STATE)         SET(PORTA,_BV(PA1),STATE)
#define BUTTON_RIGHT_INIT(STATE)        SET(PORTA,_BV(PA3),STATE)
#define BUTTON_ACTIONA_INIT(STATE)      SET(PORTC,_BV(PC7),STATE)
#define BUTTON_ACTIONB_INIT(STATE)      SET(PORTC,_BV(PC6),STATE)
#define BUTTON_SELECT_INIT(STATE)      	SET(PORTC,_BV(PC1),STATE)

#define BUTTON_UP_PRESSED               GET(PINA,_BV(PA0))
#define BUTTON_LEFT_PRESSED             GET(PINA,_BV(PA2))
#define BUTTON_DOWN_PRESSED             GET(PINA,_BV(PA1))
#define BUTTON_RIGHT_PRESSED            GET(PINA,_BV(PA3))
#define BUTTON_ACTIONA_PRESSED          GET(PINC,_BV(PC7))
#define BUTTON_ACTIONB_PRESSED          GET(PINC,_BV(PC6))
#define BUTTON_SELECT_PRESSED          	GET(PINC,_BV(PC1))

//Devices Outputs
#define BAT_LOW_LED_INIT(STATE)              	SET(PORTD,_BV(PD4),~STATE)
#define BACKLIGHT_INIT(STATE)              		SET(PORTD,_BV(PD5), STATE)

/*Define SPI*/
#define SPI_MOSI_INIT(STATE)        			SET(PORTB,_BV(PB5),STATE)
#define SPI_MISO_INIT(STATE)   	   				SET(PORTB,_BV(PB6),STATE)
#define SPI_SCK_INIT(STATE)        				SET(PORTB,_BV(PB7),STATE)

#define RAM_HOLD_INIT(STATE)        			SET(PORTA,_BV(PA6),STATE)
#define RAM_CS_INIT(STATE)        				SET(PORTA,_BV(PA4),STATE)

#define LCD_SS_INIT(STATE)            	 		SET(PORTB,_BV(PB4),STATE)
#define LCD_RST_INIT(STATE)             		SET(PORTD,_BV(PD6),STATE)
#define LCD_CS0_INIT(STATE)             		SET(PORTB,_BV(PB3),STATE)
#define LCD_CD_INIT(STATE)              		SET(PORTB,_BV(PB2),STATE)

//enable SPI in Master Mode with SCK = CK/4, CPHA = 1
#define SPI_MASTER_MODE(STATE)          SET(SPCR,(_BV(SPE)|_BV(MSTR)|_BV(CPHA)|_BV(CPOL)),STATE)

#endif

