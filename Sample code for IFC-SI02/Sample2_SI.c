//=====================================================================================
//
//	Author				: Cytron Technologies		
//	Project				: Interface Free Controller-Stepper Indexer Card (IFC-SI02)
//	Project description	: Sample1 for IFC-SI02
//  Date				: Nov 2008
//
//=====================================================================================

//=====================================================================================
//	include h file for cards
//	User has to include h file for all the cards used 
//=====================================================================================
#include<p18f2685.h>
#include "iic.h"
#include "iic_si.h"
#include "iic_cp.h"

//=====================================================================================
//	configuration bit, User is advised not to change the setting here
//=====================================================================================
#pragma config OSC = HSPLL
#pragma config FCMEN = OFF 
#pragma config IESO = OFF 
#pragma config PWRT = OFF 
#pragma config BOREN = OFF 
#pragma config WDT = OFF 
#pragma config MCLRE = ON
#pragma config PBADEN = OFF 
#pragma config DEBUG = OFF
#pragma config XINST = OFF
#pragma config LVP = OFF 

//=====================================================================================
//	define input or ouput of board here
//	For example: 
//				1.led1 was defined for first LED on MB00
//				2.buzzer was defined for buzzer on MB00
//	The address for input/output devide on board is fixed, hence user is 
//	advised not to change the defined address for input/output 
//=====================================================================================

#define	led1		LATBbits.LATB0 	// led1 defined as output for first LED on MB00	
#define	led2		LATBbits.LATB1	// led2 defined as output for second LED on MB00
#define	led3		LATBbits.LATB2	// led3 defined as output for third LED on MB00
#define	led4		LATBbits.LATB3	// led4 defined as output for forth LED on MB00
#define	led5		LATBbits.LATB4	// led5 defined as output for fifth LED on MB00
#define	led6		LATBbits.LATB5	// led6 defined as output for sixth LED on MB00
#define	led7		LATBbits.LATB6	// led7 defined as output for seventh LED on MB00
#define	led8		LATBbits.LATB7	// led8 defined as output for eighth LED on MB00
#define	led			LATB			// led defined as output for ALL LED on MB00

#define	sw1			PORTAbits.RA2	// sw1 defined as input for SW1 on MB00
#define	sw2			PORTAbits.RA1	// sw2 defined as input for SW2 on MB00
#define	sw3			PORTAbits.RA0	// sw3 defined as input for SW3 on MB00
#define	buzzer		LATCbits.LATC5	// buzzer defined as output for buzzer on MB00
#define busy_led	LATCbits.LATC1	// busy_led defined as output for Busy LED on MB00
#define error_led	LATCbits.LATC2	// error_led defined as output for Error LED on MB00
#define slave_reset	LATCbits.LATC0	// slave_reset define as output to reset slave cards

//=====================================================================================
//	card address
//	User can define the address of slave card here
//=====================================================================================
#define add_si1		0b010101	// Please make sure the communication address on 
									// IFC-si02 is compatisie with the address 

//=====================================================================================
//	function prototype
//	User needs to include all the function prototype here
//	The first 4 functions prototype are fixed, user is also advised not to
//	change it.
//=====================================================================================
void ISRHigh(void);
void ISRLow(void);
void delay(unsigned long data);
void init(void);
	
//=====================================================================================
//	Main Function
//	This is the main function where program start to execute 
//=====================================================================================

