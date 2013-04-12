// ----------------------------------------------------------------------------
//         ATMEL Crypto-Devices Software Support  -  Colorado Springs, CO -
// ----------------------------------------------------------------------------
// DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
// DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

/** \file
 *  \brief Implementation of I2C Hardware Dependent Functions in the
 *         Physical Layer of Crypto Libraries.
 *  \author Atmel Crypto Products
 *  \date  February 2, 2011
 */


#include <avr\io.h>       // GPIO definitions
#include <util\twi.h>     // I2C definitions
#include <avr\power.h>    // definitions for power saving register
#include "i2c_phys.h"     // definitions and declarations for the hardware dependent I2C module


/** \brief This function initializes and enables the I2C peripheral.
 * */
void i2c_enable(void)
{
	PRR &= ~_BV(PRTWI);            // Disable power saving.

#ifdef I2C_PULLUP
	//DDRC = (_BV(PC4) | _BV(PC5)); // Configure I2C as input to allow setting the pull-up resistors.
	//PORTC |= (_BV(PC4) | _BV(PC5)); // Connect pull-up resistors on TWI clock and data pins.
	// DDRD &= ~(_BV(PD0) | _BV(PD1)); // Configure I2C as input to allow setting the pull-up resistors.
	// PORTD |= (_BV(PD0) | _BV(PD1)); // Connect pull-up resistors on TWI clock and data pins.

	DDRC = (_BV(PC3) | _BV(PC4) | _BV(PC5));
	//PORTC |= (_BV(PC3) | _BV(PC4) | _BV(PC5)); 
	

	int i;
	while(1) {
		PORTC |= (_BV(PC3) | _BV(PC4) | _BV(PC5)); 
		for(i=0; i<2000; i++) {
		}
		PORTC &= ~ (_BV(PC3) | _BV(PC4) | _BV(PC5)); 

	}
#endif
	TWBR = ((uint8_t) (((double) F_CPU / I2C_CLOCK - 16.0) / 2.0 + 0.5)); // Set the baud rate
	//TWBR = (uint8_t)72; // Set the baud rate
}


/** \brief This function disables the I2C peripheral. */
void i2c_disable(void)
{
	TWCR = 0;                       // Disable TWI.
	PRR |= _BV(PRTWI);             // Enable power saving.
}


/** \brief This function creates a Start condition (SDA low, then SCL low).
 * \return status of the operation
 * */
uint8_t i2c_send_start(void)
{
	PORTC |= _BV(PC3);
	
	
	uint8_t timeout_counter = I2C_START_TIMEOUT;
	uint8_t i2c_status;

	TWCR = (_BV(TWEN) | _BV(TWSTA) | _BV(TWINT));

    // while (!(TWCR & (1<<TWINT)));
	
	// Wait for start condition to be sent
	do {
		if (timeout_counter-- == 0) {
			// Toggle debug pin
			//PORTC &=~ _BV(PC3);
			return I2C_FUNCTION_RETCODE_TIMEOUT;
		}
	} while ((TWCR & (_BV(TWINT))) == 0);

	i2c_status = TW_STATUS;
	if ((i2c_status != TW_START) && (i2c_status != TW_REP_START))
		return I2C_FUNCTION_RETCODE_COMM_FAIL;

	//PORTC &=~ _BV(PC3);
	return I2C_FUNCTION_RETCODE_SUCCESS;
}


/** \brief This function creates a Stop condition (SCL high, then SDA high).
 * \return status of the operation

 * */
uint8_t i2c_send_stop(void)
{
	uint8_t timeout_counter = I2C_STOP_TIMEOUT;

	TWCR = (_BV(TWEN) | _BV(TWSTO) | _BV(TWINT));
	do {
		if (timeout_counter-- == 0)
			return I2C_FUNCTION_RETCODE_TIMEOUT;
	} while ((TWCR & _BV(TWSTO)) > 0);

	if (TW_STATUS == TW_BUS_ERROR)
		return I2C_FUNCTION_RETCODE_COMM_FAIL;

	return I2C_FUNCTION_RETCODE_SUCCESS;
}


