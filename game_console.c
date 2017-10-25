/*************************************************************************
Title:    game_console Template
Initial Author:   David Jahshan
Extended by : Xiaoqian XIe 709716
Software: AVR-GCC 
Hardware: ATMEGA16 @ 8Mhz 

DESCRIPTION:
	Main and functions for Game Console basic sample code

*************************************************************************/

#include "game_console.h" 


byte frame_buffer[LCDcolumn][LCDpage-1];
byte i=0;
byte j=0;
byte ro=0;//row
byte co=0;//column

byte threshold[] = {0,90,100,110,120,130,140,150,200,255};
int condition = 0;//setup initial condition of backlight threshold




//INTERRUPT1 INT1 initialise
void int1_init( void )
	{
    	MCUCR = (1<<ISC11|1<<ISC10);    // enable rising edge on INT1
    	GICR = (1<<INT1);               // enable INT1
	}
//**************************************************************************


//SPI initialise
void SPI_INIT(void)
	{
	SET(SPCR,_BV(SPE),HIGH);
	SET(SPCR,_BV(MSTR),HIGH);
	SET(SPCR,_BV(SPR0),HIGH);
	//SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	}
//function of SPI data transmission
byte SPI_MasterTransmit(char Data)
{
	LCD_CS0_INIT(LOW);
	SPDR = Data;// start transmission 
	while(!(SPSR & (1<<SPIF)));// wait for transmission complete
		LCD_CS0_INIT(HIGH);	
	return SPDR;
}

//********************************************************************************

//function of command transmission
void LCD_command_tx(byte Data)
{	
	LCD_CD_INIT(LOW);// set LCD_CD = 0;
	SPI_MasterTransmit(Data);
}
//********************************************************************************


//function of data transmission
void LCD_data_tx(byte Data)
{
	LCD_CD_INIT(HIGH);
	SPI_MasterTransmit(Data);
}

//********************************************************************************

//function of LCD initialisation
void LCDInt(void)
{	
	LCD_RST_INIT(LOW);// rest LCD
	LCD_RST_INIT(HIGH);
	LCD_command_tx(0x40); // display start line 0
	LCD_command_tx(0xA1); // SEG reverse
	LCD_command_tx(0xC0); // normal COM0~COM63
	LCD_command_tx(0xA4); // disable -> Set All Pixel to ON
	LCD_command_tx(0xA6); // display inverse off
	LCD_command_tx(0xA2); // set LCD Bias Ratio A2/A3
	LCD_command_tx(0x2F); // set Power Control 28...2F
	LCD_command_tx(0x27); // set VLCD Resistor Ratio 20...27
	LCD_command_tx(0x81); // set Electronic Volume
	LCD_command_tx(0x10); // set Electronic Volume 00...3F
	LCD_command_tx(0xFA); // set Adv. Program Control
	LCD_command_tx(0x90); // set Adv. Program Control x00100yz yz column wrap x Temp Comp
	LCD_command_tx(0xAF); // display on
}

//********************************************************************************


//LCD select page and column
void select_page(byte page)
{
	LCD_command_tx(0xB0 | page); 
}

void select_column(byte x)
{
	LCD_command_tx(x & 0x0F);      			// set LSB address:  0000 CA[3..0] 
	LCD_command_tx((0x1F & (x>>4))|0x10);  	// set MSB address:  0001 CA[7..4]   
}

//********************************************************************************

//function of clear LCD screen
void LCD_Pixel_Clear(void)
{
	byte x0;
	byte page0;
	for (x0=0; x0<=LCDcolumn-1; x0++)
	{
		for (page0=0;page0<=LCDpage-1;page0++)
		{
			select_page(page0);
			select_column(x0);     
			LCD_data_tx(0x00);		
		}
	}
}

//********************************************************************************

//function of clear frame_buffer(initialisation)
void clear_frame_buffer(void)
{
	for(i=0;i<=LCDcolumn-1;i++){
		for(j=0;j<=LCDpage-1;j++){
			select_page(j);
			select_column(i);
			frame_buffer[i][j]=0x00;
		}
	}
}

//********************************************************************************

//PWM of LCD backlight
void LCD_BACKLIGHT(byte threshold)
{
	// set threshold
    OCR1AH = 0x00;
	OCR1AL = threshold;
	OCR1BH = 0x00;
	OCR1BL = threshold;
    // set compare output mode: phase correct
    SET(TCCR1A,_BV(COM1A1),HIGH);
	SET(TCCR1A,_BV(COM1A0),HIGH);
    SET(TCCR1A,_BV(COM1B1),HIGH);
	SET(TCCR1A,_BV(COM1B0),HIGH);  
    // set PWM (phase correct, 8-bit) TOP: 0x00FF
    SET(TCCR1A,_BV(WGM11),LOW);
	SET(TCCR1A,_BV(WGM10),HIGH);
	SET(TCCR1B,_BV(WGM13),LOW);
	SET(TCCR1B,_BV(WGM12),LOW);
	// clk selection ( choose clk cyle = clk/64)
	SET(TCCR1B,_BV(CS12),LOW);
    SET(TCCR1B,_BV(CS11),HIGH);
	SET(TCCR1B,_BV(CS10),HIGH);
}

//********************************************************************************

//send data to LCD screen
void send_data(void)
{
	for(i=0;i<=LCDcolumn-1;i++){
		for(j=0;j<=LCDpage-1;j++){
			select_page(j);
			select_column(i);
			LCD_data_tx(frame_buffer[i][j]);
		}			
	}
}

//********************************************************************************
		
//PLOT
void write_pixel(int c, int r)
{
	frame_buffer[c][r/8]=(_BV(r%8)|(frame_buffer[c][r/8]));
}

void write_pic(int cc, int rr)//Simpson
{
	/*write_pixel(cc+2, rr);
	write_pixel(cc-3, rr);
	write_pixel(cc-16, rr);
	write_pixel(cc+1, rr-1);
	write_pixel(cc+2, rr-1);
	write_pixel(cc-16, rr-1);
	write_pixel(cc+1, rr-2);
	write_pixel(cc-15, rr-2);
	write_pixel(cc+1, rr-3);
	write_pixel(cc-15, rr-3);
	write_pixel(cc+1, rr-4);
	write_pixel(cc-14, rr-4);
	write_pixel(cc+1, rr-5);
	write_pixel(cc-15, rr-5);
	write_pixel(cc-14, rr-5);
	write_pixel(cc-13, rr-5);
	write_pixel(cc-12, rr-5);
	write_pixel(cc-11, rr-5);
	write_pixel(cc+1, rr-6);
	write_pixel(cc+2, rr-6);
	write_pixel(cc+3, rr-6);
	write_pixel(cc-16, rr-6);
	write_pixel(cc+3, rr-7);
	write_pixel(cc+4, rr-7);
	write_pixel(cc-16, rr-7);
	write_pixel(cc-9, rr-7);
	write_pixel(cc-8, rr-7);
	write_pixel(cc-7, rr-7);
	write_pixel(cc-6, rr-7);
	write_pixel(cc-5, rr-7);
	write_pixel(cc+2, rr-8);
	write_pixel(cc+4, rr-8);
	write_pixel(cc-15, rr-8);
	write_pixel(cc-14, rr-8);
	write_pixel(cc-13, rr-8);
	write_pixel(cc-12, rr-8);
	write_pixel(cc-10, rr-8);
	write_pixel(cc-5, rr-8);
	write_pixel(cc+1, rr-9);
	write_pixel(cc+4, rr-9);
	write_pixel(cc-16, rr-9);
	write_pixel(cc-11, rr-9);
	write_pixel(cc-10, rr-9);
	write_pixel(cc-4, rr-9);
	write_pixel(cc+1, rr-10);
	write_pixel(cc+2, rr-10);
	write_pixel(cc+3, rr-10);
	write_pixel(cc-4, rr-10);
	write_pixel(cc-7, rr-10);
	write_pixel(cc-11, rr-10);
	write_pixel(cc-17, rr-10);
	write_pixel(cc+1, rr-11);
	write_pixel(cc-4, rr-11);
	write_pixel(cc-11, rr-11);
	write_pixel(cc-13, rr-11);
	write_pixel(cc-17, rr-11);
	write_pixel(cc+2, rr-12);
	write_pixel(cc-5, rr-12);
	write_pixel(cc-10, rr-12);
	write_pixel(cc-11, rr-12);
	write_pixel(cc-17, rr-12);
	write_pixel(cc+2, rr-13);
	write_pixel(cc-6, rr-13);
	write_pixel(cc-7, rr-13);
	write_pixel(cc-8, rr-13);
	write_pixel(cc-9, rr-13);
	write_pixel(cc-11, rr-13);
	write_pixel(cc-16, rr-13);
	write_pixel(cc+2, rr-14);
	write_pixel(cc-12, rr-14);
	write_pixel(cc-13, rr-14);
	write_pixel(cc-14, rr-14);
	write_pixel(cc-15, rr-14);
	write_pixel(cc+2, rr-15);
	write_pixel(cc-14, rr-15);
	write_pixel(cc+3, rr-16);
	write_pixel(cc-14, rr-16);
	write_pixel(cc+3, rr-17);
	write_pixel(cc-13, rr-17);
	write_pixel(cc+3, rr-18);
	write_pixel(cc-13, rr-18);
	write_pixel(cc+3, rr-19);
	write_pixel(cc-13, rr-19);
	write_pixel(cc+3, rr-20);
	write_pixel(cc-13, rr-20);
	write_pixel(cc+4, rr-21);
	write_pixel(cc-13, rr-21);
	write_pixel(cc+4, rr-22);
	write_pixel(cc-13, rr-22);
	write_pixel(cc+4, rr-23);
	write_pixel(cc-13, rr-23);
	write_pixel(cc+4, rr-24);
	write_pixel(cc-12, rr-24);
	write_pixel(cc+4, rr-25);
	write_pixel(cc-12, rr-25);
	write_pixel(cc, rr-25);
	write_pixel(cc-1, rr-25);
	write_pixel(cc-5, rr-25);
	write_pixel(cc-9, rr-25);
	write_pixel(cc-10, rr-25);
	write_pixel(cc+4, rr-26);
	write_pixel(cc-12, rr-26);
	write_pixel(cc+1, rr-26);
	write_pixel(cc-1, rr-26);
	write_pixel(cc-2, rr-26);
	write_pixel(cc-5, rr-26);
	write_pixel(cc-9, rr-26);
	write_pixel(cc-11, rr-26);
	write_pixel(cc+4, rr-27);
	write_pixel(cc-12, rr-27);
	write_pixel(cc+2, rr-27);
	write_pixel(cc-2, rr-27);
	write_pixel(cc-4, rr-27);
	write_pixel(cc-6, rr-27);
	write_pixel(cc-8, rr-27);
	write_pixel(cc+3, rr-28);
	write_pixel(cc-3, rr-28);
	write_pixel(cc-7, rr-28);
	write_pixel(cc+2, rr+1);
	write_pixel(cc-3, rr+1);
	write_pixel(cc-5, rr+1);
	write_pixel(cc-6, rr+1);
	write_pixel(cc-7, rr+1);
	write_pixel(cc-8, rr+1);
	write_pixel(cc-15, rr+1);
	write_pixel(cc+2, rr+2);
	write_pixel(cc-7, rr+2);
	write_pixel(cc-8, rr+2);
	write_pixel(cc-9, rr+2);
	write_pixel(cc-10, rr+2);
	write_pixel(cc-11, rr+2);
	write_pixel(cc-12, rr+2);
	write_pixel(cc-13, rr+2);
	write_pixel(cc-14, rr+2);
	write_pixel(cc+3, rr+3);
	write_pixel(cc+4, rr+3);
	write_pixel(cc-10, rr+3);
	write_pixel(cc+3, rr+4);
	write_pixel(cc+5, rr+4);
	write_pixel(cc-8, rr+4);
	write_pixel(cc-9, rr+4);
	write_pixel(cc-10, rr+4);
	write_pixel(cc, rr+5);
	write_pixel(cc+1, rr+5);
	write_pixel(cc+6, rr+5);
	write_pixel(cc-1, rr+5);
	write_pixel(cc-6, rr+5);
	write_pixel(cc+5, rr+6);
	write_pixel(cc+6, rr+6);
	write_pixel(cc+7, rr+6);
	write_pixel(cc+8, rr+6);
	write_pixel(cc-2, rr+6);
	write_pixel(cc-3, rr+6);
	write_pixel(cc-4, rr+6);
	write_pixel(cc-5, rr+6);
	write_pixel(cc-6, rr+6);
	write_pixel(cc+4, rr+7);
	write_pixel(cc+9, rr+7);
	write_pixel(cc-6, rr+7);
	write_pixel(cc-7, rr+7);
	write_pixel(cc+3, rr+8);
	write_pixel(cc+10, rr+8);
	write_pixel(cc-7, rr+8);
	write_pixel(cc-8, rr+8);
	write_pixel(cc-9, rr+8);
	write_pixel(cc+3, rr+9);
	write_pixel(cc+11, rr+9);
	write_pixel(cc-7, rr+9);
	write_pixel(cc-10, rr+9);
	write_pixel(cc+2, rr+10);
	write_pixel(cc+11, rr+10);
	write_pixel(cc-8, rr+10);
	write_pixel(cc-10, rr+10);
	write_pixel(cc+2, rr+11);
	write_pixel(cc+5, rr+11);
	write_pixel(cc+6, rr+11);
	write_pixel(cc+7, rr+11);
	write_pixel(cc+8, rr+11);
	write_pixel(cc+9, rr+11);
	write_pixel(cc+10, rr+11);
	write_pixel(cc+11, rr+11);
	write_pixel(cc-8, rr+11);
	write_pixel(cc-11, rr+11);
	write_pixel(cc+2, rr+12);
	write_pixel(cc+4, rr+12);
	write_pixel(cc+5, rr+12);
	write_pixel(cc+9, rr+12);
	write_pixel(cc+11, rr+12);
	write_pixel(cc-8, rr+12);
	write_pixel(cc-11, rr+12);
	write_pixel(cc-12, rr+12);*/
	write_pixel(cc-1, rr);
	write_pixel(cc+1, rr);
	write_pixel(cc+1, rr+1);
	write_pixel(cc-1, rr+1);
	write_pixel(cc+1, rr-1);
	write_pixel(cc-1, rr-1);
	write_pixel(cc+1, rr);
	write_pixel(cc-1, rr);
}

void write_heart(int cc, int rr)
{
	write_pixel(cc+2, rr);
	write_pixel(cc+4, rr);
        write_pixel(cc+6, rr);
	write_pixel(cc, rr);
	write_pixel(cc-2, rr);
	write_pixel(cc-4, rr);
	write_pixel(cc-6, rr);
        write_pixel(cc, rr-2);
	write_pixel(cc, rr-4);
	write_pixel(cc, rr-6);
	write_pixel(cc, rr+2);
	write_pixel(cc, rr+4);
	write_pixel(cc, rr+6);
        write_pixel(cc+2, rr+2);
	write_pixel(cc+4, rr+4);
	write_pixel(cc+6, rr+6);
	write_pixel(cc-2, rr-2);
	write_pixel(cc-4, rr-4);
	write_pixel(cc-6, rr-6);
	write_pixel(cc+2, rr-2);
	write_pixel(cc+4, rr-4);
	write_pixel(cc+6, rr-6);
	write_pixel(cc-2, rr+2);
	write_pixel(cc-4, rr+4);
	write_pixel(cc-6, rr+6);

}
//********************************************************************************

//As long as INTERRUPT PORT INT1 at PD4 toggled flag rises
    ISR( INT1_vect )
	{
	 	if (BUTTON_SELECT_PRESSED)
						{
							if (condition == 11){ condition=(-1);}
							condition=condition+1;
							LCD_BACKLIGHT(threshold[condition]);
						_delay_ms(10);	
						} // toggle flag
	}

//**************************************************************************

//ADC test battary_low 
void ADC_initialise(void)
{
	ADMUX = (1<<REFS1)|(1<<REFS0)|(1<<MUX1)|(1<<MUX0);//ref = 2.65V, ADC3
	ADCSRA |= (1<<ADSC); // Start conversion
}


byte read_adc(void)
{		
		while (ADCSRA & (1<<ADSC)); // wait for conversion to complete
  		return(ADC);
}

//********************************************************************************
//********************************************************************************
//********************************************************************************


//MAIN FUNCTION
int main(void)
{
	BUTTON_UP_DIR(IN);
 	BUTTON_LEFT_DIR(IN);
 	BUTTON_DOWN_DIR(IN);
 	BUTTON_RIGHT_DIR(IN);
 	BUTTON_ACTIONA_DIR(IN);
 	BUTTON_ACTIONB_DIR(IN);
 	BUTTON_SELECT_DIR(IN);

	BUTTON_UP_INIT(HIGH);
 	BUTTON_LEFT_INIT(HIGH);
 	BUTTON_DOWN_INIT(HIGH);
 	BUTTON_RIGHT_INIT(HIGH);
 	BUTTON_ACTIONA_INIT(HIGH);
 	BUTTON_ACTIONB_INIT(HIGH);
 	BUTTON_SELECT_INIT(HIGH);

	BACKLIGHT_DIR(OUT);
	BACKLIGHT_INIT(HIGH);

	BAT_LOW_LED_DIR(OUT);
	BAT_LOW_LED_INIT(OFF); 
	
	SPI_PORTS_DIR(OUT); 
	SPI_MOSI_DIR(OUT);
	SPI_SCK_DIR(OUT);

	LCD_CS0_DIR(OUT);
	LCD_CD_DIR(OUT);
	LCD_RST_DIR(OUT);
	LCD_SS_INIT(HIGH);//PB4

	//ADC init
	ADC_initialise();


	// enable SPI and Master mode
	SPI_INIT();
	
//********************************************************************************


    sei();//enables global interrupt
	int1_init();
	LCD_BACKLIGHT(0);//light up backlight
	LCDInt();

//********************************************************************************

	LCD_Pixel_Clear();
	clear_frame_buffer();
	send_data();
	ro = 30;//the midpoint of pic
	co = 51;	
	write_pic(co, ro);
	send_data();

//********************************************************************************


	while (TRUE)
	{
		//ADC
		if(read_adc() < LOW_BATTARY){
			BAT_LOW_LED_INIT(ON);
		}

		if (BUTTON_RIGHT_PRESSED)
		{
			if(co==17)
			{
				ro = ro;
				co = co;
				write_pic(co, ro);
				send_data();
				clear_frame_buffer();
			}
			else
			{
				ro = ro;
				co = co-1;
				write_pic(co, ro);
				send_data();
				clear_frame_buffer();
			}		
		} 	
	
//********************************************************************************

		if (BUTTON_LEFT_PRESSED)
		{
			if(co==91)
			{
				ro = ro;
				co = co;
				write_pic(co, ro);
				send_data();
				clear_frame_buffer();
			}
			else
			{
				ro = ro;
				co = co+1;
				write_pic(co, ro);
				send_data();
				clear_frame_buffer();
			}		
		} 	
//********************************************************************************


		if (BUTTON_ACTIONA_PRESSED)
		{
			write_heart(co, ro);
			send_data();
			clear_frame_buffer();
			
			while (INFINITE_LOOP)
			{
	
					if (BUTTON_RIGHT_PRESSED)
					{
						if(co==5)
						{
							ro = ro;
							co = co;
							write_heart(co, ro);
							send_data();
							clear_frame_buffer();
						}
						else
						{
							ro = ro;
							co = co-1;
							write_heart(co, ro);
							send_data();
							clear_frame_buffer();
						}		
					} 	

					if (BUTTON_LEFT_PRESSED)
					{
						if(co==96)
						{
							ro = ro;
							co = co;
							write_heart(co, ro);
							send_data();
							clear_frame_buffer();
						}
						else
						{
							ro = ro;
							co = co+1;
							write_heart(co, ro);
							send_data();
							clear_frame_buffer();
						}		
					} 	

					if (BUTTON_ACTIONB_PRESSED)
					{
						write_pic(co, ro);
						send_data();
						clear_frame_buffer();	
						break;
					} 
			}				
		} 	
	}
}