void main(void)
{//--------------------------------------------------------------------------------
	//	Program start
	//	User can start to write/modify the program here
	//--------------------------------------------------------------------------------
	//local variables
	unsigned short i,temp1=0;
	unsigned char flag1=1;	

	//	User is advised not to change or remove the initialization function called 
	init();					// call initialization function of the setting in program
	delay(200000);				// important !! waiting for slave card to ready
		
	si_maxfreq(add_si1,80);
	si_1_setacc(add_si1, 200);
	si_2_setacc(add_si1, 200);
	si_1_cson(add_si1);
	si_2_cson(add_si1);
	si_1_cw(add_si1);
	si_2_cw(add_si1);

	while(1)
	{

		//--------------------------------------------------------------------------------
		// This program is mainly for slave card,SI02. It will test the 3 push buttons
		// on MB00, the push buttons are active low, hence, when any of the push button 
		// is pressed, value '0' will be sent to microcontroller on master card, MB00.
		// Hence, 2 Stepper motors will run base on program
		//--------------------------------------------------------------------------------

		if(cp1_sw(1)==0)					// Test whether SW1 on IFC-MB00 is pressed
		{		
			si_1_econclr(add_si1);				//clear encoder and encoder functions
			si_2_econclr(add_si1);
			si_1_enclr(add_si1);
			si_2_enclr(add_si1);
			
			cp1_clr();					// clear LCD screen by calling function cp1_clr
			temp1=0;
			while(temp1<255)
			{
				temp1+=1;
				cp1_blight(temp1);
				delay(500);
			}
							
			cp1_goto(0,0);					// set the position of message to be displayed
			cp1_str("Demo1:");				// send message to be displayed by calling function cp1_str
			led1 = 1;					// turn ON LED1 on MB00 by sending value '1'
			buzzer = 1;					// turn ON buzzer on MB00 by sending value '1'
			delay(50000);					// delay for buzzer ON time
			buzzer = 0;					// turn OFF buzzer on MB00 by sending value '0'
			delay(500000);
			cp1_clr();	
			
			// 2 stepper Motors are activated. 

			si_1_cw(add_si1);				// Run motor SI1 in clockwise
			si_1_speed(add_si1,250);			// SI1's speed = 250				
			si_2_ccw(add_si1);				// Run motor SI2 in counter clockwise
			si_2_speed(add_si1,250);			// SI2's speed = 250
			delay(50000);					// delay, wait time for motors to accelerate from speed 0 to 1

			cp1_goto(0,8);
			cp1_str("c1:CW  ");
			cp1_goto(1,8);
			cp1_str("c2:CCW ");
				
			while(si_2_spval(add_si1)<250)  		// waiting for motor speed to increase
			{
				cp1_goto(0,0);
				cp1_str("sp1:");
				cp1_dec(si_1_spval(add_si1),3);		//displaying channel 1 speed
				
				cp1_goto(1,0);
				cp1_str("sp2:");
				cp1_dec(si_2_spval(add_si1),3);		//displaying channel 2 speed
				
			}
			
			si_1_speed(add_si1,0);				// speed=0 motor SI1				
			si_2_speed(add_si1,0);				// speed=0 motor SI2
			cp1_goto(0,8);
			cp1_str("c1:-sp");
			cp1_goto(1,8);
			cp1_str("c2:-sp");
			while(si_2_runstat(add_si1))			// waiting for motor speed to become zero
			{
				cp1_goto(0,0);
				cp1_str("sp1:");
				cp1_dec(si_1_spval(add_si1),3);		//displaying channel 1 speed
				
				cp1_goto(1,0);
				cp1_str("sp2:");
				cp1_dec(si_2_spval(add_si1),3);		//displaying channel 2 speed
			}	
				
			delay(50000);					// delay, wait time 

			si_1_ccw(add_si1);				// Run motor SI1 in counter-clockwise
			si_1_speed(add_si1,10);				// SI1's speed = 10
			si_2_cw(add_si1);				// Run motor SI2 in clockwise
			si_2_speed(add_si1,10);				// SI2's speed = 10
			delay(50000);					// delay, wait time for motors to accelerate from speed 0 to 1	
			
			cp1_goto(0,8);
			cp1_str("c1:CCW ");
			cp1_goto(1,8);
			cp1_str("c2:CW  ");
			
			for(i=0;i<1000;i++)				// delay for motors running time
			{
				delay(500);
				cp1_goto(0,0);
				cp1_str("sp1:");
				cp1_dec(si_1_spval(add_si1),3);		//displaying channel 1 speed
				
				cp1_goto(1,0);
				cp1_str("sp2:");
				cp1_dec(si_2_spval(add_si1),3);		//displaying channel 2 speed
			}
			
			si_1_brake(add_si1);				// brake motor SI1
			si_2_brake(add_si1);				// brake motor SI2
			delay(50000);					// delay, wait time for motors to accelerate from speed 0 to 1
			cp1_goto(0,8);
			cp1_str("c1:brake");
			cp1_goto(1,8);
			cp1_str("c2:brake");
			
			
		}

		if(cp1_sw(2)==0)							// Test whether SW2 on IFC-MB00 is pressed
		{	
			
			// infinite loop, user need to press reset to exit the loop
			
			si_1_econclr(add_si1);						//clear encoder and encoder functions
			si_2_econclr(add_si1);
			si_1_enclr(add_si1);
			si_2_enclr(add_si1);
			
			cp1_clr();
			temp1=0;
			while(temp1<255)
			{
				temp1+=1;
				cp1_blight(temp1);
				delay(500);
			}
			cp1_goto(0,0);					// set the position of message to be displayed
			cp1_str("Demo2:");				// send message to be displayed by calling function cp1_str
			led2 = 1;					// turn ON LED2 on MB00 by sending value '1'
			buzzer = 1;					// buzzer ON for first time
			delay(50000);					// delay for buzzer ON time
			buzzer = 0;					// OFF buzzer
			delay(50000);					// delay for buzzer OFF time
			buzzer = 1;					// buzzer ON for second times
			delay(50000);					// delay for buzzer ON time
			buzzer = 0;					// OFF buzzer
			delay(500000);
			cp1_clr();
						
			
				// 2 Stepper Motors are activated. The motors will change the running 
				// direction and speed based on the encoder value.
				
				si_1_cw(add_si1);					// Run motor si1 in clockwise
				si_1_speed(add_si1,250);				// si1's speed = 250
				si_1_encon(add_si1, 50000, 4, 100,0);			// change si1's speed and direction
											// when encoder value = 50000.
				si_2_ccw(add_si1);					// Run motor si1 in clockwise
				si_2_speed(add_si1,250);
				si_2_encon(add_si1, 40000, 3, 0,100);			// change si2 speed and direction
											// when encoder value = 40000.
															
															
				while(si_1_enval(add_si1)<50010)			// wait for enconder tasks are done
				{
					cp1_goto(0,0);
					cp1_str("sp1:");
					cp1_dec(si_1_spval(add_si1),3);			//displaying channel 1 speed
					
					cp1_goto(1,0);
					cp1_str("sp2:");
					cp1_dec(si_2_spval(add_si1),3);			//displaying channel 2 speed
					
					cp1_goto(0,8);
					cp1_str("c1:");
					if(si_1_dirstat(add_si1)) cp1_str("CCW");	//checking and displaying channel 1 direction
					else cp1_str("CW " );
					
					cp1_goto(1,8);
					cp1_str("c2:");
					if(si_2_dirstat(add_si1)) cp1_str("CCW");	//checking and displaying channel 2 direction
					else cp1_str("CW ");
					
				}	
				si_1_speed(add_si1,0);					// change speed of motors to zero	
				si_2_speed(add_si1,0);
				
				while(si_1_runstat(add_si1)|si_2_runstat(add_si1)) 	 //waiting for both motors to completely brake
				{	
					cp1_goto(0,0);
					cp1_str("sp1:");
					cp1_dec(si_1_spval(add_si1),3);			//displaying channel 1 speed
					
					cp1_goto(1,0);
					cp1_str("sp2:");
					cp1_dec(si_2_spval(add_si1),3);			//displaying channel 2 speed
					
					cp1_goto(0,8);
					cp1_str("c1:");
					if(si_1_dirstat(add_si1)) cp1_str("CCW");	//checking and displaying channel 1 direction
					else cp1_str("CW ");
					
					cp1_goto(1,8);
					cp1_str("c2:");
					if(si_2_dirstat(add_si1)) cp1_str("CCW");	//checking and displaying channel 2 direction
					else cp1_str("CW ");
					
				}		
				delay(100000);							// delay for motors brake time
				
				
				//functions are running motors for one turn then brake
				si_1_enclr(add_si1);				//this set of encoder functions will
				si_1_econclr(add_si1);				//turn one round(2000) pulse SD02(micro stepping of 10) with 
				si_1_speed(add_si1,25);				//stepper motor of step angle 1.8 degree
				si_1_encon(add_si1,1068,5, 1,0);				
				si_1_encon(add_si1, 2000, 2, 0, 0);
				
				si_2_enclr(add_si1);				//this set of enconder functions will 
				si_2_econclr(add_si1);				//turn one round(28800 pulse) Vexta stepper motor of
				si_2_speed(add_si1,89);				//step angle 0.2 degree include gear head (with Vexta
				si_2_encon(add_si1,17317,5, 1,0);		//Stepper Driver micro stepping of 16)			
				si_2_encon(add_si1, 28800, 2, 0, 0);
				
				cp1_clr();
				while(si_2_enval(add_si1)<28810)
				{
					cp1_goto(0,0);
					cp1_str("En1:");
					cp1_dec(si_1_enval(add_si1),8);		//Display channel 1 encoder value
					//cp1_str(" SD02");
					
					cp1_goto(1,0);
					cp1_str("En2:");
					cp1_dec(si_2_enval(add_si1),8);		//Display channel 2 encoder value
					//cp1_str("Vexta");
					
				}	
		
		}

		if(cp1_sw(3)==0)			   			// Test whether SW3 on IFC-MB00 is pressed
		{
			si_1_econclr(add_si1);	//clear encoder and encoder functions
			si_2_econclr(add_si1);
			si_1_enclr(add_si1);
			si_2_enclr(add_si1);
			
			cp1_clr();
			temp1=0;
			while(temp1<255)
			{
				temp1+=1;
				cp1_blight(temp1);
				delay(500);
			}
			
			cp1_goto(0,0);					// set the position of message to be displayed
			cp1_str("Demo3:");				// send message to be displayed by calling function cp1_str		
			led3 = 1;					// turn ON LED3 on MB00 by sending value '1'
			buzzer = 1;					// buzzer ON for first time
			delay(50000);					// delay for buzzer ON time
			buzzer = 0;					// OFF buzzer
			delay(50000);					// delay for buzzer OFF time
			buzzer = 1;					// buzzer ON for second times
			delay(50000);					// delay for buzzer ON time
			buzzer = 0;					// OFF buzzer
			delay(50000);					// delay for buzzer OFF time
			buzzer = 1;					// buzzer ON for third times
			delay(50000);					// delay for buzzer ON time
			buzzer = 0;					// OFF buzzer
			
			delay(500000);
			cp1_clr();
			
			si_1_setacc(add_si1,250);		//set channel 1 acceleration to 250
			si_2_setacc(add_si1,250);		//set channel 2 acceleration to 250
			si_1_cw(add_si1);				//set channel 1 direction to cw
			si_2_cw(add_si1);				//set channel 2 direction to cw
			
			// infinite loop, user need to press reset to exit the loop
			
			//this set of codes will run motor SI1 1st, when the speed of SI1 reach 128, SI2 will start run.
			//both channel will increase speed to maximum speed then decrease speed to zero and change direction.
			// motors will accelerate in the other direction and slow down after maximum speed repeatly.
			
			while (1)
			{		
				
				if((si_1_spval(add_si1)>=128)&flag1)
				{
					si_2_speed(add_si1,255);
					flag1=0;
				}
				
				if(si_1_spval(add_si1)==255)
				{
					si_1_speed(add_si1,0);
					
				}
				if(si_2_spval(add_si1)==255)
				{
					si_2_speed(add_si1,0);
					
				}
				
				if((si_2_spval(add_si1)==0)&(flag1==0))
					{	
						
						if(si_2_dirstat(add_si1)) si_2_cw(add_si1); 
						else si_2_ccw(add_si1);
						si_2_speed(add_si1, 255);
						
					}	
				if(si_1_spval(add_si1)==0)
					{  	
						if(si_1_dirstat(add_si1)) si_1_cw(add_si1); 
						else si_1_ccw(add_si1);
						si_1_speed(add_si1, 255);
						
					}
					
				cp1_goto(0,0);
				cp1_str("Sp1:");
				cp1_dec(si_1_spval(add_si1),3);
				cp1_goto(1,0);
				cp1_str("Sp2:");
				cp1_dec(si_2_spval(add_si1),3);
				
				cp1_goto(0,8);
				cp1_str("c1:");
				if(si_1_dirstat(add_si1)) cp1_str("CCW");		//checking and displaying channel 1 direction
				else cp1_str("CW ");
				
				cp1_goto(1,8);
				cp1_str("c2:");
				if(si_2_dirstat(add_si1)) cp1_str("CCW");		//checking and displaying channel 2 direction
				else cp1_str("CW ");
				
			}

		}
	}
	
	
		
} //endmain