/** \brief This function sends bytes to an I2C device.
 * \param[in] count number of bytes to send
 * \param[in] data pointer to tx buffer
 * \return status of the operation
 */
uint8_t i2c_send_bytes(uint8_t count, uint8_t *data)
{
	uint8_t timeout_counter;
	uint8_t twi_status;
	uint8_t i;

	
	for (i = 0; i < count; i++) {
		TWDR = *data++;
		TWCR = _BV(TWEN) | _BV(TWINT);

		timeout_counter = I2C_BYTE_TIMEOUT;
		do {
			if (timeout_counter-- == 0) {
				//PORTC &=~ _BV(PC3);
				return I2C_FUNCTION_RETCODE_TIMEOUT;
			}
		} while ((TWCR & (_BV(TWINT))) == 0);

		twi_status = TW_STATUS;
		if ((twi_status != TW_MT_SLA_ACK)
					&& (twi_status != TW_MT_DATA_ACK)
					&& (twi_status != TW_MR_SLA_ACK))
			// Return error if byte got nacked.
			return I2C_FUNCTION_RETCODE_NACK;
	}

	return I2C_FUNCTION_RETCODE_SUCCESS;
}


/** \brief This function receives one byte from an I2C device.
 *
 * \param[out] data pointer to received byte
 * \return status of the operation
 */
uint8_t i2c_receive_byte(uint8_t *data)
{
	uint8_t timeout_counter = I2C_BYTE_TIMEOUT;

	// Enable acknowledging data.
	TWCR = (_BV(TWEN) | _BV(TWINT) | _BV(TWEA));
	do {
		if (timeout_counter-- == 0)
			return I2C_FUNCTION_RETCODE_TIMEOUT;
	} while ((TWCR & (_BV(TWINT))) == 0);

	if (TW_STATUS != TW_MR_DATA_ACK) {
		// Do not override original error.
		(void) i2c_send_stop();
		return I2C_FUNCTION_RETCODE_COMM_FAIL;
	}
	*data = TWDR;

	return I2C_FUNCTION_RETCODE_SUCCESS;
}


/** \brief This function receives bytes from an I2C device
 *         and sends a Stop.
 *
 * \param[in] count number of bytes to receive
 * \param[out] data pointer to rx buffer
 * \return status of the operation
 */
uint8_t i2c_receive_bytes(uint8_t count, uint8_t *data)
{
	uint8_t i;
	uint8_t timeout_counter;

	// Acknowledge all bytes except the last one.
	for (i = 0; i < count - 1; i++) {
		// Enable acknowledging data.
		TWCR = (_BV(TWEN) | _BV(TWINT) | _BV(TWEA));
		timeout_counter = I2C_BYTE_TIMEOUT;
		do {
			if (timeout_counter-- == 0)
				return I2C_FUNCTION_RETCODE_TIMEOUT;
		} while ((TWCR & (_BV(TWINT))) == 0);

		if (TW_STATUS != TW_MR_DATA_ACK) {
			// Do not override original error.
			(void) i2c_send_stop();
			return I2C_FUNCTION_RETCODE_COMM_FAIL;
		}
		*data++ = TWDR;
	}

	// Disable acknowledging data for the last byte.
	TWCR = (_BV(TWEN) | _BV(TWINT));
	timeout_counter = I2C_BYTE_TIMEOUT;
	do {
		if (timeout_counter-- == 0)
			return I2C_FUNCTION_RETCODE_TIMEOUT;
	} while ((TWCR & (_BV(TWINT))) == 0);

	if (TW_STATUS != TW_MR_DATA_NACK) {
		// Do not override original error.
		(void) i2c_send_stop();
		return I2C_FUNCTION_RETCODE_COMM_FAIL;
	}
	*data = TWDR;

	return i2c_send_stop();
}
