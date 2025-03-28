#include <msp430fr2310.h>
#include <stdbool.h>

int data;

int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    UCB0CTLW0   |=  UCSWRST;    // put into reset

    UCB0CTLW0   |= UCSSEL_3;    // setting up SMCLK
    UCB0BRW = 10;               // dividing by 10

    UCB0CTLW0   |=  UCMODE_3;   // put into I2C mode
    UCB0CTLW0   |=  UCMST;      // put into master mode
    UCB0CTLW0   |=  UCTR;       // put into send (TX) mode

    UCB0CTLW1   |=  UCASTP_2;   // auto stop
    UCB0TBCNT   =   0x01;       // send 1 byte of data

    P1SEL1  &=  ~BIT3;          // P1.3 as SCL
    P1SEL0  |=  BIT3;
    P1SEL1  &=  ~BIT2;          // P1.2 as SDA
    P1SEL0  |=  BIT2;

    // Port 2 as an output
    P3DIR = 0xFF;
    P3OUT = 0x00;

    // Disable low-power mode / GPIO high-impedance
    PM5CTL0 &= ~LOCKLPM5;

    UCB0CTLW0 &= ~UCSWRST;      // put out of reset

    UCB0IE |= UCRXIE0;          // ENABLE I2C RX IRQ

    __enable_interrupt();       // global enable

    while (true)
    {
        P1OUT ^= BIT0;

        // Delay for 100000*(1/MCLK)=0.1s
        __delay_cycles(100000);
    }
}

void handleButtonPress(int button) {
  int j;
  switch (button) {
    case 1: // if A is pressed
        P3OUT = 0xAA;
    break;
    case 2: // if B is pressed
        for (j = 0; j <= 255; j++) {
            P3OUT = j;
            delay();
            delay();
        }
    break;
    case 3: // if C is pressed
        P3OUT = 0xFE;
        for (j = 0; j < 8; j++) {
            delay(); delay(); delay(); delay();
            P3OUT = (P3OUT << 1) | 0x01;
        }
        P3OUT = 0xFE;
    break;
    case 4: // if D is pressed
        P3OUT = 0x18;
        delay(); delay();
        P3OUT = 0x00;
        delay();
        P3OUT = 0x24;
        delay(); delay();
        P3OUT = 0x00;
        delay();
        P3OUT = 0x42;
        delay(); delay();
        P3OUT = 0x00;
        delay();
        P3OUT = 0x81;
        delay(); delay();
        P3OUT = 0x00;
        delay();
        P3OUT = 0x42;
        delay(); delay();
        P3OUT = 0x00;
        delay();
        P3OUT = 0x24;
        delay(); delay();
        P3OUT = 0x00;
        delay();
        P3OUT = 0x18;
    break;
    case 5: // if 3 is pressed
        P3OUT = 0x03;
    break;
    case 6: // if 6 is pressed
        P3OUT = 0x06;
    break;
    case 7: // if 9 is pressed
        P3OUT = 0x09;
    break;
    case 8: // if # is pressed
        P3OUT = 0x00;
        delay(); delay();
        P3OUT = 0x73;
        delay(); delay();
        P3OUT = 0x00;
        delay(); delay();
        P3OUT = 0x6F;
        delay(); delay();
        P3OUT = 0x00;
        delay(); delay();
        P3OUT = 0x6F;
        delay(); delay();
        P3OUT = 0x00;
        delay(); delay();
        P3OUT = 0x73;
        delay(); delay();
        P3OUT = 0x00;
    break;
    case 9: // if 2 is pressed
        P3OUT = 0x02;
    break;
    case 10: // if 5 is pressed
        P3OUT = 0x05;
    break;
    case 11: // if 8 is pressed
        P3OUT = 0x08;
    break;
    case 12: // if 0 is pressed
        P3OUT = 0x00;
    break;
    case 13: // if 1 is pressed
        P3OUT = 0x01;
    break;
    case 14: // if 4 is pressed
        P3OUT = 0x04;
    break;
    case 15: // if 7 is pressed
        P3OUT = 0x07;
    break;
    case 16: // if * is pressed
        P3OUT = 0xFF;
    break;
  }
}

#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void){
    UCB0IE &= ~UCRXIE0;
    Data = UCB0RXBUF;           // receive data from buffer
    handleButtonPress(Data);
}