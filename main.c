//Uzair Jawaid
//2DP4 Final Project


#include <hidef.h>
#include "derivative.h"
#include "SCI.h" //allows us to use serial communication

char string[20];
unsigned short val;
int counter = 0;  //button mode counter
float x;  //angle
int xRounded;  //linear bar for LEDs
 
void setClk();
void delay1ms(int multiple);

void BCD (int bcdVal);  //function to convert angle to BCD
void BCDBar (int bcdVal);  //function to convert angle to BCD in linear bar format


void OutCRLF(void)
{
  SCI_OutChar(CR);
  SCI_OutChar(LF);
}

void main(void) 
{

    TSCR1 = 0x90;
    TSCR2 = 0x04;
    TIOS = 0xFC;
    
    PERT = 0x03;
    
    TCTL3 = 0x00;
    TCTL4 = 0x0A;
    TIE = 0x01;
    IRQCR = 0x00;
    EnableInterrupts;
    

  //configure ADC channel
    ATDCTL1 = 0x2F; //configure for 10 bit resolution
    ATDCTL3 = 0x88; //right justified
    ATDCTL4 = 0x03; //prescaler set to 3; 8MHz/(2*(3+1)) = 1Mhz
    ATDCTL5 = 0x24; //continous conversion on channel 4
    setClk(); //set the E-Clock speed
    
    SCI_Init(38400); //initialize serial port
    
    
    DDRJ = 0xFF; 
    DDRP = 0xFF;
    
    //Configure PAD0-PAD7 Pins
    DDR1AD = 0x0F; //0x00; //driving the row with output pins using PT1AD

    //test serial communication between ESDX and PC
    SCI_OutString("Connection established.");
    SCI_OutChar(CR);
          
    while(1)                    
    {
        if(counter % 3 == 1)  //mode 0 will output the degree in decimal and the LEDs in binary
  	    {
  	     
          val= ATDDR0; //stores digital value
  	      
  	      //output angle using appropriate linear equation
  	      if(val < 544 && val >= 503) 
  	      {
  	        x =  0.4501*val - 226.88;
  	      } 
  	      else if (val >= 544 && val <= 608)
  	      {
  	        x = 0.8494*val - 443.18;
  	      } 
  	      else if (val <503) 
  	      {
  	        x = 0.0;
  	      }
  	      else
  	      {
  	        x = 5*val - 2960;
  	      }
  	        
  	      SCI_OutUDec(x);
  	      BCD(x);
  	      
  	      delay1ms(126);
  	      //delay1ms(1);
  	      SCI_OutChar(CR);
  	      PTJ^= 0x00;
  	    } 
  	    else if (counter % 3 == 2) //mode 1 will show the LED as bars for degrees in 10s
  	    {
  	      
  	      val= ATDDR0; //stores digital value
  	      
  	      //output angle using appropriate linear equation
  	      if(val < 544) 
  	      {
  	        x =  0.4501*val - 226.88;
  	      } 
  	      else if (val >= 544 && val <= 608)
  	      {
  	        x = 0.8494*val - 443.18;
  	      } 
  	      else
  	      {
  	        x = 5*val - 2960;
  	      }
  	      
  	      xRounded = x/10;
  	      SCI_OutUDec(xRounded*10);
  	      BCDBar(xRounded);
  	      delay1ms(126);
  	      //delay1ms(1);
  	      SCI_OutChar(CR);
  	      PTJ^= 0x00;
  	    } 
  	    else 
  	    {
  	      
  	    }    
    
        //Bus speed verification
        //delay1ms(1000);
        //PTJ^=0x01;
    }
}

interrupt VectorNumber_Vtimch0 void ISR_Vtimch0(void)
{
   unsigned int temp;
   counter = counter + 1;
   PTJ ^= 0x01;
   temp = TC0;  
}

//Bus Speed
void setClk(void)
{
  CPMUCLKS = 0x80; // choose PLL = 1
  CPMUOSC = 0x00; // source clock chosen
  CPMUPROT = 0x26; // protection of clock config is enable
  // now frequency_ref = 1MHz
  CPMUSYNR = 0x1F; // VCOFRQ = 00, SYNDIV = 31 = F; 64MHz = VCOCLK = 2*1MHz*(1+SYNDIV);
  CPMUFLG = 0x00; // LOCK = 0
  // PLLCLK = 64MHz/4 = 16MHz and BUSCLK = PLLCLK/2 = 8MHz 
}

//Delay Function
void delay1ms(int multiple)
{
      int ix;
      TSCR1 = 0x90;   /* enable timer and fast timer flag clear */
      TSCR2 = 0x00;   /* disable timer interrupt, set prescaler to 1*/
      TIOS = 0x02;   /* enable OC0 */        //(not necessary)
      TC0 = TCNT + 8000;     
      for(ix = 0; ix < multiple; ix++) {
             while(!(TFLG1_C1F));
             TC1 += 8000;
      }
}

void BCD (int bcdVal)
{
    
    unsigned char ones;
    unsigned int tens;
    unsigned int temp;
    
    temp = bcdVal; 
    temp = temp%100;   
    tens = temp/10;    
    ones = temp%10;    
    
    
    PT1AD = ones;  //lower bits
    
    PTP = tens;  //upper bits
    
}

void BCDBar (int bcdVal) 
{ 
   if(bcdVal == 1) 
   {
      PT1AD = 0b00000001;
      PTP = 0b00000000;
   } 
   else if(bcdVal == 2)  
   {
      PT1AD = 0b00000011;
      PTP = 0b00000000;
   }
   else if(bcdVal == 3)  
   {
      PT1AD = 0b00000111;
      PTP = 0b00000000;
   }
   else if(bcdVal == 4)  
   {
      PT1AD = 0b00001111;
      PTP = 0b00000000;
   }
   else if (bcdVal == 5) 
   {
      PT1AD = 0b00001111;
      PTP = 0b00000001;
   }
   else if(bcdVal == 6)  
   {
      PT1AD = 0b00001111;
      PTP = 0b00000011;
   }
   else if(bcdVal == 7)  
   {
      PT1AD = 0b00001111;
      PTP = 0b00000111;
   }
   else if(bcdVal == 8)  
   {
      PT1AD = 0b00001111;
      PTP = 0b00001111;
   }
   else if(bcdVal == 9)  
   {
      PT1AD = 0b00001111;
      PTP = 0b00011111;
   }
   else 
   {
      PT1AD = 0x00;
      PTP = 0x00;
   }    
}