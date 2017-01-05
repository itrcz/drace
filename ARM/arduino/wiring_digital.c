/*
  Copyright (c) 2014 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "wiring_digital.h"
#include "WVariant.h"

#ifdef __cplusplus
 extern "C" {
#endif

int pinPeripheral( uint32_t ulPin, EPioType ulPeripheral )
{
  if ( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN )
  {
    return -1 ;
  }

  switch ( ulPeripheral )
  {
    case PIO_DIGITAL:
    case PIO_INPUT:
    case PIO_INPUT_PULLUP:
    case PIO_OUTPUT:
      // Disable peripheral muxing, done in pinMode
      // Configure pin mode, if requested
      if ( ulPeripheral == PIO_INPUT )
      {
        pinMode( ulPin, INPUT ) ;
      }
      else
      {
        if ( ulPeripheral == PIO_INPUT_PULLUP )
        {
          pinMode( ulPin, INPUT_PULLUP ) ;
        }
        else
        {
          if ( ulPeripheral == PIO_OUTPUT )
          {
            pinMode( ulPin, OUTPUT ) ;
          }
          else
          {
              // do notting
          }
        }
      }
    break ;

    case PIO_ANALOG:
    case PIO_SERCOM:
    case PIO_SERCOM_ALT:
    case PIO_TIMER:
    case PIO_TIMER_ALT:
    case PIO_EXTINT:
    case PIO_COM:
    case PIO_AC_CLK:
#if 0
      // Is the pio pin in the lower 16 ones?
      // The WRCONFIG register allows update of only 16 pin max out of 32
      if ( g_APinDescription[ulPin].ulPin < 16 )
      {
        PORT->Group[g_APinDescription[ulPin].ulPort].WRCONFIG.reg = PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_PMUX( ulPeripheral ) |
                                                                    PORT_WRCONFIG_WRPINCFG |
                                                                    PORT_WRCONFIG_PINMASK( g_APinDescription[ulPin].ulPin ) ;
      }
      else
      {
        PORT->Group[g_APinDescription[ulPin].ulPort].WRCONFIG.reg = PORT_WRCONFIG_HWSEL |
                                                                    PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_PMUX( ulPeripheral ) |
                                                                    PORT_WRCONFIG_WRPINCFG |
                                                                    PORT_WRCONFIG_PINMASK( g_APinDescription[ulPin].ulPin - 16 ) ;
      }
#else
      if ( g_APinDescription[ulPin].ulPin & 1 )
      {
        uint32_t temp ;

        // Get whole current setup for both odd and even pins and remove odd one
        temp = (PORT->Group[g_APinDescription[ulPin].ulPort].PMUX[g_APinDescription[ulPin].ulPin >> 1].reg) & PORT_PMUX_PMUXE( 0xF ) ;
        // Set new muxing
        PORT->Group[g_APinDescription[ulPin].ulPort].PMUX[g_APinDescription[ulPin].ulPin >> 1].reg = temp|PORT_PMUX_PMUXO( ulPeripheral ) ;
        // Enable port mux
        PORT->Group[g_APinDescription[ulPin].ulPort].PINCFG[g_APinDescription[ulPin].ulPin].reg |= PORT_PINCFG_PMUXEN ;
      }
      else
      {
        uint32_t temp ;

        temp = (PORT->Group[g_APinDescription[ulPin].ulPort].PMUX[g_APinDescription[ulPin].ulPin >> 1].reg) & PORT_PMUX_PMUXO( 0xF ) ;
        PORT->Group[g_APinDescription[ulPin].ulPort].PMUX[g_APinDescription[ulPin].ulPin >> 1].reg = temp|PORT_PMUX_PMUXE( ulPeripheral ) ;
        PORT->Group[g_APinDescription[ulPin].ulPort].PINCFG[g_APinDescription[ulPin].ulPin].reg |= PORT_PINCFG_PMUXEN ; // Enable port mux
      }
#endif
    break ;

    case PIO_NOT_A_PIN:
      return -1l ;
    break ;
  }

  return 0l ;
}

void pinMode( uint32_t ulPin, uint32_t ulMode )
{
  if ( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN )
  {
    return ;
  }

  switch ( ulMode )
  {
    case INPUT:
      // Set pin to input mode
      PORT->Group[g_APinDescription[ulPin].ulPort].PINCFG[g_APinDescription[ulPin].ulPin].reg=(uint8_t)(PORT_PINCFG_INEN) ;
      PORT->Group[g_APinDescription[ulPin].ulPort].DIRCLR.reg = (uint32_t)(1<<g_APinDescription[ulPin].ulPin) ;
    break ;

    case INPUT_PULLUP:
      // Set pin to input mode with pull-up resistor enabled
      PORT->Group[g_APinDescription[ulPin].ulPort].PINCFG[g_APinDescription[ulPin].ulPin].reg=(uint8_t)(PORT_PINCFG_INEN|PORT_PINCFG_PULLEN) ;
      PORT->Group[g_APinDescription[ulPin].ulPort].DIRCLR.reg = (uint32_t)(1<<g_APinDescription[ulPin].ulPin) ;
	  
	  //pull-upconfig
	  PORT->Group[g_APinDescription[ulPin].ulPort].OUTSET.reg = (uint32_t)(1<<g_APinDescription[ulPin].ulPin) ;	
    break ;

    case OUTPUT:
      // Set pin to output mode
		  PORT->Group[g_APinDescription[ulPin].ulPort].PINCFG[g_APinDescription[ulPin].ulPin].reg&=~(uint8_t)(PORT_PINCFG_INEN) ;
		  PORT->Group[g_APinDescription[ulPin].ulPort].DIRSET.reg = (uint32_t)(1<<g_APinDescription[ulPin].ulPin) ;
	  
	  break ;

    default:
      // do nothing
    break ;
  }
}

void digitalWrite( uint32_t ulPin, uint32_t ulVal )
{
  /* Handle the case the pin isn't usable as PIO */
  if ( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN )
  {
    return ;
  }

  // Enable pull-up resistor
  PORT->Group[g_APinDescription[ulPin].ulPort].PINCFG[g_APinDescription[ulPin].ulPin].reg=(uint8_t)(PORT_PINCFG_PULLEN) ;

  switch ( ulVal )
  {
    case LOW:
      PORT->Group[g_APinDescription[ulPin].ulPort].OUTCLR.reg = (1ul << g_APinDescription[ulPin].ulPin) ;
    break ;

    case HIGH:
      PORT->Group[g_APinDescription[ulPin].ulPort].OUTSET.reg = (1ul << g_APinDescription[ulPin].ulPin) ;
    break ;

    default:
	  PORT->Group[g_APinDescription[ulPin].ulPort].OUTSET.reg = (1ul << g_APinDescription[ulPin].ulPin) ;
    break ;
  }

  return ;
}

int digitalRead( uint32_t ulPin )
{
  /* Handle the case the pin isn't usable as PIO */
  if ( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN )
  {
    return LOW ;
  }

  if ( (PORT->Group[g_APinDescription[ulPin].ulPort].IN.reg & (1ul << g_APinDescription[ulPin].ulPin)) != 0 )
  {
    return HIGH ;
  }

  return LOW ;
}

#ifdef __cplusplus
}
#endif

