/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : McuGenericSWSPI.h
**     Project     : FRDM-K64F_Generator
**     Processor   : MK64FN1M0VLL12
**     Component   : GenericSWSPI
**     Version     : Component 01.031, Driver 01.15, CPU db: 3.00.000
**     Compiler    : GNU C Compiler
**     Date/Time   : 2020-08-13, 18:42, # CodeGen: 675
**     Abstract    :
**
**     Settings    :
**     Contents    :
**         RecvChar              - uint8_t McuGenericSWSPI_RecvChar(uint8_t *Chr);
**         SendChar              - uint8_t McuGenericSWSPI_SendChar(uint8_t val);
**         CharsInRxBuf          - uint8_t McuGenericSWSPI_CharsInRxBuf(void);
**         CharsInTxBuf          - uint8_t McuGenericSWSPI_CharsInTxBuf(void);
**         SetShiftClockPolarity - uint8_t McuGenericSWSPI_SetShiftClockPolarity(uint8_t Edge);
**         SetIdleClockPolarity  - uint8_t McuGenericSWSPI_SetIdleClockPolarity(uint8_t Level);
**         Write_ReadDummy       - void McuGenericSWSPI_Write_ReadDummy(uint8_t val);
**         SetSlowMode           - void McuGenericSWSPI_SetSlowMode(void);
**         SetFastMode           - void McuGenericSWSPI_SetFastMode(void);
**         Deinit                - void McuGenericSWSPI_Deinit(void);
**         Init                  - void McuGenericSWSPI_Init(void);
**
** * Copyright (c) 2013-2020, Erich Styger
**  * Web:         https://mcuoneclipse.com
**  * SourceForge: https://sourceforge.net/projects/mcuoneclipse
**  * Git:         https://github.com/ErichStyger/McuOnEclipse_PEx
**  * All rights reserved.
**  *
**  * Redistribution and use in source and binary forms, with or without modification,
**  * are permitted provided that the following conditions are met:
**  *
**  * - Redistributions of source code must retain the above copyright notice, this list
**  *   of conditions and the following disclaimer.
**  *
**  * - Redistributions in binary form must reproduce the above copyright notice, this
**  *   list of conditions and the following disclaimer in the documentation and/or
**  *   other materials provided with the distribution.
**  *
**  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**  * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**  * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**  * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**  * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**  * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**  * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**  * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**  * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
** ###################################################################*/
/*!
** @file McuGenericSWSPI.h
** @version 01.15
** @brief
**
*/         
/*!
**  @addtogroup McuGenericSWSPI_module McuGenericSWSPI module documentation
**  @{
*/         


/* MODULE McuGenericSWSPI. */
#include "McuGenericSWSPI.h"

/* Include inherited components */
#include "Clock1.h"
#include "Input1.h"
#include "Output1.h"
#include "McuWait.h"
#include "McuLib.h"

  /* data-clock-clock */

#define OVERRUN_ERR  0x01              /* Overrun error flag bit   */
#define CHAR_IN_RX   0x08              /* Char is in RX buffer     */

static uint8_t McuGenericSWSPI_FastMode; /* 0: slow, 1: fast */
#define MOSI_IDLE_POLARITY    1        /* MOSI idle polarity is high */
#define CLOCK_IDLE_POLARITY   0        /* Clock idle polarity is low */

static uint8_t CLKshift;
static uint8_t CLKsampl;
static uint8_t InputBuffer;
static uint8_t SerFlag;                /* Flags for serial communication */
                                       /* Bits: 0 - OverRun error */
                                       /*       1 - Unused */
                                       /*       2 - Unused */
                                       /*       3 - Char in RX buffer */
                                       /*       4 - Unused */
                                       /*       5 - Unused */
                                       /*       6 - Unused */
                                       /*       7 - Unused */
#define McuGenericSWSPI_DELAY() \
    if (McuGenericSWSPI_FastMode) { /* we can change the speed at runtime */ \
      McuWait_Waitus(0);               /* delay for some cycles */ \
    } else { \
      McuWait_Waitus(0);               /* delay for some cycles */ \
    }
/*
** ===================================================================
**     Method      :  SetFastMode (component GenericSWSPI)
**
**     Description :
**         Sets the fast mode
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void McuGenericSWSPI_SetFastMode(void)
{
  McuGenericSWSPI_FastMode = 1;
}

/*
** ===================================================================
**     Method      :  SetSlowMode (component GenericSWSPI)
**
**     Description :
**         Changes to slow mode
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void McuGenericSWSPI_SetSlowMode(void)
{
  McuGenericSWSPI_FastMode = 0;
}

/*
** ===================================================================
**     Method      :  RecvChar (component GenericSWSPI)
**
**     Description :
**         If any data received, this method returns one character,
**         otherwise it returns an error code (it does not wait for
**         data).
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Chr             - A pointer to the received character.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_RXEMPTY - No data in receiver
**                           ERR_OVERRUN - Overrun error is detected
** ===================================================================
*/
uint8_t McuGenericSWSPI_RecvChar(uint8_t *Chr)
{
  if(!(SerFlag & CHAR_IN_RX)) {        /* Is char. received */
    return ERR_RXEMPTY;
  }
  *Chr = InputBuffer;                  /* Store the character */
  if(SerFlag&OVERRUN_ERR) {            /* Is "overrun" occurred? */
    SerFlag &= ~(OVERRUN_ERR|CHAR_IN_RX); /* If yes, clear flags */
    return ERR_OVERRUN;                /* ... and return error */
  } else {
    SerFlag &= ~CHAR_IN_RX;            /* If no, clear flag */
    return ERR_OK;                     /* ... and return */
  }
}