//=====================================================================================
//	functions
//  User can write all the necessary function here
//=====================================================================================
void delay(unsigned long data)
{
	// this is a delay function for user to use when the program need a delay
	// this function can be call by type : delay(xxxx),
	// user can replace the 'xxxx' with a value to determine how long the program
	// should delay for, the bigger the value, the longer the time of the delay
	for( ;data>0;data-=1);
}


void init(void)
{
	// this is a initialization for set the input and output of PIC18F2685
	// User is advised NOT to change the setting here
	// tris
	TRISB=0x00;				// led as output
	TRISA=0xff;				// push button as input
	TRISC=0x00;				// set port c as output
	
	// the setting for analog to digital converter, ADC for PIC18F2685
	// User is advised NOT to change the setting here
	// adc
	ADCON1=0x0F;
	
	//call the initialization function of I2C
	iic_init();		
	
	//	set the initial condition of output device on board
	//	User is advised not to change the initial condition
	led=0x00;				// turn OFF all LED on board by sending value '0x00'
	buzzer=0;				// turn OFF buzzer on board by sending value '0'
	error_led=0;			// turn OFF Error LED on board by sending value '0'
	busy_led=1;				// turn ON Busy LED on board by sending value '1'
	slave_reset=1;			// reset slave cards
	delay(10000);			
	slave_reset=0;			
	delay(80000);			
	busy_led=0;				// turn OFF Busy LED on board by sending value '0'
}


//=====================================================================================
//	Interrupt vector
//=====================================================================================
#pragma	code InterruptVectorHigh = 0x08
void InterruptVectorHigh(void)
{
 	_asm
		goto ISRHigh		// jump to interrupt routine
	_endasm
}
#pragma code
#pragma	code InterruptVectorLow = 0x18
void InterruptVectorLow(void)
{
 	_asm
		goto ISRLow			// jump to interrupt routine
	_endasm
}
#pragma code

//=====================================================================================
//	Interupt Service Routine
//	this a function reserved for interrupt service routine
//	User may need it in advance development of the program
//=====================================================================================
#pragma interrupt ISRHigh
void ISRHigh(void)
{

}

#pragma interrupt ISRLow
void ISRLow(void)
{

}

