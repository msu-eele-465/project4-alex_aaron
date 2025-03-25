#include <msp430.h>

int i;
unsigned char dataIn;
int Space;
int Data;                       // Data is used to received data from buffer

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    UCB0CTLW0 |= UCSWRST;       // put into software reset

    UCB0CTLW0 |= UCMODE_3;      // put into I2C mode
    UCB0CTLW0 &= ~UCMST;        // put into slave mode
    UCB0I2COA0 = 0x0013;        // LCD slave address set to 0x13
    UCB0I2COA0 |= UCOAEN;       // slave address enable
    UCB0CTLW0 &= ~UCTR;         // put into receive mode
    UCB0CTLW1 &= ~UCSWACK;      // auto acknowledge
    UCB0TBCNT = 0x01;           // send 1 byte of data

    UCB0CTLW1 &= ~UCTXIFG0;

    P1SEL1 &= ~BIT3;            // P1.3 as SCL
    P1SEL0 |= BIT3;
    P1SEL1 &= ~BIT2;            // P1.2 as SDA
    P1SEL0 |= BIT2;

    //setting the upper bits of port 1 as output
    P1DIR |= BIT7;
    P1DIR |= BIT6;
    P1DIR |= BIT5;
    P1DIR |= BIT4;

    P1OUT &= ~BIT7;
    P1OUT &= ~BIT6;
    P1OUT &= ~BIT5;
    P1OUT &= ~BIT4;

    P2DIR |= BIT6;              // P2.6 as RS port
    P2OUT &= ~BIT6;
    P2DIR |= BIT7;              // P2.7 as Enable Port
    P2OUT &= ~BIT7;

    P2DIR |= BIT0;              //Green LED
    P2OUT &= ~BIT0;

    PM5CTL0 &= ~LOCKLPM5;       // disable digital I/O low-power default

    init();                     // call init function
    Command(0x00);              // clear LCD display

    UCB0CTLW0 &= ~UCSWRST;      // put out of reset

    UCB0IE |= UCRXIE0;          // ENABLE I2C RX IRQ

    __enable_interrupt();       // global enable

    while(1){
    }

    return 0;
}

void Command(unsigned char SendCom){
    P1OUT &= 0x0F;              // clear upper bits (output)
    dataIn = SendCom;
    dataIn &= 0xF0;             // clear lower bits of data
    P1OUT |= dataIn;            // set P1 to new data
    P2OUT &= ~BIT6;             // send Command mode
    Nybble();                   // call nybble function

    dataIn = SendCom;
    dataIn= dataIn<<4;          // move lower bits up
    dataIn &= 0xF0;             // clear lower bits of data
    P1OUT &= 0x0F;              // clear upper bits (output)
    P1OUT |= dataIn;            // set P1 to new data
    Nybble();                   // call nybble function
}

void Write(unsigned char SendWR){
    P1OUT &= 0x0F;              // clear upper bits (output)
    dataIn = SendWR;
    dataIn &= 0xF0;             // clear lower bits of data
    P1OUT |= dataIn;            // set P1 to new data
    P2OUT |= BIT6;              // send Write mode
    Nybble();                   // call nybble function

    dataIn = SendWR;
    dataIn = dataIn << 4;       // move lower bits up
    dataIn &= 0xF0;             // clear lower bits of data
    P1OUT &= 0x0F;              // clear upper bits (output)
    P1OUT |= dataIn;            // set P1 to new data
    Nybble();                   // call nybble function
}

// enable delay
void Nybble(){
    P2OUT |= BIT7;
    Delay(1);
    P2OUT &= ~BIT7;

}

void init(){
    Delay(100);
    P1OUT |= BIT5;              // set to 4-bit mode
    P1OUT |= BIT4;
    Delay(30);
    Nybble();                   // Wake up Command
    Delay(10);
    Nybble();
    Delay(10);
    Nybble();
    Delay(10);
    P1OUT &= ~BIT4;             // clear the output for the mode
    Nybble();
    Command(0x28);              // 4-bit/2-line mode
    Command(0x10);              // Set cursor
    Command(0x0F);              // display is on and cursor is blinking
    Command(0x06);              // set to entry mode
}

int Delay(int CountLong) {
    while(CountLong > 0) {
        CountLong--;
        InnerDelay();
    }
    return 0;
}


int InnerDelay() {
    int CountShort;
    for(CountShort = 0; CountShort < 102; CountShort++) {}
    return 0;
}

#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void){
    UCB0IE &= ~UCRXIE0;
    Data = UCB0RXBUF;           // receive data from buffer
    if(Space == 32){
        Command(0x01);          // clear LCD display
        Delay(200);
        Space=1;                // set the cursor to position 32
        Write(Data);            // send Data to Write function
        Delay(200);
    }
    else if(Space == 16){
        Command(0xC0);          // set the second line address
        Space++;
        Write(Data);            // send Data to Write function
        Delay(200);
    }
    else if(Data==0x23){        // if # is pressed, clear display
        Command(0x01);          // clear LCD display
        P2OUT &= ~BIT0;
        Delay(200);
        Space=1;                // set the cursor to position 32
        Write(Data);            // send Data to Write function
        Delay(200);

    }
    else{
        P2OUT |= BIT0;
        Delay(200);
        Write(Data);            // send Data to Write function
        Space++;
    }
    Delay(200);
    UCB0IE |= UCRXIE0;
    }
