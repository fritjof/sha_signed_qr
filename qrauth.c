#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <stddef.h>                   // data type definitions
#include "qrencode.h"
#include "sha204_lib_return_codes.h"  // declarations of function return codes
#include "sha204_comm_marshaling.h"   // definitions and declarations for the Command module
#include "base64_enc.h"
#include "bitmaps.h"
#include <avr/interrupt.h>
#include <avr/wdt.h>

typedef unsigned char u8;
typedef unsigned u16;

#define BAUD 9600 // Serial baud rate for scanner communication
#define BAUD_TOL 2   // defaults to 2%, warns if F_CPU/BAUD don't work together


// SPI communication with display
static void sendspiblock(u8 *buf, u16 len)
{
    volatile u8 x;
    while (len--) {
        SPDR = *buf++;
        while (!(SPSR & 0x80));
        SPSR &= 0x7f;
        x = SPDR;
    }
}

static void csact() {
    PORTB &= ~4;
}

static void csinact() {
    PORTB |= 4;
}

static void spihwinit()
{
    DDRB = 0x2c;                // set port for SPI
    PORTB |= 4;
    SPCR = 0x56;                // /32 (0x52 /64)
    SPSR = 0;
}

// Serial communication functions (for scanner communication)
#include <util/setbaud.h>

