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
volatile bool locked;
volatile unsigned char key = 'x';
volatile unsigned int keyflag = 0;

// Intiating the Keypad IO
void keypad_init() {
    // Configure Port 3 Rows as outputs (Row)
    P3DIR |= (BIT0 | BIT1 | BIT2 | BIT3);
    P3OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3);

    // Configure Port 5 Columns as inputs (Col)
    P5DIR &= ~(BIT0 | BIT1 | BIT2 | BIT3);
    P5REN |= (BIT0 | BIT1 | BIT2 | BIT3); // Enable pull-up/pull-down
    P5OUT |= (BIT0 | BIT1 | BIT2 | BIT3); // Set pull-ups

    // Setup Hi-to-Low interrupt trigger
    P3IES |= (BIT0 | BIT1 | BIT2 | BIT3);
    P3IFG &= ~(BIT0 | BIT1 | BIT2 | BIT3); // Clear pending flags
    P3IE |= (BIT0 | BIT1 | BIT2 | BIT3);   // Enable interrupts

    // Setup Locked and Unlocked LEDS
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;

    P6DIR |= BIT6;
    P6OUT &= ~BIT6;

    // Unlock GPIO
    PM5CTL0 &= ~LOCKLPM5;
}

int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    // Setup Keypad and other Inits
    keypad_init();

    __enable_interrupt();

    // set lockout
    locked = true;

    while(1){  
        P1OUT = BIT0;
        P6OUT &= ~BIT6;      
        check_combo();
        while(!locked){
/*
            poll keypad
            if different{
                change to new pattern
            }
            display pattern
*/
            P6OUT |= BIT6;
            P1OUT &= ~BIT0;
            int i;
            for (i = 100; i>0; i--){}

            locked = true;
        }

    }

    return 0;
}

// Check Combination Lock
void check_combo(void){
    unsigned int num = 0;
    char combo[4];
    while(num < 4){
        // if a button press is flagged incriment count
        if(keyflag == 1){
            combo[num] = key;
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
    char combo[] = {'1','2','3','4'};
    if(combo[0]==usr_combo[0] && combo[1]==usr_combo[1] && combo[2]==usr_combo[2] && combo[3]==usr_combo[3]){
        return true;
    }
    return false;
}


char check_colmn(int row){

    // Configure columns as outputs, set low
    P5DIR |= (BIT0 | BIT1 | BIT2 | BIT3);
    P5OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3);

    // Configure rows as inputs with pull-ups
    P3DIR &= ~(BIT0 | BIT1 | BIT2 | BIT3);
    P3REN |= (BIT0 | BIT1 | BIT2 | BIT3);
    P3OUT |= (BIT0 | BIT1 | BIT2 | BIT3);

    // Poll for Column
    

    char button = 'x';


    // given the row, determine the colmn, and set the character
    // if grounded then it is the row in question
    // if P5IN & BIT0 == 0 then col 1 is it
    if(row == 1){
        if(P5IN & BIT0/*Col 1*/){
            button = '1';
        }else if(P5IN & BIT1/*Col 2*/){
            button = '2';
        }else if(P5IN & BIT2/*Col 3*/){
            button = '3';
        }else if(P5IN & BIT3/*Col 4*/){
            button = 'A';
        }else{
            button = 'x';
        }
    }else if(row == 2){
        if(P5IN & BIT0){
            button = '4';
        }else if(P5IN & BIT1){
            button = '5';
        }else if(P5IN & BIT2){
            button = '6';
        }else if(P5IN & BIT3){
            button = 'B';
        }else{
            button = 'x';
        }
    }else if(row ==3){
        if(P5IN & BIT0){
            button = '7';
        }else if(P5IN & BIT1){
            button = '8';
        }else if(P5IN & BIT2){
            button = '9';
        }else if(P5IN & BIT3){
            button = 'C';
        }else{
            button = 'x';
        }
    }else if(row == 4){
        if(P5IN & BIT0){
            button = '*';
        }else if(P5IN & BIT1){
            button = '0';
        }else if(P5IN & BIT2){
            button = '#';
        }else if(P5IN & BIT3){
            button = 'D';
        }else{
            button = 'x';
        }
    }

    // Restore GPIO settings (columns as inputs)
    P5DIR &= ~(BIT0 | BIT1 | BIT2 | BIT3);
    P5REN |= (BIT0 | BIT1 | BIT2 | BIT3);
    P5OUT |= (BIT0 | BIT1 | BIT2 | BIT3);

    // Configure rows as outputs, set low
    P3DIR |= (BIT0 | BIT1 | BIT2 | BIT3);
    P3OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3);

    // return Key Character
    return button;
}

// -------------------------------------------------------------------
// Interrupt Service Routines
// -------------------------------------------------------------------

// Keypad ISR
#pragma vector = PORT3_VECTOR
__interrupt void ISR_Port3(void) {
    if (P3IFG & BIT0) {
        key = check_colmn(1);
        P3IFG &= ~BIT0;
    } else if (P3IFG & BIT1) {
        key = check_colmn(2);
        P3IFG &= ~BIT1;
    } else if (P3IFG & BIT2) {
        key = check_colmn(3);
        P3IFG &= ~BIT2;
    } else if (P3IFG & BIT3) {
        key = check_colmn(4);
        P3IFG &= ~BIT3;
    }
    int i;
    for(i=1000;i>0;i--){}
    keyflag = 1;  // Set key flag after determining keypress
}
