#include "msp430fr2310.h"
#include <msp430.h>

unsigned char Data_In = 0;
unsigned char Data;
int i, j;
int Space;
int count = 0;


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    
    UCB0CTLW0 |= UCSWRST;                   // Put into software reset

    // I2C Setup
    UCB0CTLW0 |= UCMODE_3;                  // Put into I2C mode
    UCB0CTLW0 &= ~UCMST;                    // Put into slave mode
    UCB0I2COA0 = 0x0013;                    // LCD slave address set to 0x13
    UCB0I2COA0 |= UC0AEN;                   // Slave address enable
    UCB0CTLW0 &= ~UCTR;                     // Put into receive mode
    UCB0CTLW1 &= ~UCSWACK;                  // Auto Acknowledge
    UCB0TBCNT = 0x01;                       // Send 1 byte of data

    UCB0CTLW0 &= ~UCTXIFG0;
    
    P1SEL1 &= ~BIT3;     // SCL
    P1SEL0 |= BIT3;
    P1SEL1 &= ~BIT2;     // SDA
    P1SEL0 |= BIT2;

    // Setup Outputs
    P2DIR |= BIT6 | BIT7;   // (RS and RW)
    P2OUT &= ~(BIT6 | BIT7);

    P1DIR |= BIT4 | BIT5 | BIT6 | BIT7;     // DB Inputs
    P1OUT &= ~(BIT4 | BIT5 | BIT6 | BIT7);

    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode

    init();                                 // call init function

    UCB0CTLW0 &= ~UCSWRST;                  // Take out of software reset

    while(1)
    {
        P1OUT ^= BIT0;                      // Toggle P1.0 using exclusive-OR
        __delay_cycles(100000);             // Delay for 100000*(1/MCLK)=0.1s
    }
}