static void uart_init(void) {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */ 
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

#if 0
static void uart_putchar(char c) {
    loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
    UDR0 = c;
}
#endif

char uart_getchar(void) {
    loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
    return UDR0;
}


/** \brief This function evaluates a function return code
 *         and puts the device to sleep if the return code
 *         indicates that the device is awake.
 * \param[in] ret_code return code of the last call to a SHA204 library function
 */
void evaluate_ret_code(uint8_t ret_code)
{
	if ((ret_code == SHA204_PARSE_ERROR)
				|| (ret_code == SHA204_CMD_FAIL)
				|| (ret_code == SHA204_RX_FAIL))
		// We got some kind of response. Return codes of
		// SHA204_PARSE_ERROR and SHA204_CMD_FAIL indicate
		// a consistent response whereas SHA204_RX_FAIL
		// just indicates that we received some bytes,
		// possibly garbled. In all these cases we put
		// the device to sleep.
		(void) sha204p_sleep();
}

uint8_t ascii_to_hex(char input) {
	if(input == '0') return 0x00;
	else if(input == '1') return 0x01;
	else if(input == '2') return 0x02;
	else if(input == '3') return 0x03;
	else if(input == '4') return 0x04;
	else if(input == '5') return 0x05;
	else if(input == '6') return 0x06;
	else if(input == '7') return 0x07;
	else if(input == '8') return 0x08;
	else if(input == '9') return 0x09;
	else if(input == 'A') return 0x0A;
	else if(input == 'B') return 0x0B;
	else if(input == 'C') return 0x0C;
	else if(input == 'D') return 0x0D;
	else if(input == 'E') return 0x0E;
	else if(input == 'F') return 0x0F;
	else return 0x00;
}
unsigned int timer_cnt = 0;
// Timer interrupt service routine 
ISR(TIMER1_OVF_vect) {
	timer_cnt++;
	if(timer_cnt > 20) {
		//cli(); // disable interrupts 
  		//wdt_enable(WDTO_15MS); // enable watchdog 
  		//while(1); // wait for watchdog to reset processor 
	}
}

#include <string.h>
#define spi_transfer(x) {unsigned char ttt=(x);sendspiblock(&ttt,1);}

int main()
{
	/*
	cli();
	// Set up the timer
	TIMSK1=0x01; // enabled global and timer overflow interrupt;
	TCCR1A = 0x00; // normal operation page 148 (mode0);
	TCNT1=0x0000; // 16bit counter register
	TCCR1B = 0x04; // start timer/ set clock
	*/

	unsigned i, k, t, j;
    
	// Initialize SPI interface for display communication	
	spihwinit();

	// Initialize uart to read from scanner
	uart_init();

	// Read QR code from serial port
	volatile char scan_in_buff[64];

	// Display initial screen!
	csact();
    spi_transfer(0x00); // TX command
    spi_transfer(0x00); // Address high byte
    spi_transfer(0x00); // Address low byte
    for(int i = 0; i < 4800; i ++) {
		spi_transfer(pgm_read_byte((&bmp1[i])));	
	}

    csinact();
  	_delay_ms(5); // Or you could poll the BUSY pin, whatever floats your boat


	csact(); // Screen update
  	spi_transfer(0x18);
  	spi_transfer(0x00);
  	spi_transfer(0x00);
  	csinact();

	while(1) {
	
	for(i = 0; i < 64 ; i ++) {
		scan_in_buff[i] = uart_getchar();
	}

	csact();
    spi_transfer(0x00); // TX command
    spi_transfer(0x00); // Address high byte
    spi_transfer(0x00); // Address low byte
    for(int i = 0; i < 4800; i ++) {
		spi_transfer(pgm_read_byte((&bmp2[i])));	
	}

    csinact();
  	_delay_ms(5); // Or you could poll the BUSY pin, whatever floats your boat


	csact(); // Screen update
  	spi_transfer(0x18);
  	spi_transfer(0x00);
  	spi_transfer(0x00);
  	csinact();
	
	volatile uint8_t nounce_challenge_buff[32];
	for (i = 0; i < sizeof(nounce_challenge_buff); i++)
		nounce_challenge_buff[i] = 0;

	j = 0;
	for(i = 0; i < 32; i ++) {		
		nounce_challenge_buff[i] = (ascii_to_hex(scan_in_buff[j])<<4) | ascii_to_hex(scan_in_buff[j+1]);
		j = j + 2;
	}

	// declared as "volatile" for easier debugging
	volatile uint8_t ret_code;

	uint8_t l; // 8-bit (unsigned)

	// Make the response buffer the size of a MAC response.
	static uint8_t response[MAC_RSP_SIZE];

	volatile uint8_t tx_buff[SHA204_CMD_SIZE_MAX]; // ((uint8_t) 84)
	volatile uint8_t rx_buff[SHA204_CMD_SIZE_MAX]; // ((uint8_t) 84)

	// Initialize the hardware interface..
	sha204p_init();

	// Wake up the device.
	ret_code = sha204c_wakeup(&response[0]);
	if (ret_code != SHA204_SUCCESS) {
		continue;
	}	

	for (l = 0; l < sizeof(rx_buff); l++)
		rx_buff[l] = 0;

	volatile uint8_t serial[9];
	// Read serial number from SHA chip
	ret_code = sha204m_read((uint8_t *) tx_buff, (uint8_t *) rx_buff, ((uint8_t)0x00), ((uint16_t)0x00));
	if (ret_code != SHA204_SUCCESS) {
		continue;
	}
	serial[0] = rx_buff[1];
	serial[1] = rx_buff[2];
	serial[2] = rx_buff[3];
	serial[3] = rx_buff[4];
	/*for (l = 0; l < sizeof(rx_buff); l++)
		rx_buff[l] = 0;*/

	// Read serial number from SHA chip
	ret_code = sha204m_read((uint8_t *) tx_buff, (uint8_t *) rx_buff, ((uint8_t)0x00), ((uint16_t)0x02));
	if (ret_code != SHA204_SUCCESS) {
		continue;
	}

	serial[4] = rx_buff[1];
	serial[5] = rx_buff[2];
	serial[6] = rx_buff[3];
	serial[7] = rx_buff[4];

	// Read serial number from SHA chip
	ret_code = sha204m_read((uint8_t *) tx_buff, (uint8_t *) rx_buff, ((uint8_t)0x00), ((uint16_t)0x03));
	if (ret_code != SHA204_SUCCESS) {
		continue;
	}

	serial[8] = rx_buff[1];

	// Send nounce to device
	ret_code = sha204m_nonce((uint8_t *) tx_buff, (uint8_t *) rx_buff, NONCE_MODE_PASSTHROUGH, (uint8_t *) nounce_challenge_buff);

	if (ret_code != SHA204_SUCCESS) {
		evaluate_ret_code(ret_code);
		continue;
	}

	for (l = 0; l < sizeof(rx_buff); l++)
		rx_buff[l] = 0;

	// Execute MAC command 
	ret_code = sha204m_hmac((uint8_t *) tx_buff, (uint8_t *) rx_buff, ((uint8_t) 0x44), ((uint16_t)0x00));
		
	if (ret_code != SHA204_SUCCESS) {
		evaluate_ret_code(ret_code);
		continue;
	}

	// Put device to sleep.
 	ret_code = sha204p_sleep();

	PORTC &=~ _BV(PC3);

    // Write QR code to display
	volatile char tosnd[56];

	for (l = 0; l < sizeof(tosnd); l++)
		tosnd[l] = 0;

	// Shift contents of rx_buff one left
	for (i=0; i < 33; i ++) {
		rx_buff[i] = rx_buff[i+1];
	}	

	// Add serial number
	for (i=0; i < 9; i ++) {
		rx_buff[i+32] = serial[i];	
	}

	base64enc((char *) tosnd,(uint8_t *) rx_buff, ((uint16_t)0x29));

	/*char hexval[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	for(int j = 0; j < 32; j++){
		tosnd[j*2] = hexval[((rx_buff[j+1] >> 4) & 0xF)];
		tosnd[(j*2) + 1] = hexval[(rx_buff[j+1]) & 0x0F];
	}*/
	//strcpy((char *)strinbuf, "XRxeXi48BKU5IFmPb6/lpTdRIunjaMSaDfMMyGCL/hcBIzs5BZda7u4=");
	strcpy((char *)strinbuf, tosnd);
	
    qrencode();
    _delay_ms(1200); // Or you could poll the BUSY pin, whatever floats your boat
    csact();
    spi_transfer(0x24)
    csinact();
    _delay_ms(1200); // Or you could poll the BUSY pin, whatever floats your boat

    t = 0;
    unsigned char b = 0;
    for ( k = 0; k < 160 ; k++) { // 240
        csact();
        i = k * 30; // 40
        spi_transfer(0x00); // TX command
        spi_transfer((i>>8)); // Address high byte
        spi_transfer(i); // Address low byte
        for ( i = 0; i < 240 ; i++) { // 320
            b <<= 1;
            int x = (i - 14)/4; // (i - 60)/6
            int y = (k - 14)/4; // (k - 18)/6
            if( x >= 0 && y >= 0 && x < WD && y < WD )
                if( QRBIT(x,y) )
                    b |= 1;
            if( ++t > 7 ) {
                t = 0;
                spi_transfer((~b));
                b = 0;
            }
        }
        csinact();
        _delay_ms(5); // Or you could poll the BUSY pin, whatever floats your boat
    }

    csinact();
  	_delay_ms(5); // Or you could poll the BUSY pin, whatever floats your boat


	csact(); // Screen update
  	spi_transfer(0x18);
  	spi_transfer(0x00);
  	spi_transfer(0x00);
  	csinact();

	

	}
	//return (int) ret_code;	
}
