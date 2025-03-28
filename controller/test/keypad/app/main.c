#include "msp430fr2355.h"
#include <msp430.h>
#include <stdbool.h>

/**
    PROJECT 4: Keypad test code
 
    Aaron McLean    EELE 465
 
    Last Updated: 03/25/2025
 
    A test program that read from the keypad
 */

// Global Variables
bool locked;
char key = 'x';
int keyflag = 0;

// Intiating the Keypad IO
void keypad_init() {

    // Configure pin for lockout Indicator

    // configure LED_PIN as output
    // Start the PWM Pin high

    //  Set Port 3 bits 0-3 as Outputs (row)
    P3DIR |= BIT0;
    P3DIR |= BIT1;
    P3DIR |= BIT2;
    P3DIR |= BIT3;

    P3OUT &= ~BIT0;
    P3OUT &= ~BIT1;
    P3OUT &= ~BIT2;
    P3OUT &= ~BIT3;


    // Set Port 5 bits 0-3 as Inputs (Col)
    P5DIR &= ~BIT0;
    P5DIR &= ~BIT1;
    P5DIR &= ~BIT2;
    P5DIR &= ~BIT3;

    // enable resistors
    P5REN |= BIT0;
    P5REN |= BIT1;
    P5REN |= BIT2;
    P5REN |= BIT3;

    // set as pull up resistors
    P5OUT |= BIT0;
    P5OUT |= BIT1;
    P5OUT |= BIT2;
    P5OUT |= BIT3;

    // setup hi->low interrupt trigger 
    P5IES |= BIT0;
    P5IES |= BIT1;
    P5IES |= BIT2;
    P5IES |= BIT3;


    // turn on GPIO
    PM5CTL0 &= ~LOCKLPM5;
}

int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    // Setup Keypad and other Inits
    keypad_init();

    // setup timer B
    /*
    TB0CTL |= TBCLR;            // clear timers and dividers
    TB0CTL |= TBSSEL__ACLK;     // source=ACLK
    TB0CTL |= MC__UP;           // mode=UP

    TB0CCR0 = 32768;
    TB0CCR1 = 1638;
    */


    // setup timer compare IRQs for CCR 1 and 0

    /*
    TB0CCTL0 |= CCIE;           // enable TB0 CCR0 overflow IRQ
    TB0CCTL0 &= ~CCIFG;         // clear CCR0 Flag
    TB0CCTL1 |= CCIE;           // enable TB1 CCR0 overflow IRQ
    TB0CCTL1 &= ~CCIFG;         // clear CCR1 Flag
    */

    // Keypad IRQs
    // clear the IRQ Flags
    P3IFG &= BIT0;
    P3IFG &= BIT1;
    P3IFG &= BIT2;
    P3IFG &= BIT3;
    // Enable IRQs
    P4IE |= BIT0;
    P4IE |= BIT1;
    P4IE |= BIT2;
    P4IE |= BIT3;
    
    __enable_interrupt();

    // set lockout
    locked = true;

    while(1){        
        check_combo();
        while(!locked){
/*
            poll keypad
            if different{
                change to new pattern
            }
            display pattern
*/
            locked = true;
        }

    }

    return 0;
}

// Check Combination Lock
int check_combo(void){
    int num = 0;
    char combo[4];
    while(num < 4){
        // if a button press is flagged incriment count
        if(keyflag == 1){
            combo[i] = key;
            num++;
            keyflag = 0;
        }
    }
    // if correct code unlock the keypad
    if(combo_compare(combo)){
        locked = false;
    }
}

// compare combination to the preset lock combination
bool combo_compare(char usr_combo[4]){
    char combo[] = {'1','2','3','4'}
    if(combo[0]==usr_combo[0] && combo[1]==usr_combo[1] && combo[2]==usr_combo[2] && combo[3]==usr_combo[3]){
        return true;
    }
    return false;
}


