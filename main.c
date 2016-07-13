#include <avr/io.h>
#include <util/delay.h>

#define NUM_COLS 16
#define NUM_ROWS 8

// COL PINS:
// PB3 DATA
// PB4 LATCH
// PB5 CLOCK

// ROW PINS:
// PB0 DATA
// PB1 LATCH
// PB2 ClOCK


void initIO(void) {
	//uint8_t i = 0;
    for (uint8_t i = 0; i < 6; i++) {
        DDRB |= (1 << i);
    }
    // TIMER INIT:
    TCCR0A = 0; // set registers to 0
    TCCR0B = 0;
    TCNT0 = 0; // initialize timer counter
    TCCR0B |= (1 << CS02)|(1 << CS00); // initialize timer 0 with 1024 bit prescaler
    //sei();
}

void eShiftOut(volatile uint8_t *DS_PORT, uint8_t DS_PIN, volatile uint8_t *STCP_PORT, uint8_t STCP_PIN,
               volatile uint8_t *SHCP_PORT, uint8_t SHCP_PIN, uint8_t dataOut) { // AVR SHIFTOUT FUNCTION MADE BY ME HANDLING MSB
    *DS_PORT = (0 << DS_PIN);
    *SHCP_PORT = (0 << SHCP_PIN); // clock pin
	//uint8_t i = 0;
    for (uint8_t i = 0; i < 8; i++) {
        *SHCP_PORT = (0 << SHCP_PIN);
        if (dataOut & (1 << i)) {
            *DS_PORT = (1 << DS_PIN); // write the bit
        } else {
            *DS_PORT = (0 << DS_PIN); // write the bit
        }
        *SHCP_PORT = (1 << SHCP_PIN); // shift to next bit by raising clock pin to high
        *DS_PORT = (0 << DS_PIN); // return DS_PIN to zero for next bit to be written
    }
    *SHCP_PORT = (0 << SHCP_PIN); // stop shifting by making clock pin LOW
}

void resetMatrix(void) { // resets matrix by turning it off
    PORTB = (0 << PB4);
    eShiftOut(&PORTB, PB3, &PORTB, PB4, &PORTB, PB5, 0b00000000);
    PORTB = (1 << PB4);
    PORTB = (0 << PB1);
    eShiftOut(&PORTB, PB0, &PORTB, PB1, &PORTB, PB2, 0b00000000);
    PORTB = (1 << PB1);
}

void mPoint(uint8_t x, uint8_t y) { // will light up an LED on the matrix at the specified coordinates
    PORTB = (0 << PB4);
    PORTB = (0 << PB1);
    eShiftOut(&PORTB, PB3, &PORTB, PB4, &PORTB, PB5, x);
    eShiftOut(&PORTB, PB0, &PORTB, PB1, &PORTB, PB2, y);
    PORTB = (1 << PB4);
    PORTB = (1 << PB1);
}

void displayChar(uint8_t charMap[8]) {
	//uint8_t i = 0;
    for (uint8_t i = 0; i < 8; i++) {
        uint8_t row = 255;
        row &= ~(1 << (7 - i)); // to ensure MSB is processed first as well as flipping bits in the row to 0 so the columns being displayed are displayed on the correct row
        mPoint(charMap[i], row);
    }
}

//ISR(TIMER0_OVF_vect) {
//    
//}

int main(void) {
    uint8_t overflows = 0;
    uint8_t letter = 7;
    initIO();
    uint8_t E[8] = {0b11111111, 0b10000000, 0b10000000, 0b11111111, 0b11111111, 0b10000000, 0b10000000, 0b11111111};
    uint8_t T[8] = {0b11111111, 0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b00011000};
    uint8_t H[8] = {0b10000001, 0b10000001, 0b10000001, 0b11111111, 0b11111111, 0b10000001, 0b10000001, 0b10000001};
    uint8_t A[8] = {0b01111110, 0b10000001, 0b10000001, 0b10000001, 0b11111111, 0b10000001, 0b10000001, 0b10000001};
    uint8_t N[8] = {0b10000001, 0b11000001, 0b10100001, 0b10010001, 0b10001001, 0b10000101, 0b10000011, 0b10000001};
    while(1) {
        if (TCNT0 >= 124) {
            overflows++;
            TCNT0 = 0;
            if (overflows == 25) {
                overflows = 0;
				//uint8_t i = 0;
                for (uint8_t i = 0; i < 8; i++) {
                    E[i] &= ~(1 << letter);
                }
                letter--;
            }
        }
        displayChar(E);
    }
    return 0;
}
