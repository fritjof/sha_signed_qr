// ----------------------------------------------------------------------------
//         ATMEL Microcontroller Software Support  -  Colorado Springs, CO -
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
 *  \brief  Definitions for Configurable Values of the SHA204 Library
 *
 *          This file contains several library configuration sections
 *          for the three interfaces the library supports
 *          (SWI using GPIO or UART, and I2C) and one that is common
 *          to all interfaces.
 *  \author Atmel Crypto Products
 *  \date 	February 2, 2011
 */
#ifndef SHA204_CONFIG_H
#   define SHA204_CONFIG_H

#include <stddef.h>                    // data type definitions

//////////////////////////////////////////////////////////////////////////
////////// definitions common to all interfaces //////////////////////////
//////////////////////////////////////////////////////////////////////////

/** \brief maximum CPU clock deviation to higher frequency (crystal etc.)
 * This value is used to establish time related worst case numbers, for
 * example to calculate execution delays and timeouts.
 */
#define CPU_CLOCK_DEVIATION_POSITIVE   (1.01)

/** \brief maximum CPU clock deviation to lower frequency (crystal etc.)
 * This value is used to establish time related worst case numbers, for
 * example to calculate execution delays and timeouts.
 */
#define CPU_CLOCK_DEVIATION_NEGATIVE   (0.99)

/** \brief number of command / response retries
 *
 * If communication is lost, re-synchronization includes waiting for the
 * longest possible execution time of a command.
 * This adds a #SHA204_COMMAND_EXEC_MAX delay to every retry.
 * Every increment of the number of retries increases the time
 * the library is spending in the retry loop by #SHA204_COMMAND_EXEC_MAX.
 */
#define SHA204_RETRY_COUNT           (1)



/** \brief For I2C, the response polling time is the time
 *         it takes to send the I2C address.
 *
 *         This value is used to timeout when waiting for a response.
 */

#define SHA204_RESPONSE_TIMEOUT     ((uint16_t) 37)





#endif