char check_colmn(int row){

    // Disable the Column Resistors
    P5REN &= ~BIT0;
    P5REN &= ~BIT1;
    P5REN &= ~BIT2;
    P5REN &= ~BIT3;

    // Set Columns to outputs
    P5DIR |= BIT0;
    P5DIR |= BIT1;
    P5DIR |= BIT2;
    P5DIR |= BIT3;

    // Set Columns Low
    P5OUT &= ~BIT0;
    P5OUT &= ~BIT1;
    P5OUT &= ~BIT2;
    P5OUT &= ~BIT3;

    // Set Rows as inputs
    P3DIR &= ~BIT0;
    P3DIR &= ~BIT1;
    P3DIR &= ~BIT2;
    P3DIR &= ~BIT3;

    // (ROW) enable resistors
    P3REN |= BIT0;
    P3REN |= BIT1;
    P3REN |= BIT2;
    P3REN |= BIT3;

    // (ROW) set as pull up resistors
    P3OUT |= BIT0;
    P3OUT |= BIT1;
    P3OUT |= BIT2;
    P3OUT |= BIT3;

    // Poll for Column
    


    /*
    set colmn outputs low
    disable the colmn outputs
    disable the colmn resistors
    disable the row inputs

    enable colmn inputs
    enable row resistors
    enable row outputs
    set the row outputs high

    check for colmn

    set the row outputs low
    disable the row outputs
    disable the row resistors
    disable the colmn inputs

    enable the row inputs
    enable the colmn resistors
    enable the colmn outputs
    set the colmn outputs high

    return the character
    */

    char button = 'x';


    // given the row, determine the colmn, and set the character
    if(row == 1){
        if(/*Col 1*/){
            button = '1';
        }else if(/*Col 2*/){
            button = '2';
        }else if(/*Col 3*/){
            button = '3';
        }else if(/*Col 4*/){
            button = 'A';
        }else{
            button = 'x';
        }
    }else if(row == 2){
        if(/*Col 1*/){
            button = '4';
        }else if(/*Col 2*/){
            button = '5';
        }else if(/*Col 3*/){
            button = '6';
        }else if(/*Col 4*/){
            button = 'B';
        }else{
            button = 'x';
        }
    }else if(row ==3){
        if(/*Col 1*/){
            button = '7';
        }else if(/*Col 2*/){
            button = '8';
        }else if(/*Col 3*/){
            button = '9';
        }else if(/*Col 4*/){
            button = 'C';
        }else{
            button = 'x';
        }
    }else if(row == 4){
        if(/*Col 1*/){
            button = '*';
        }else if(/*Col 2*/){
            button = '0';
        }else if(/*Col 3*/){
            button = '#';
        }else if(/*Col 4*/){
            button = 'D';
        }else{
            button = 'x';
        }
    }



    // temporary return value
    return 'x';
}

// -------------------------------------------------------------------
// Interrupt Service Routines
// -------------------------------------------------------------------

// CCR0 ISR -- 10 ms
#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0(void){
    P1OUT |= BIT0;
    TB0CCTL0 &= ~CCIFG;
}

// CCR1 ISR -- 5ms
#pragma vector = TIMER0_B1_VECTOR
__interrupt void ISR_TB0_CCR1(void){
    P1OUT &= ~BIT0;
    TB0CCTL1 &= ~CCIFG;
}

// P3.0 ISR --Row1
#pragma vector = PORT3_VECTOR
__interrupt void ISR_Port3_S0(void){
    
    //Call function to determine column
    key = check_colmn(1);

    P3IFG &= ~BIT0;
}

// P3.1 ISR --Row2
#pragma vector = PORT3_VECTOR
__interrupt void ISR_Port3_S1(void){
    

    P3IFG &= ~BIT1;
}

// P3.2 ISR --Row3
#pragma vector = PORT3_VECTOR
__interrupt void ISR_Port3_S2(void){
    

    P3IFG &= ~BIT2;
}

// P3.3 ISR --Row4
#pragma vector = PORT3_VECTOR
__interrupt void ISR_Port3_S3(void){
    

    P3IFG &= ~BIT3;
}
