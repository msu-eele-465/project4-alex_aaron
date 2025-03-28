#include <msp430.h>

int DataOut = 0x24;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    UCB1CTLW0 |= UCSWRST;                   // UCSWRST=1 for eUSCI_B1 in SW reset

    // Configure eUSCI_B1
    UCB1CTLW0 |= UCSSEL__SMCLK;
    UCB1BRW = 10;
    UCB1CTLW0 |= UCMODE_3;
    UCB1CTLW0 |= USMST;
    UCB1CTLW0 |= UCTR;
    UCB1I2CSA = 20h;
    USB1CTLW1 |= UCASTP_2;
    UCB0TBCNT = 0x01;

    // Configure Ports
    P4SEL1 &= ~BIT5;                        // SCL
    P4SEL0 |= BIT5;
    P4SEL1 &= ~BIT6;                        // SDA
    P4SEL0 |= BIT6;

    PM5CTL0 &= ~LOCKLPM5;

    UCB1CTLW0 &= ~UCSWRST;

    UCB1IE |= UCTXIE0;
    __enable_interrupt();

    int i;
    while(1){
        UCB1CTLW0 |= UCTXSTT;
        for(i=0; i<100; i++){}
    }

    return 0;
}

#pragma vector=EUSCI_B1_VECTOR
__interrupt void EUSCI_B1_I2C_ISR(void){
    UCB1TXBUF = DataOut;
}