/*
** ===================================================================
**     Method      :  Write_ReadDummy (component GenericSWSPI)
**
**     Description :
**         Writes to the bus, but does not read.
**     Parameters  :
**         NAME            - DESCRIPTION
**         val             - value to put on the bus
**     Returns     : Nothing
** ===================================================================
*/
void McuGenericSWSPI_Write_ReadDummy(uint8_t val)
{
  int i;

  for(i=0; i<8; i++) {
    McuGenericSWSPI_DELAY();
    Output1_PutVal((bool)(val&128));   /* Set value on MOSI */
    InputBuffer <<= 1;
    McuGenericSWSPI_DELAY();
    Clock1_PutVal(CLKsampl);           /* Set CLK to sample value */
    val <<= 1;
    Clock1_PutVal(CLKshift);           /* Set CLK to shift value */
  }
  Output1_PutVal((bool)MOSI_IDLE_POLARITY); /* Set value on MOSI */
}

/*
** ===================================================================
**     Method      :  SendChar (component GenericSWSPI)
**
**     Description :
**         Sends one character to the channel.
**     Parameters  :
**         NAME            - DESCRIPTION
**         val             - Character to send.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
** ===================================================================
*/
uint8_t McuGenericSWSPI_SendChar(uint8_t val)
{
  int i;

  for(i=0; i<8; i++) {
    McuGenericSWSPI_DELAY();
    Output1_PutVal((bool)(val&128));   /* Set value on MOSI */
    InputBuffer <<= 1;
    McuGenericSWSPI_DELAY();
    Clock1_PutVal(CLKsampl);           /* Set CLK to sample value */
    InputBuffer |= Input1_GetVal()?1:0; /* Read value from MISO */
    val <<= 1;
    Clock1_PutVal(CLKshift);           /* Set CLK to shift value */
  }
  Output1_PutVal((bool)MOSI_IDLE_POLARITY); /* Set value on MOSI */
  if(SerFlag&CHAR_IN_RX) {             /* Is char. received? */
    SerFlag |= OVERRUN_ERR;            /* If yes then set "overrun" flag */
  } else {
    SerFlag |= CHAR_IN_RX;             /* If no then set "char in Rx" flag */
  }
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  CharsInRxBuf (component GenericSWSPI)
**
**     Description :
**         Returns number of characters in the input buffer.
**     Parameters  : None
**     Returns     :
**         ---             - Returns number of characters in the input
**                           buffer.
** ===================================================================
*/
uint8_t McuGenericSWSPI_CharsInRxBuf(void)
{
  return (uint8_t)((SerFlag & CHAR_IN_RX)?(uint8_t)1:(uint8_t)0); /* Return number of chars in receive buffer */
}

/*
** ===================================================================
**     Method      :  CharsInTxBuf (component GenericSWSPI)
**
**     Description :
**         Returns number of characters in the output buffer.
**     Parameters  : None
**     Returns     :
**         ---             - Returns number of characters in the output
**                           buffer.
** ===================================================================
*/
#if 0 /* implemented as macro in the header file */
uint8_t McuGenericSWSPI_CharsInTxBuf(void)
{
  return 0;                            /* Return number of chars in transmit buffer */
}
#endif
/*
** ===================================================================
**     Method      :  SetShiftClockPolarity (component GenericSWSPI)
**
**     Description :
**         Sets the shift clock polarity at runtime. Output data will
**         be shifted on the selected edge polarity.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Edge            - Edge polarity, possible values:
**                           0-falling edge,
**                           1-rising edge.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
** ===================================================================
*/
uint8_t McuGenericSWSPI_SetShiftClockPolarity(uint8_t Edge)
{
  CLKshift = (uint8_t)(Edge?(uint8_t)1:(uint8_t)0); /* Set shift value */
  CLKsampl = (uint8_t)(CLKshift^(uint8_t)0x01); /* Set sample value */
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  SetIdleClockPolarity (component GenericSWSPI)
**
**     Description :
**         Set the idle clock polarity at runtime. If communication
**         does not run, the clock signal will be set to the required
**         level.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Level           - Idle clock polarity:
**                           0-low
**                           1-high
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
** ===================================================================
*/
uint8_t McuGenericSWSPI_SetIdleClockPolarity(uint8_t Level)
{
  Clock1_PutVal((uint8_t)Level);       /* Set CLK to (new) idle value */
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  Init (component GenericSWSPI)
**
**     Description :
**         Driver Initialization
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void McuGenericSWSPI_Init(void)
{
#if McuLib_CONFIG_SDK_VERSION_USED != McuLib_CONFIG_SDK_PROCESSOR_EXPERT
  Input1_Init();
  Output1_Init();
  Clock1_Init();
#endif
  McuGenericSWSPI_SetSlowMode();       /* slow mode is default */
  /* clock idle low, falling edge: Data - Sample - Shift */
  CLKsampl = 1;
  CLKshift = 0;
  Clock1_PutVal((bool)CLOCK_IDLE_POLARITY); /* Set CLK to idle value */
  Output1_PutVal((bool)MOSI_IDLE_POLARITY); /* Set value on MOSI */
  SerFlag = 0;                         /* Clear flags */
}

/*
** ===================================================================
**     Method      :  Deinit (component GenericSWSPI)
**
**     Description :
**         Driver Deinitialization
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void McuGenericSWSPI_Deinit(void)
{
#if McuLib_CONFIG_SDK_VERSION_USED != McuLib_CONFIG_SDK_PROCESSOR_EXPERT
  Input1_Deinit();
  Output1_Deinit();
  Clock1_Deinit();
#endif
}


/* END __McuGenericSWSPI_H */

/*!
** @}
*/
