/*
 * LEGO® MINDSTORMS EV3
 *
 * Copyright (C) 2010-2013 The LEGO Group
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


/*! \page AnalogModule Analog Input Module
 *
 *  Manages communication and controlling the A to D conversion on an input port.\n
 *
 *-  \subpage DcmDriver
 *-  \subpage AnalogModuleMemory
 *-  \subpage AnalogModuleResources
 *
 */


/*! \page DcmDriver Device Connection Manager Driver
 *
 *  Manages the detection of adding and removing different devices to an input or an output port.\n
 *
 *  The device will change some connection levels on the port when added and that way give an event to start the evaluation of the device id.
 *
 *  The evaluation state machine is timer interrupt driven at a low frequency (less than 1KHz) so the used CPU power is held at lowest possible level.\n
 *
 *
 * \n
 *
 *  Input Port
 *
 *  - From the beginning all I/O is set as input - an open port is defined as:
 *    - Value at connection 1 is higher than IN1_NEAR_5V
 *    - Level at connection 2 is high
 *    - Level at connection 5 is high
 *    - Level at connection 6 is low
 *    - Value at connection 6 is lower than IN6_NEAR_GND
 *
 *
 * \n
 *
 *  If anything changes to a steady value (for more than STEADY_TIME) it will start a state machine that will try to
 *  detect what happened.
 *
 *  When it is detected - a signal is sent to the \ref InputLibraryDeviceSetup "Input Library Device Setup" and the state will freeze in a state that only looks for
 *  an open port condition (for more than STEADY_TIME).
 *  \verbatim
*/

#define   IN_CONNECT_STEADY_TIME        350   //  [mS]  time needed to be sure that the connection is steady
#define   IN_DISCONNECT_STEADY_TIME     100   //  [mS]  time needed to be sure that the disconnection is steady

#define   IN1_NEAR_5V                   4800  //  [mV]  higher values mean that connection 1 is floating
#define   IN1_NEAR_PIN2                 3100  //  [mV]  higher values mean that connection 1 is shorted to connection 2 (5000 * 18K / (18K + 10k))
#define   IN1_TOUCH_HIGH                950   //  [mV]  values in between these limits means that an old touch sensor is connected
#define   IN1_TOUCH_LOW                 850   //  [mV]
#define   IN1_NEAR_GND                  100   //  [mV]  lower  values mean that connection 1 is shorted to connection 3
#define   IN6_NEAR_GND                  150   //  [mV]  lower  values mean that connection 6 is floating

/*

INPUT
*********************************************************************************************************************************************************************************************************************

MICRO                             CIRCUIT                               CONNECTION        NEW UART DEVICE           NEW DUMB SENSOR                 OLD SENSOR                TACHO MOTOR           NEW DUMP ACTUATOR
----------------------            ----------------------------------    ----------        ---------------------     ------------------------        --------------------      ----------------      -----------------
Analogue I                        10K pull up to ADC_REF                    1             Short circuit to ground   ID resistor to ground           Analogue value            Motor +               Motor +

Digital I                         6K4 impedance to NEAR_PIN2 voltage        2             Open                      Open                            Short to ground           Motor -               Motor -

Ground                            Ground                                    3             Ground                    Ground                          Ground                    Ground                ?

Supply                            Supply                                    4             Supply                    Supply                          Supply                    Supply                ?

Digital I/O (float)               100K pull up to 3.3V                      5             RXD (float)               Short to ground                 ?                         Tacho A               ID resistor to 5

Analogue I + Digital I/O (float)  220K pull down to ground                  6             TXD (low)                 Analogue value                  ?                         Tacho B               ID resistor to 6

*********************************************************************************************************************************************************************************************************************



ID VALUE ON CONNECTION 1:

  ADC_REF         -----
  IN1_NEAR_5V     -----
                    |
                    |
                    |   TACHO MOTOR and NEW DUMB ACTUATOR
                    |
                    |
  IN1_NEAR_PIN2   -----
                    |
                    |
                    |
                    |
                    |
                    |
                    |
                    |   NEW SENSOR ID's
                    |
                    |
                    |
                    |
                    |
                    |
                    |   NEW UART SENSOR
  0.0V            -----



IMPLEMENTED DETECTION RULES (sequence matters):

I.      Connection 2 low
            1.  Connection 5 and 6 high                                   -> NXT IIC   DEVICE
            2.  Connection 5 low                                          -> NXT LIGHT SENSOR
            3.  Connection 1 lower than IN1_NEAR_GND                      -> NXT COLOR SENSOR
            4.  Connection 1 higher than IN1_NEAR_5V                      -> NXT TOUCH SENSOR
            5.  Connection 1 between IN1_TOUCH_HIGH and IN1_TOUCH_LOW     -> NXT TOUCH SENSOR
            6.  else                                                      -> NXT SOUND SENSOR


II.     Connection 1 loaded
            1.  Connection 1 higher than IN1_NEAR_PIN2                    -> ERROR
            2.  Connection 1 lower than IN1_NEAR_GND                      -> NEW UART DEVICE
            3.  else (value on connection 1 is ID)                        -> NEW DUMB DEVICE


III.    Connection 6 high                                                 -> NXT IIC TEMP SENSOR


IV.     Connection 5 low                                                  -> ERROR



NEW ID's:

The range from 0mV to just beneath the voltage on connection 2 is free to be used for the new sensor ID's - recommendations:

I.  Use a short circuit to ground to identify the UART device.

\endverbatim
 *
 *  \n
 */

/*! \page DcmDriver
 *
 *  Output Port
 *
 *  - From the beginning motor driver is floating and all I/O is set as input - an open port is defined as:
 *    - Value at connection 5 is in between OUT5_BALANCE_LOW and OUT5_BALANCE_HIGH
 *    - Level at connection 5 is high
 *    - Level at connection 6 is high
 *
 *  If anything changes to a steady value (for more than STEADY_TIME) it will start a state machine that will try to
 *  detect what happened.
 *
 *  When it is detected - a signal is sent to the \ref InputLibraryDeviceSetup "Input Library Device Setup" and the state will freeze in a state that only looks for
 *  an open port condition (for more than STEADY_TIME).
 *  \verbatim
*/

#define   OUT5_IIC_HIGH                 3700  //  [mV]  values in between these limits means that an old IIC sensor or color sensor is connected
#define   OUT5_IIC_LOW                  2800  //  [mV]

#define   OUT5_MINITACHO_HIGH1          2000  //  [mV]  values in between these limits means that a mini tacho motor is pulling high when pin5 is pulling low
#define   OUT5_MINITACHO_LOW1           1600  //  [mV]

#define   OUT5_BALANCE_HIGH             2600  //  [mV]  values in between these limits means that connection 5 is floating
#define   OUT5_BALANCE_LOW              2400  //  [mV]

#define   OUT5_LIGHT_HIGH                850  //  [mV]  values in between these limits means that an old light sensor is connected
#define   OUT5_LIGHT_LOW                 650  //  [mV]

#define   OUT5_MINITACHO_HIGH2           450  //  [mV]  values in between these limits means that a mini tacho motor is pulling low when pin5 floats
#define   OUT5_MINITACHO_LOW2            250  //  [mV]

#define   OUT5_NEAR_GND                  100  //  [mV]  lower  values mean that connection 5 is shorted to ground

/*


OUTPUT
*********************************************************************************************************************************************************************************************************************

MICRO                             CIRCUIT                               CONNECTION        NEW UART DEVICE           NEW DUMB SENSOR                 OLD SENSOR                TACHO MOTOR           NEW DUMP ACTUATOR
----------------------            -------------------------------       ----------        ---------------------     ------------------------        --------------------      ----------------      -----------------
Motor driver +                    Connected to motor driver                 1             ID resistor to ground     ID resistor to ground           Analogue value            Motor +               Motor +

Motor driver -                    100K pull up to ADC_REF                   2             Open                      Open                            Short to ground           Motor -               Motor -

Ground                            Ground                                    3             Ground                    Ground                          Ground                    Ground                ?

Supply                            Supply                                    4             Supply                    Supply                          Supply                    Supply                ?

Analogue I + Digital I/O          50K impedance to OUT5_BALANCE             5             RXD (float)               Short to ground                 ?                         Tacho A               ID resistor to 5

Digital I/O (low)                 100K pull up to connection 2              6             TXD (low)                 Analogue value                  ?                         Tacho B               ID resistor to 6

*********************************************************************************************************************************************************************************************************************



ID VALUE ON CONNECTION 5:

  Connection 6 floating                                                                                       Connection 6 low and value on connection 5 changed
  ---------------------------------------------------------------------------------------                     --------------------------------------------------

  ADC_REF           -------------------------------------------------------------------------------------------------
  NEAR_5V           -----                                           |
                      |                                             |
                      |                                             |
  3.3               - | - - - - - - - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - - - - - - - - - - - -   2014 ADC reference voltage
                      |     LARGE TACHO MOTOR                       -
                      |     MINI TACHO MOTOR, NEW DUMB MOTOR        -     OLD TACHO MOTOR
                      |     NEW TACHO MOTOR                         -
  OUT5_IIC_HIGH     -----                                           |
                      |     OLD IIC SENSOR                          -
  OUT5_IIC_LOW      -----                                           |
                      |                                             |
  OUT5_BALANCE_HIGH -----                                           |
                      |     OPEN                                    -
  OUT5_BALANCE_LOW  -----                                           |
                      |                                             |                                           -----
                      |                                             |                                             |
                      |                                             |                                             |
                      |                                             |                                             |     NEW ACTUATOR ID's
                      |                                             |                                             |
                      |                                             |                                             |
                      |                                             |                                           -----
  OUT5_LIGHT_HIGH   -----                                           |
                      |     OLD LIGHT SENSOR                        -
  OUT5_LIGHT_LOW    -----                                           |
                      |     NEW TACHO MOTOR                         -
                      |     MINI TACHO MOTOR                        -
                      |     LARGE TACHO MOTOR                       -
  OUT5_NEAR_GND     -----                                         -----
                      |     NEW DUMB SENSOR
  0.0V              -------------------------------------------------------------------------------------------------

When something is connected: the value on connection 5 is stored as Value5Float. Connection 6 is driven low and the value on connection 5 is then stored as Value5Low.
Connection 6 is set floating again.
The two stored values is used to determine the type of the attached device.


IMPLEMENTED DETECTION RULES (sequence matters):

I.      Value5Float is equal to Value5Low
            1.  Value5Float is between OUT5_BALANCE_LOW and OUT5_BALANCE_HIGH and Connection 6 is low   -> ERROR        (NXT TOUCH SENSOR, NXT SOUND SENSOR, NEW UART SENSOR)
            2.  Value5Float is lower than OUT5_NEAR_GND                                                 -> ERROR        (NEW DUMP SENSOR)
            3.  Value5Float is between OUT5_LIGHT_LOW and OUT5_LIGHT_HIGH                               -> ERROR        (OLD IIC SENSOR)
            4.  Value5Float is between OUT5_IIC_LOW and OUT5_IIC_HIGH                                   -> ERROR        (OLD TEMP SENSOR)
            5.  Value5Float is lower than OUT5_BALANCE_LOW
                    Value5Float is higher than OUT5_MINITACHO_HIGH2                                     -> NEW TACHO
                    Value5Float is higher than OUT5_MINITACHO_LOW2                                      -> MINI TACHO   (NEW MINI TACHO MOTOR)
                    else                                                                                -> TACHO MOTOR
            6.  Set connection 5 low and measure new Value5Low
            7.  VALUE5Low is lower than OUT5_MINITACHO_LOW1                                             -> NEW TACHO    (NEW MINI TACHO MOTOR)
                    VALUE5Low is lower than OUT5_MINITACHO_HIGH1                                        -> MINI TACHO   (NEW MINI TACHO MOTOR)
                    else                                                                                -> TACHO MOTOR

II.     Value5Float is NOT equal to Value5Low
            1.  Value5Low is between OUT5_NEAR_GND and OUT5_BALANCE_LOW                                 -> NEW DUMP ACTUATOR  (ID is Value5Low)
            2.  else                                                                                    -> ERROR



NEW ID's:

The range from 800mV to OUT5_BALANCE_LOW is free to be used for the new sensor ID's - recommendations:

I.    Use a short circuit between connection 5 and connection 6 to identify the cheapest device

II.   Remember to reserve one id for the smart actuator (maybe the highest resistance = 500K)

III.  Remember that the ID resistor must get connection 5 out of balance

IV.   Remember the voltage on pin 1 when connected to an input port


\endverbatim
 *
 *  \n
 */


#ifndef PCASM
#include  <asm/types.h>
#include  <linux/time.h>
#endif

#define   HW_ID_SUPPORT

#include  "../../lms2012/source/lms2012.h"
#include  "../../lms2012/source/am1808.h"

#ifdef    DEBUG_D_ANALOG
#define   DEBUG
#endif


int       Hw = 0;


enum      InputPortPins
{
  INPUT_PORT_PIN1,
  INPUT_PORT_PIN2,
  INPUT_PORT_PIN5,
  INPUT_PORT_PIN6,
  INPUT_PORT_BUF,
  INPUT_PORT_PINS,
  INPUT_PORT_VALUE
};


enum      InputSpiPins
{
  ADCMOSI,
  ADCMISO,
  ADCCLK,
  ADCCS,
  ADC_SPI_PINS
};


enum      OutputPortPins
{
  OUTPUT_PORT_PIN1,
  OUTPUT_PORT_PIN2,
  OUTPUT_PORT_PIN5W,
  OUTPUT_PORT_PIN5R,
  OUTPUT_PORT_PIN6,
  OUTPUT_PORT_PINS,
  OUTPUT_PORT_VALUE
};


enum      AdcPowerPins
{
  ADCONIGEN,
  ADCBATEN,
  ADC_POWER_PINS
};


#define   INPUTADCPORTS   12
#define   INPUTADCPOWERS  4
#define   INPUTADC        (INPUTADCPORTS + INPUTADCPOWERS)

#define   NO_OF_INPUT_PORTS             INPUTS
#define   NO_OF_OUTPUT_PORTS            OUTPUTS

static    void InputPortFloat(int Port);

INPIN     InputPortPin[NO_OF_INPUT_PORTS][INPUT_PORT_PINS];

INPIN     OutputPortPin[NO_OF_OUTPUT_PORTS][OUTPUT_PORT_PINS];

INPIN     AdcPowerPin[ADC_POWER_PINS];


/*! \page AnalogModuleMemory
 *
 *  <hr size="1"/>
 *  <b>     Map physical ADC channels to logical channels in shared memory </b>
 *
 *  The mapping is done by following array of physical channels
 *  \verbatim
 *
 */
static    const UBYTE InputReadMap[INPUTADC] = { 6,8,10,12,5,7,9,11,1,0,13,14,2,15,3,4 };

/*  \endverbatim
 *  \n
 */


/*! \page AnalogModuleResources Gpios and Resources used for Module
 *
 *  Describes use of gpio and resources\n
 *
 *  \verbatim
 */


//  EP2


INPIN     EP2_InputPortPin[][INPUT_PORT_PINS] =
{
  { // Input port 1
    { GP8_10 , NULL, 0 }, // Pin 1  - I_ONA           - 9V enable (high)
    { GP2_2  , NULL, 0 }, // Pin 2  - LEGDETA         - Digital input pulled up
    { GP0_2  , NULL, 0 }, // Pin 5  - DIGIA0          - Digital input/output
    { GP0_15 , NULL, 0 }, // Pin 6  - DIGIA1          - Digital input/output
    { GP8_11 , NULL, 0 }, // Buffer disable
  },
  { // Input port 2
    { GP8_12 , NULL, 0 }, // Pin 1  - I_ONB           - 9V enable (high)
    { GP8_15 , NULL, 0 }, // Pin 2  - LEGDETB         - Digital input pulled up
    { GP0_14 , NULL, 0 }, // Pin 5  - DIGIB0          - Digital input/output
    { GP0_13 , NULL, 0 }, // Pin 6  - DIGIB1          - Digital input/output
    { GP8_14 , NULL, 0 }, // Buffer disable
  },
  { // Input port 3
    { GP8_9  , NULL, 0 }, // Pin 1  - I_ONC           - 9V enable (high)
    { GP7_11 , NULL, 0 }, // Pin 2  - LEGDETC         - Digital input pulled up
    { GP0_12 , NULL, 0 }, // Pin 5  - DIGIC0          - Digital input/output
    { GP1_14 , NULL, 0 }, // Pin 6  - DIGIC1          - Digital input/output
    { GP7_9  , NULL, 0 }, // Buffer disable
  },
  { // Input port 4
    { GP6_4  , NULL, 0 }, // Pin 1  - I_OND           - 9V enable (high)
    { GP7_8  , NULL, 0 }, // Pin 2  - LEGDETD         - Digital input pulled up
    { GP0_1  , NULL, 0 }, // Pin 5  - DIGID0          - Digital input/output
    { GP1_15 , NULL, 0 }, // Pin 6  - DIGID1          - Digital input/output
    { GP7_10 , NULL, 0 }, // Buffer disable
  },
};


INPIN     EP2_OutputPortPin[][OUTPUT_PORT_PINS] =
{
  { // Output port 1
    { GP3_15 , NULL, 0 }, // Pin 1  - MAIN0
    { GP3_6  , NULL, 0 }, // Pin 2  - MAIN1
    { GP5_4  , NULL, 0 }, // Pin 5  - DETA0 TP18
    { GP5_11 , NULL, 0 }, // Pin 5  - INTA0
    { GP0_4  , NULL, 0 }, // Pin 6  - DIRA
  },
  { // Output port 2
    { GP0_3  , NULL, 0 }, // Pin 1  - MBIN0
    { GP2_1  , NULL, 0 }, // Pin 2  - MBIN1
    { GP2_5  , NULL, 0 }, // Pin 5  - DETB0 TP19
    { GP5_8  , NULL, 0 }, // Pin 5  - INTB0
    { GP2_9  , NULL, 0 }, // Pin 6  - DIRB
  },
  { // Output port 3
    { GP6_8  , NULL, 0 }, // Pin 1  - MCIN0
    { GP5_9  , NULL, 0 }, // Pin 2  - MCIN1
    { GP3_8  , NULL, 0 }, // Pin 5  - DETC0 TP20
    { GP5_13 , NULL, 0 }, // Pin 5  - INTC0
    { GP3_14 , NULL, 0 }, // Pin 6  - DIRC
  },
  { // Output port 4
    { GP5_10 , NULL, 0 }, // Pin 1  - MDIN0
    { GP5_3  , NULL, 0 }, // Pin 2  - MDIN1
    { GP5_15 , NULL, 0 }, // Pin 5  - DETD0 TP21
    { GP6_9  , NULL, 0 }, // Pin 5  - INTD0
    { GP2_8  , NULL, 0 }, // Pin 6  - DIRD
  },
};


INPIN     EP2_AdcPowerPin[ADC_POWER_PINS] =
{
  { GP6_14 , NULL, 0 }, // 5VONIGEN
  { GP0_6  , NULL, 0 }, // ADCBATEN
};


//  FINALB


INPIN     FINALB_InputPortPin[][INPUT_PORT_PINS] =
{
  { // Input port 1
    { GP8_10 , NULL, 0 }, // Pin 1  - I_ONA           - 9V enable (high)
    { GP2_2  , NULL, 0 }, // Pin 2  - LEGDETA         - Digital input pulled up
    { GP0_2  , NULL, 0 }, // Pin 5  - DIGIA0          - Digital input/output
    { GP0_15 , NULL, 0 }, // Pin 6  - DIGIA1          - Digital input/output
    { GP8_11 , NULL, 0 }, // Buffer disable
  },
  { // Input port 2
    { GP8_12 , NULL, 0 }, // Pin 1  - I_ONB           - 9V enable (high)
    { GP8_15 , NULL, 0 }, // Pin 2  - LEGDETB         - Digital input pulled up
    { GP0_14 , NULL, 0 }, // Pin 5  - DIGIB0          - Digital input/output
    { GP0_13 , NULL, 0 }, // Pin 6  - DIGIB1          - Digital input/output
    { GP8_14 , NULL, 0 }, // Buffer disable
  },
  { // Input port 3
    { GP8_9  , NULL, 0 }, // Pin 1  - I_ONC           - 9V enable (high)
    { GP7_11 , NULL, 0 }, // Pin 2  - LEGDETC         - Digital input pulled up
    { GP0_12 , NULL, 0 }, // Pin 5  - DIGIC0          - Digital input/output
    { GP1_14 , NULL, 0 }, // Pin 6  - DIGIC1          - Digital input/output
    { GP7_9  , NULL, 0 }, // Buffer disable
  },
  { // Input port 4
    { GP6_4  , NULL, 0 }, // Pin 1  - I_OND           - 9V enable (high)
    { GP7_8  , NULL, 0 }, // Pin 2  - LEGDETD         - Digital input pulled up
    { GP1_13 , NULL, 0 }, // Pin 5  - DIGID0          - Digital input/output
    { GP1_15 , NULL, 0 }, // Pin 6  - DIGID1          - Digital input/output
    { GP7_10 , NULL, 0 }, // Buffer disable
  },
};


INPIN     FINALB_OutputPortPin[][OUTPUT_PORT_PINS] =
{
  { // Output port 1
    { GP0_3  , NULL, 0 }, // Pin 1  - MAIN0
    { GP4_12 , NULL, 0 }, // Pin 2  - MAIN1
    { GP5_4  , NULL, 0 }, // Pin 5  - DETA0 TP18
    { GP5_11 , NULL, 0 }, // Pin 5  - INTA0
    { GP0_4  , NULL, 0 }, // Pin 6  - DIRA
  },
  { // Output port 2
    { GP3_15 , NULL, 0 }, // Pin 1  - MBIN0
    { GP3_6  , NULL, 0 }, // Pin 2  - MBIN1
    { GP2_5  , NULL, 0 }, // Pin 5  - DETB0 TP19
    { GP5_8  , NULL, 0 }, // Pin 5  - INTB0
    { GP2_9  , NULL, 0 }, // Pin 6  - DIRB
  },
  { // Output port 3
    { GP5_10 , NULL, 0 }, // Pin 1  - MCIN0
    { GP5_3  , NULL, 0 }, // Pin 2  - MCIN1
    { GP3_2  , NULL, 0 }, // Pin 5  - DETC0 TP20
    { GP5_13 , NULL, 0 }, // Pin 5  - INTC0
    { GP3_14 , NULL, 0 }, // Pin 6  - DIRC
  },
  { // Output port 4
    { GP6_8  , NULL, 0 }, // Pin 1  - MDIN0
    { GP5_9  , NULL, 0 }, // Pin 2  - MDIN1
    { GP5_15 , NULL, 0 }, // Pin 5  - DETD0 TP21
    { GP6_9  , NULL, 0 }, // Pin 5  - INTD0
    { GP2_8  , NULL, 0 }, // Pin 6  - DIRD
  },
};


INPIN     FINALB_AdcPowerPin[ADC_POWER_PINS] =
{
  { GP6_14 , NULL, 0 }, // 5VONIGEN
  { GP0_6  , NULL, 0 }, // ADCBATEN
};


//  FINAL


INPIN     FINAL_InputPortPin[][INPUT_PORT_PINS] =
{
  { // Input port 1
    { GP8_10 , NULL, 0 }, // Pin 1  - I_ONA           - 9V enable (high)
    { GP2_2  , NULL, 0 }, // Pin 2  - LEGDETA         - Digital input pulled up
    { GP0_2  , NULL, 0 }, // Pin 5  - DIGIA0          - Digital input/output
    { GP0_15 , NULL, 0 }, // Pin 6  - DIGIA1          - Digital input/output
    { GP8_11 , NULL, 0 }, // Buffer disable
  },
  { // Input port 2
    { GP8_12 , NULL, 0 }, // Pin 1  - I_ONB           - 9V enable (high)
    { GP8_15 , NULL, 0 }, // Pin 2  - LEGDETB         - Digital input pulled up
    { GP0_14 , NULL, 0 }, // Pin 5  - DIGIB0          - Digital input/output
    { GP0_13 , NULL, 0 }, // Pin 6  - DIGIB1          - Digital input/output
    { GP8_14 , NULL, 0 }, // Buffer disable
  },
  { // Input port 3
    { GP8_9  , NULL, 0 }, // Pin 1  - I_ONC           - 9V enable (high)
    { GP7_11 , NULL, 0 }, // Pin 2  - LEGDETC         - Digital input pulled up
    { GP0_12 , NULL, 0 }, // Pin 5  - DIGIC0          - Digital input/output
    { GP1_14 , NULL, 0 }, // Pin 6  - DIGIC1          - Digital input/output
    { GP7_9  , NULL, 0 }, // Buffer disable
  },
  { // Input port 4
    { GP6_4  , NULL, 0 }, // Pin 1  - I_OND           - 9V enable (high)
    { GP7_8  , NULL, 0 }, // Pin 2  - LEGDETD         - Digital input pulled up
    { GP1_13 , NULL, 0 }, // Pin 5  - DIGID0          - Digital input/output
    { GP1_15 , NULL, 0 }, // Pin 6  - DIGID1          - Digital input/output
    { GP7_10 , NULL, 0 }, // Buffer disable
  },
};


INPIN     FINAL_OutputPortPin[][OUTPUT_PORT_PINS] =
{
  { // Output port 1
    { GP0_3  , NULL, 0 }, // Pin 1  - MAIN0
    { GP4_12 , NULL, 0 }, // Pin 2  - MAIN1
    { GP5_4  , NULL, 0 }, // Pin 5  - DETA0 TP18
    { GP5_11 , NULL, 0 }, // Pin 5  - INTA0
    { GP0_4  , NULL, 0 }, // Pin 6  - DIRA
  },
  { // Output port 2
    { GP3_15 , NULL, 0 }, // Pin 1  - MBIN0
    { GP3_6  , NULL, 0 }, // Pin 2  - MBIN1
    { GP2_5  , NULL, 0 }, // Pin 5  - DETB0 TP19
    { GP5_8  , NULL, 0 }, // Pin 5  - INTB0
    { GP2_9  , NULL, 0 }, // Pin 6  - DIRB
  },
  { // Output port 3
    { GP5_10 , NULL, 0 }, // Pin 1  - MCIN0
    { GP5_3  , NULL, 0 }, // Pin 2  - MCIN1
    { GP3_2  , NULL, 0 }, // Pin 5  - DETC0 TP20
    { GP5_13 , NULL, 0 }, // Pin 5  - INTC0
    { GP3_14 , NULL, 0 }, // Pin 6  - DIRC
  },
  { // Output port 4
    { GP6_8  , NULL, 0 }, // Pin 1  - MDIN0
    { GP5_9  , NULL, 0 }, // Pin 2  - MDIN1
    { GP5_15 , NULL, 0 }, // Pin 5  - DETD0 TP21
    { GP6_9  , NULL, 0 }, // Pin 5  - INTD0
    { GP2_8  , NULL, 0 }, // Pin 6  - DIRD
  },
};


INPIN     FINAL_AdcPowerPin[ADC_POWER_PINS] =
{
  { GP6_14 , NULL, 0 }, // 5VONIGEN
  { GP0_6  , NULL, 0 }, // ADCBATEN
};


#ifndef   ADC_BITBANGING

INPIN     AdcSpiPin[ADC_SPI_PINS] =
{
  { SPI0_MOSI , NULL, 0 }, // ADCMOSI
  { SPI0_MISO , NULL, 0 }, // ADCMISO
  { SPI0_SCL  , NULL, 0 }, // ADCCLK
  { SPI0_CS   , NULL, 0 }  // ADCCS
};

#else

INPIN     AdcSpiPin[ADC_SPI_PINS] =
{
  { GP8_5  , NULL, 0 }, // ADCMOSI
  { GP8_6  , NULL, 0 }, // ADCMISO
  { GP1_8  , NULL, 0 }, // ADCCLK
  { GP8_2  , NULL, 0 }  // ADCCS
};

#endif


/*  \endverbatim
 *  \n
 */

#define   PUDisable                     iowrite32(ioread32(da8xx_syscfg1_base + 0x0C) & ~0xFFFFFFFF,da8xx_syscfg1_base + 0x0C)


INPIN     *pInputPortPin[] =
{
  [FINAL]     =   (INPIN*)&FINAL_InputPortPin[0],    //  FINAL   platform
  [FINALB]    =   (INPIN*)&FINALB_InputPortPin[0],   //  FINALB  platform
  [EP2]       =   (INPIN*)&EP2_InputPortPin[0],      //  EP2     platform
};


INPIN     *pOutputPortPin[] =
{
  [FINAL]     =   FINAL_OutputPortPin[0],   //  FINAL   platform
  [FINALB]    =   FINALB_OutputPortPin[0],  //  FINALB  platform
  [EP2]       =   EP2_OutputPortPin[0],     //  EP2     platform
};


INPIN     *pAdcPowerPin[] =
{
  [FINAL]     =   FINAL_AdcPowerPin,        //  FINAL   platform
  [FINALB]    =   FINALB_AdcPowerPin,       //  FINALB  platform
  [EP2]       =   EP2_AdcPowerPin,          //  EP2     platform
};


//*****************************************************************************

#define   MODULE_NAME                   "analog_module"
#define   DEVICE1_NAME                  ANALOG_DEVICE
#define   DEVICE2_NAME                  TEST_PIN_DEVICE
#define   DEVICE3_NAME                  DCM_DEVICE

static    int  ModuleInit(void);
static    void ModuleExit(void);

#define   __USE_POSIX

#include  <linux/kernel.h>
#include  <linux/fs.h>

#include  <linux/sched.h>


#ifndef   PCASM
#ifndef DISABLE_PREEMPTED_VM
#include  <linux/slab.h>
#endif
#include  <linux/mm.h>
#include  <linux/hrtimer.h>

#include  <linux/init.h>
#include  <linux/uaccess.h>
#include  <linux/debugfs.h>

#include  <linux/ioport.h>
#include  <asm/gpio.h>
#include  <asm/io.h>
#include  <linux/module.h>
#include  <linux/miscdevice.h>
#include  <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("The LEGO Group");
MODULE_DESCRIPTION(MODULE_NAME);
MODULE_SUPPORTED_DEVICE(DEVICE1_NAME);

module_init(ModuleInit);
module_exit(ModuleExit);

#else
// Keep Eclipse happy
#endif


static    void      __iomem *GpioBase;

void      SetGpio(int Pin)
{
  int     Tmp = 0;
  void    __iomem *Reg;

  if (Pin >= 0)
  {
    while ((MuxRegMap[Tmp].Pin != -1) && (MuxRegMap[Tmp].Pin != Pin))
    {
      Tmp++;
    }
    if (MuxRegMap[Tmp].Pin == Pin)
    {
      Reg   =  da8xx_syscfg0_base + 0x120 + (MuxRegMap[Tmp].MuxReg << 2);

      *(u32*)Reg &=  MuxRegMap[Tmp].Mask;
      *(u32*)Reg |=  MuxRegMap[Tmp].Mode;

      if (Pin < NO_OF_GPIOS)
      {
  #ifdef DEBUG
        printk("    GP%d_%-2d   0x%08X and 0x%08X or 0x%08X\n",(Pin >> 4),(Pin & 0x0F),(u32)Reg, MuxRegMap[Tmp].Mask, MuxRegMap[Tmp].Mode);
  #endif
      }
      else
      {
  #ifdef DEBUG
        printk("    ANALOG SPI FUNCTION 0x%08X and 0x%08X or 0x%08X\n",(u32)Reg, MuxRegMap[Tmp].Mask, MuxRegMap[Tmp].Mode);
  #endif
      }
    }
    else
    {
      printk("*   GP%d_%-2d  ********* ERROR not found *********\n",(Pin >> 4),(Pin & 0x0F));
    }
  }
}


void      InitGpio(void)
{
  int     Port;
  int     Pin;

  // unlock
  REGUnlock;

  memcpy(InputPortPin,pInputPortPin[Hw],sizeof(EP2_InputPortPin));
  if (memcmp((const void*)InputPortPin,(const void*)pInputPortPin[Hw],sizeof(EP2_InputPortPin)) != 0)
  {
    printk("%s InputPortPin tabel broken!\n",MODULE_NAME);
  }

  for (Port = 0;Port < NO_OF_INPUT_PORTS;Port++)
  {
#ifdef DEBUG
    printk("  Input port %d\n",Port + 1);
#endif
    for (Pin = 0;Pin < INPUT_PORT_PINS;Pin++)
    {
      if (InputPortPin[Port][Pin].Pin >= 0)
      {
        InputPortPin[Port][Pin].pGpio  =  (struct gpio_controller *__iomem)(GpioBase + ((InputPortPin[Port][Pin].Pin >> 5) * 0x28) + 0x10);
        InputPortPin[Port][Pin].Mask   =  (1 << (InputPortPin[Port][Pin].Pin & 0x1F));

        SetGpio(InputPortPin[Port][Pin].Pin);
      }
    }
  }
#ifdef DEBUG
  printk("  Adc spi\n");
#endif
  for (Pin = 0;Pin < ADC_SPI_PINS;Pin++)
  {
    AdcSpiPin[Pin].pGpio  =  (struct gpio_controller *__iomem)(GpioBase + ((AdcSpiPin[Pin].Pin >> 5) * 0x28) + 0x10);
    AdcSpiPin[Pin].Mask   =  (1 << (AdcSpiPin[Pin].Pin & 0x1F));

    SetGpio(AdcSpiPin[Pin].Pin);
  }
  memcpy(OutputPortPin,pOutputPortPin[Hw],sizeof(EP2_OutputPortPin));
  if (memcmp((const void*)OutputPortPin,(const void*)pOutputPortPin[Hw],sizeof(EP2_OutputPortPin)) != 0)
  {
    printk("%s OutputPortPin tabel broken!\n",MODULE_NAME);
  }
  for (Port = 0;Port < NO_OF_OUTPUT_PORTS;Port++)
  {
#ifdef DEBUG
    printk("  Output port %d\n",Port + 1);
#endif
    for (Pin = 0;Pin < OUTPUT_PORT_PINS;Pin++)
    {
      if (OutputPortPin[Port][Pin].Pin >= 0)
      {
        OutputPortPin[Port][Pin].pGpio  =  (struct gpio_controller *__iomem)(GpioBase + ((OutputPortPin[Port][Pin].Pin >> 5) * 0x28) + 0x10);
        OutputPortPin[Port][Pin].Mask   =  (1 << (OutputPortPin[Port][Pin].Pin & 0x1F));

        SetGpio(OutputPortPin[Port][Pin].Pin);
      }
    }
  }
#ifdef DEBUG
  printk("  Adc power\n");
#endif
  memcpy(AdcPowerPin,pAdcPowerPin[Hw],sizeof(EP2_AdcPowerPin));
  if (memcmp((const void*)AdcPowerPin,(const void*)pAdcPowerPin[Hw],sizeof(EP2_AdcPowerPin)) != 0)
  {
    printk("%s AdcPowerPin tabel broken!\n",MODULE_NAME);
  }

  for (Pin = 0;Pin < ADC_POWER_PINS;Pin++)
  {
    if (AdcPowerPin[Pin].Pin >= 0)
    {
      AdcPowerPin[Pin].pGpio  =  (struct gpio_controller *__iomem)(GpioBase + ((AdcPowerPin[Pin].Pin >> 5) * 0x28) + 0x10);
      AdcPowerPin[Pin].Mask   =  (1 << (AdcPowerPin[Pin].Pin & 0x1F));

      SetGpio(AdcPowerPin[Pin].Pin);
    }
  }

  // Disable pull up/down
  PUDisable;

  // lock
  REGLock;
}


#define   PINFloat(port,pin)            {\
                                          (*InputPortPin[port][pin].pGpio).dir |=  InputPortPin[port][pin].Mask;\
                                        }


#define   PINRead(port,pin)             ((*InputPortPin[port][pin].pGpio).in_data & InputPortPin[port][pin].Mask)


#define   PINHigh(port,pin)             {\
                                          (*InputPortPin[port][pin].pGpio).set_data  =  InputPortPin[port][pin].Mask;\
                                          (*InputPortPin[port][pin].pGpio).dir      &= ~InputPortPin[port][pin].Mask;\
                                        }

#define   PINLow(port,pin)              {\
                                          (*InputPortPin[port][pin].pGpio).clr_data  =  InputPortPin[port][pin].Mask;\
                                          (*InputPortPin[port][pin].pGpio).dir      &= ~InputPortPin[port][pin].Mask;\
                                        }


#define   POUTFloat(port,pin)           {\
                                          (*OutputPortPin[port][pin].pGpio).dir |=  OutputPortPin[port][pin].Mask;\
                                        }


#define   POUTRead(port,pin)            ((*OutputPortPin[port][pin].pGpio).in_data & OutputPortPin[port][pin].Mask)


#define   POUTHigh(port,pin)            {\
                                          (*OutputPortPin[port][pin].pGpio).set_data  =  OutputPortPin[port][pin].Mask;\
                                          (*OutputPortPin[port][pin].pGpio).dir      &= ~OutputPortPin[port][pin].Mask;\
                                        }


#define   POUTLow(port,pin)             {\
                                          (*OutputPortPin[port][pin].pGpio).clr_data  =  OutputPortPin[port][pin].Mask;\
                                          (*OutputPortPin[port][pin].pGpio).dir        &= ~OutputPortPin[port][pin].Mask;\
                                        }

#define   IGENOn                        {\
                                          (*AdcPowerPin[ADCONIGEN].pGpio).set_data  =  AdcPowerPin[ADCONIGEN].Mask;\
                                          (*AdcPowerPin[ADCONIGEN].pGpio).dir      &= ~AdcPowerPin[ADCONIGEN].Mask;\
                                        }


#define   IGENOff                       {\
                                          (*AdcPowerPin[ADCONIGEN].pGpio).clr_data  =  AdcPowerPin[ADCONIGEN].Mask;\
                                          (*AdcPowerPin[ADCONIGEN].pGpio).dir      &= ~AdcPowerPin[ADCONIGEN].Mask;\
                                        }

#define   BATENOn                       {\
                                          (*AdcPowerPin[ADCBATEN].pGpio).set_data  =  AdcPowerPin[ADCBATEN].Mask;\
                                          (*AdcPowerPin[ADCBATEN].pGpio).dir      &= ~AdcPowerPin[ADCBATEN].Mask;\
                                        }


#define   BATENOff                      {\
                                          (*AdcPowerPin[ADCBATEN].pGpio).clr_data  =  AdcPowerPin[ADCBATEN].Mask;\
                                          (*AdcPowerPin[ADCBATEN].pGpio).dir      &= ~AdcPowerPin[ADCBATEN].Mask;\
                                        }



#ifdef DISABLE_OLD_COLOR

static    UBYTE CtoH(UBYTE Char)
{
  UBYTE   Hex = 0;

  if ((Char >= '0') && (Char <= '9'))
  {
    Hex  =  Char - '0';
  }
  else
  {
    if ((Char >= 'a') && (Char <= 'f'))
    {
      Hex  =  Char - 'a' + 10;
    }
    else
    {
      if ((Char >= 'A') && (Char <= 'F'))
      {
        Hex  =  Char - 'A' + 10;
      }
    }
  }

  return (Hex);
}

#endif

// SW SPI *********************************************************************

#ifndef   ADC_BITBANGING

#define   SPI0_CLOCK    150000000

#define   ADC_TIME      8               // [uS]


#define   ADC_CLOCK                     (((ULONG)1000000 * (ULONG)16) / (ULONG)ADC_TIME)
#define   CNVSPD                        (((ULONG)SPI0_CLOCK / (ULONG)ADC_CLOCK) - 1)

/* SPI Register number */
#define   SPIGCR0                       0
#define   SPIGCR1                       1
#define   SPIINT0                       2
#define   SPILVL                        3
#define   SPIFLG                        4
#define   SPIPC0                        5
#define   SPIPC1                        6
#define   SPIPC2                        7
#define   SPIPC3                        8
#define   SPIPC4                        9
#define   SPIPC5                        10
#define   SPIDAT0                       14
#define   SPIDAT1                       15
#define   SPIBUF                        16
#define   SPIEMU                        17
#define   SPIDELAY                      18
#define   SPIDEF                        19
#define   SPIFMT0                       20
#define   SPIFMT1                       21
#define   SPIFMT2                       22
#define   SPIFMT3                       23
#define   INTVEC1                       25


static    volatile  unsigned long       *Spi0;

#define   SPIRxempty                    (Spi0[SPIBUF] & 0x80000000)
#define   SPITxfull                     (Spi0[SPIBUF] & 0x20000000)

#define   SPIInit                       {\
                                          Spi0[SPIGCR1]     = 0x00000003; /* Master enable                */\
                                          Spi0[SPIPC0]      = 0x00000E08; /*                              */\
                                          Spi0[SPIDAT1]     = 0x0;        /* Format 0 is selected         */\
                                          Spi0[SPIFMT0]     = 0x00010010 | ((CNVSPD << 8) & 0xFF00);\
                                          Spi0[SPIDELAY]    = 0x0A0A0A0A; /* Delays = 10                  */\
                                          Spi0[SPIINT0]     = 0x0;        /* Interrupts disabled          */\
                                          Spi0[SPIDEF]      = 0x00000008;\
                                          Spi0[SPIGCR1]     = 0x01000003; /* Enable bit                   */\
                                        }

#define   SPIExit                       {\
                                          Spi0[SPIGCR1]     = 0x00000103; /* Master enable                */\
                                          Spi0[SPIPC0]      = 0x01010E01; /*                              */\
                                          Spi0[SPIDAT1]     = 0x003F0000;        /* Format 0 is selected         */\
                                          Spi0[SPIFMT0]     = 0x00010408 | ((CNVSPD << 8) & 0xFF00);\
                                          Spi0[SPIDELAY]    = 0x00000000; /* Delays = 10                  */\
                                          Spi0[SPIINT0]     = 0x00000000;        /* Interrupts disabled          */\
                                          Spi0[SPIDEF]      = 0x0000003F;\
                                        }



ULONG     SpiSave0;
ULONG     SpiSave1;
ULONG     SpiSave2;
ULONG     SpiSave3;
ULONG     SpiSave4;
ULONG     SpiSave5;
ULONG     SpiSave6;
ULONG     SpiSave7;


void      SpiSaveReg(void)
{
  SpiSave0  =  Spi0[SPIGCR0];
  SpiSave1  =  Spi0[SPIGCR1];
  SpiSave2  =  Spi0[SPIPC0];
  SpiSave3  =  Spi0[SPIDAT1];
  SpiSave4  =  Spi0[SPIFMT0];
  SpiSave5  =  Spi0[SPIDELAY];
  SpiSave6  =  Spi0[SPIINT0];
  SpiSave7  =  Spi0[SPIDEF];
}


void      SpiRestoreReg(void)
{
  Spi0[SPIGCR0]   =  SpiSave0;
  Spi0[SPIGCR1]   =  SpiSave1;
  Spi0[SPIPC0]    =  SpiSave2;
  Spi0[SPIDAT1]   =  SpiSave3;
  Spi0[SPIFMT0]   =  SpiSave4;
  Spi0[SPIDELAY]  =  SpiSave5;
  Spi0[SPIINT0]   =  SpiSave6;
  Spi0[SPIDEF]    =  SpiSave7;
}


void      SpiInit(void)
{
  if (request_mem_region(0x01C41000,0x68,MODULE_NAME) >= 0)
  {
    Spi0  =  (unsigned long*)ioremap(0x01C41000,0x68);
    if (Spi0 != NULL)
    {
  #ifdef DEBUG
      printk("  %s memory mapped from 0x%08lX\n",DEVICE1_NAME,(unsigned long)Spi0);
  #endif
    }

#ifdef DEBUG
    printk("SPIGCR0     = 0x%08lX\n",Spi0[SPIGCR0]);
    printk("SPIGCR1     = 0x%08lX\n",Spi0[SPIGCR1]);
    printk("SPIPC0      = 0x%08lX\n",Spi0[SPIPC0]);
    printk("SPIDAT1     = 0x%08lX\n",Spi0[SPIDAT1]);
    printk("SPIFMT0     = 0x%08lX\n",Spi0[SPIFMT0]);
    printk("SPIDELAY    = 0x%08lX\n",Spi0[SPIDELAY]);
    printk("SPIINT0     = 0x%08lX\n",Spi0[SPIINT0]);
    printk("SPIDEF      = 0x%08lX\n",Spi0[SPIDEF]);
#endif
    SpiSaveReg();
    SPIInit;
  }
}


void      SpiExit(void)
{
  SpiRestoreReg();
#ifdef DEBUG
    printk("SPIGCR0     = 0x%08lX\n",Spi0[SPIGCR0]);
    printk("SPIGCR1     = 0x%08lX\n",Spi0[SPIGCR1]);
    printk("SPIPC0      = 0x%08lX\n",Spi0[SPIPC0]);
    printk("SPIDAT1     = 0x%08lX\n",Spi0[SPIDAT1]);
    printk("SPIFMT0     = 0x%08lX\n",Spi0[SPIFMT0]);
    printk("SPIDELAY    = 0x%08lX\n",Spi0[SPIDELAY]);
    printk("SPIINT0     = 0x%08lX\n",Spi0[SPIINT0]);
    printk("SPIDEF      = 0x%08lX\n",Spi0[SPIDEF]);
#endif
  iounmap(Spi0);
}


UWORD     SpiUpdate(UWORD DataOut)
{
#ifndef DISABLE_ADC
  while (SPITxfull);

  Spi0[SPIDAT0]   = (ULONG)DataOut;

  while (SPIRxempty);

  return ((UWORD)(Spi0[SPIBUF] & 0x0000FFFF));
#else
  return(0);
#endif
}


#else

#define   SIMOHigh                      {\
                                          (*AdcSpiPin[ADCMOSI].pGpio).set_data  =  AdcSpiPin[ADCMOSI].Mask;\
                                        }


#define   SIMOLow                       {\
                                          (*AdcSpiPin[ADCMOSI].pGpio).clr_data  =  AdcSpiPin[ADCMOSI].Mask;\
                                        }


#define   CLKHigh                       {\
                                          (*AdcSpiPin[ADCCLK].pGpio).set_data  =  AdcSpiPin[ADCCLK].Mask;\
                                        }


#define   CLKLow                        {\
                                          (*AdcSpiPin[ADCCLK].pGpio).clr_data  =  AdcSpiPin[ADCCLK].Mask;\
                                        }


#define   SCSHigh                       {\
                                          (*AdcSpiPin[ADCCS].pGpio).set_data  =  AdcSpiPin[ADCCS].Mask;\
                                        }


#define   SCSLow                        {\
                                          (*AdcSpiPin[ADCCS].pGpio).clr_data  =  AdcSpiPin[ADCCS].Mask;\
                                        }


#define   SOMIFloat                     {\
                                          (*AdcSpiPin[ADCMISO].pGpio).dir |=  AdcSpiPin[ADCMISO].Mask;\
                                          (*AdcSpiPin[ADCMOSI].pGpio).dir &= ~AdcSpiPin[ADCMOSI].Mask;\
                                          (*AdcSpiPin[ADCCLK].pGpio).dir  &= ~AdcSpiPin[ADCCLK].Mask;\
                                          (*AdcSpiPin[ADCCS].pGpio).dir   &= ~AdcSpiPin[ADCCS].Mask;\
                                        }


#define   SOMIRead                      ((*AdcSpiPin[ADCMISO].pGpio).in_data & AdcSpiPin[ADCMISO].Mask)




void      SpiInit(void)
{
  SCSHigh;
  CLKLow;
  SIMOLow;
  SOMIFloat;
}


void      SpiExit(void)
{
  SCSHigh;
  CLKLow;
  SIMOLow;
  SOMIFloat;
}


UWORD     SpiUpdate(UWORD DataOut)
{
  UWORD   DataIn = 0;
  UBYTE   Bit = 16;


  SCSLow;

  while (Bit--)
  {
    if (DataOut & 0x8000)
    {
      SIMOHigh;
    }
    else
    {
      SIMOLow;
    }
    CLKHigh;
    DataOut <<=  1;
    DataIn  <<=  1;
    if (SOMIRead)
    {
      DataIn |=  1;
    }
    CLKLow;
  }

  SCSHigh;

  return (DataIn);
}

#endif


// DEVICE1 ********************************************************************

static    struct hrtimer Device1Timer;
static    ktime_t        Device1Time;

static    UBYTE TestMode = 0;

static    ANALOG  AnalogDefault;

static    ANALOG *pAnalog   =  &AnalogDefault;
static    UWORD  *pInputs   = (UWORD*)&AnalogDefault;

static    UBYTE Device3State = 0;


typedef struct
{
  UWORD   Value;
  UBYTE   Connected;
  UBYTE   Cmd;
  UBYTE   State;
  UBYTE   OldState;
  UBYTE   Event;
  UBYTE   Timer;
}
INPORT;


INPORT    InputPort[NO_OF_INPUT_PORTS];


typedef struct
{
  UWORD   Value5Float;
  UWORD   Value5Low;
  UBYTE   Connected;
  UBYTE   Code;
  UBYTE   Type;
  UBYTE   State;
  UBYTE   OldState;
  UBYTE   Event;
  UBYTE   Timer;
}
OUTPORT;


OUTPORT   OutputPort[NO_OF_OUTPUT_PORTS];

#ifndef DISABLE_OLD_COLOR

/*

          NO NXT COLOR SENSOR ATTACHED

                  |---------------------------------------------------------------------------------------------------|
                     100uS
                  |---------|


Clock             ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


INTR              |         |         |

SPI R/W           ||||      ||||      ||

PIN 1             ||||

PIN 6                       ||||

Other                                 ||

MuxSetup          0123      4567      xx

Converting        x012      3456      7x

Reading           -x01      2345      67

Time              0001      0002      01

1 = 200uS
2 = 600uS

*/

#define   SCHEMESIZE1    10

static    UBYTE MuxSetup1[SCHEMESIZE1]    = { 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x20,0x20 };

static    UBYTE Reading1[SCHEMESIZE1]     = { 0x80,0x20,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07 };

static    UBYTE NextTime1[SCHEMESIZE1]    = { 0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x01 };

static    ktime_t Time1[2];


/*

          ONE OR MORE NXT COLOR SENSOR ATTACHED

                  |---------------------------------------------------------------------------------------------------|
                     100uS
                  |---------|

                    ,---------,         ,---------,
Clock               |         |         |         |
                  --'         '---------'         '-------------------------------------------------------------------

INTR              |         |         |         |

Colour convert    ||         |         |         ||

SPI R/W           ||||      ||||      ||||      |||

PIN 1               ||      | |

PIN 6                          |      | ||

Other                                           |

MuxSetup          e012      r345      g67x      bee

Converting        ee01      2r34      5g67      xbe

Reading           --e0      12r3      45g6      7xb

ClockHigh         0100      0000      0100      000

ClockLow          0000      0100      0000      010

NextTime          0001      0001      0002      001

1 = 200uS
2 = 400uS

*/

#define   SCHEMESIZE2    15

static    UBYTE MuxSetup2[SCHEMESIZE2]    = { 0x13,0x00,0x01,0x02,0x10,0x03,0x04,0x05,0x11,0x06,0x07,0x20,0x12,0x20,0x20 };

static    UBYTE Reading2[SCHEMESIZE2]     = { 0x80,0x20,0x13,0x00,0x01,0x02,0x10,0x03,0x04,0x05,0x11,0x06,0x07,0x80,0x12 };

static    UBYTE ClockHigh2[SCHEMESIZE2]   = { 0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00 };

static    UBYTE ClockLow2[SCHEMESIZE2]    = { 0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00 };

static    UBYTE NextTime2[SCHEMESIZE2]    = { 0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x01 };

static    ktime_t Time2[2];
static    ktime_t NextTime;

static    UBYTE InputPoint1 = 8;

static    UBYTE Color = 0;
static    UBYTE NxtPointer = 0;
static    UBYTE NxtColorActive[INPUTS];
static    UBYTE Nxtcolor[INPUTS];
static    UBYTE NxtcolorCmd[INPUTS];
static    UBYTE NxtcolorLatchedCmd[INPUTS];
static    UBYTE InputPoint2 = 0;



static enum hrtimer_restart Device1TimerInterrupt1(struct hrtimer *pTimer)
{
  UBYTE   Port;
  UWORD   *pData;
  UWORD   Data;
  UWORD   Input;

  // restart timer
  hrtimer_forward_now(pTimer,NextTime);

  if (NxtPointer == 0)
  {
    if (++InputPoint2 >= INPUTS)
    {
      InputPoint2  =  0;
    }
#ifndef DISABLE_PREEMPTED_VM
    ((*pAnalog).PreemptMilliSeconds)++;
#endif
  }

  if (!Color)
  { // No NXT color sensor attached -> use scheme 1

    do
    {
      pData  =  &Data;
      if (MuxSetup1[NxtPointer] & 0x20)
      {
        Input  =  (UWORD)InputReadMap[InputPoint1];
      }
      else
      {
        Input  =   (UWORD)InputReadMap[MuxSetup1[NxtPointer] & 0x0F];
      }

      if (Reading1[NxtPointer] & 0x20)
      {
        pData  =  &pInputs[InputPoint1];
        if (++InputPoint1 >= INPUTADC)
        {
          InputPoint1  =  8;
        }
      }
      else
      {
        if (!(Reading1[NxtPointer] & 0xF0))
        {
          pData  =   &pInputs[Reading1[NxtPointer]];
        }
      }

      *pData  =  (UWORD)SpiUpdate((0x1840 | ((Input & 0x000F) << 7)));
      *pData &=  0x0FFF;

      NxtPointer++;
    }
    while ((NextTime1[NxtPointer - 1] == 0));

    NextTime  =  Time1[NextTime1[NxtPointer - 1] - 1];

    if (NxtPointer >= SCHEMESIZE1)
    {
      NxtPointer  =  0;
    }
  }
  else
  { // One or more NXT color sensors attached -> use scheme 2

    do
    {

      pData  =  &Data;
      if (MuxSetup2[NxtPointer] & 0x20)
      {
        Input  =  (UWORD)InputReadMap[InputPoint1];
      }
      else
      {
        if (MuxSetup2[NxtPointer] & 0x10)
        {
          Input  =  (UWORD)InputReadMap[InputPoint2 + INPUTS];
        }
        else
        {
          Input  =   (UWORD)InputReadMap[MuxSetup2[NxtPointer] & 0x0F];
        }
      }

      if (Reading2[NxtPointer] & 0x20)
      {
        pData  =  &pInputs[InputPoint1];
        if (++InputPoint1 >= INPUTADC)
        {
          InputPoint1  =  8;
        }
      }
      else
      {
        if (Reading2[NxtPointer] & 0x10)
        {
          pData  =  (UWORD*)&(*pAnalog).NxtCol[InputPoint2].ADRaw[(Reading2[NxtPointer] & 0x03)];
        }
        else
        {
          if (!(Reading2[NxtPointer] & 0xF0))
          {
            pData  =   &pInputs[Reading2[NxtPointer]];
          }
        }
      }

      *pData  =  (UWORD)SpiUpdate((0x1840 | ((Input & 0x000F) << 7))) & 0xFFF;
      if (NxtcolorLatchedCmd[InputPoint2] == 0x0D)
      {
        if (ClockHigh2[NxtPointer])
        {
          if (Nxtcolor[InputPoint2])
          {
            if (ClockHigh2[NxtPointer] == 0x01)
            {
              PINHigh(InputPoint2,INPUT_PORT_PIN5);
            }
            else
            {
              PINFloat(InputPoint2,INPUT_PORT_PIN5);
            }
          }
        }
        if (ClockLow2[NxtPointer])
        {
          if (Nxtcolor[InputPoint2])
          {
            PINLow(InputPoint2,INPUT_PORT_PIN5);
          }
        }
      }

      NxtPointer++;
    }
    while ((NextTime2[NxtPointer - 1] == 0));

    NextTime  =  Time2[NextTime2[NxtPointer - 1] - 1];

    if (NxtPointer >= SCHEMESIZE2)
    {
      NxtPointer  =  0;
    }
  }

  if (NxtPointer == 0)
  {
    Color  =  0;
    for (Port = 0;Port < INPUTS;Port++)
    {
#ifndef DISABLE_FAST_DATALOG_BUFFER
      if (NxtColorActive[Port])
      {
        Color  =  1;
      }
      else
      { // Buffer for fast data logging

        (*pAnalog).Pin1[Port][(*pAnalog).LogIn[Port]]  =  (*pAnalog).InPin1[Port];
        (*pAnalog).Pin6[Port][(*pAnalog).LogIn[Port]]  =  (*pAnalog).InPin6[Port];

        (*pAnalog).Actual[Port]  =  (*pAnalog).LogIn[Port];

        if (++((*pAnalog).LogIn[Port]) >= DEVICE_LOGBUF_SIZE)
        {
          (*pAnalog).LogIn[Port]      =  0;
        }
        if ((*pAnalog).LogIn[Port] == (*pAnalog).LogOut[Port])
        {
          if (++((*pAnalog).LogOut[Port]) >= DEVICE_LOGBUF_SIZE)
          {
            (*pAnalog).LogOut[Port]   =  0;
          }
        }
      }
      Nxtcolor[Port]  =  NxtColorActive[Port];
#else
      if (NxtColorActive[Port])
      {
        Color  =  1;
      }
      Nxtcolor[Port]  =  NxtColorActive[Port];
#endif

      (*pAnalog).Updated[Port]  =  1;
    }
    NxtcolorLatchedCmd[InputPoint2]  =  NxtcolorCmd[InputPoint2];
  }

  return (HRTIMER_RESTART);
}


#else

static    UBYTE InputScheme1[INPUTADC + 1];
static    UBYTE InputScheme2[INPUTADC + 1];
static    UBYTE InputPoint1 = 0;
static    UBYTE InputPoint2 = 0;
static    UBYTE InputLast   = 0;
static    UBYTE InputMid    = 0;
static    UBYTE InputNext   = 0;

static enum hrtimer_restart Device1TimerInterrupt1(struct hrtimer *pTimer)
{
  DATA8   Port;

  // find next channel from scheme 2 to convert
  InputScheme1[InputSchemeLng1]  =  InputScheme2[InputPoint2];
  if (++InputPoint2 >= InputSchemeLng2)
  {
    InputPoint2  =  0;
  }

  // do the scheme
  InputPoint1  =  0;
  while ((InputPoint1 < InputSchemeLng1) || ((InputPoint1 <= InputSchemeLng1) && (InputSchemeLng2)))
  {
    InputLast  =  InputMid;
    InputMid   =  InputNext;
    InputNext  =  InputScheme1[InputPoint1];

    pInputs[InputLast]  =  (UWORD)SpiUpdate((0x1850 | (InputReadMap[InputNext] << 7))) & 0xFFF;
    InputPoint1++;
#if       (HARDWARE == ONE2ONE)
    if (InputLast == 15)
    {
      pMuxData[MuxAddr]  =  pInputs[InputLast];

      if (++MuxAddr >= 8)
      {
        MuxAddr  =  0;
      }
      SetMultiplexer(MuxAddr);
    }
#endif
  }

  for (Port = 0;Port < INPUTS;Port++)
  {
    (*pAnalog).Updated[Port]  =  1;
  }

  // restart timer
  hrtimer_forward_now(pTimer,Device1Time);
  return (HRTIMER_RESTART);
}

#endif


enum      DCM_STATE
{
  DCM_INIT,
  DCM_FLOATING_DELAY,
  DCM_FLOATING,
  DCM_WAITING_FOR_PIN5_LOW,
  DCM_WAITING_FOR_PIN6_LOW,
  DCM_CONNECTION,
  DCM_PIN2_LOW,
  DCM_NXT_TOUCH_CHECK,
#ifndef DISABLE_OLD_COLOR
  DCM_NXT_COLOR_INIT,
  DCM_NXT_COLOR_WAIT,
  DCM_NXT_COLOR_START,
  DCM_NXT_COLOR_BUSY,
#endif
  DCM_CONNECTED_WAITING_FOR_PIN2_HIGH,
  DCM_PIN1_LOADED,
  DCM_CONNECTED_WAITING_FOR_PIN1_TO_FLOAT,
  DCM_PIN6_HIGH,
  DCM_CONNECTED_WAITING_FOR_PIN6_LOW,
  DCM_PIN5_LOW,
  DCM_CONNECTED_WAITING_FOR_PIN5_HIGH,
  DCM_CONNECTED_WAITING_FOR_PORT_OPEN,
  DCM_STATES
};


char      DcmStateText[DCM_STATES][50] =
{
  "DCM_INIT",
  "DCM_FLOATING_DELAY",
  "DCM_FLOATING\n",
  "DCM_WAITING_FOR_PIN5_LOW",
  "DCM_WAITING_FOR_PIN6_LOW",
  "DCM_CONNECTION",
  "DCM_PIN2_LOW",
  "DCM_NXT_TOUCH_CHECK",
#ifndef DISABLE_OLD_COLOR
  "DCM_NXT_COLOR_INIT",
  "DCM_NXT_COLOR_WAIT",
  "DCM_NXT_COLOR_START",
  "DCM_NXT_COLOR_BUSY",
#endif
  "DCM_CONNECTED_WAITING_FOR_PIN2_HIGH",
  "DCM_PIN1_LOADED",
  "DCM_CONNECTED_WAITING_FOR_PIN1_TO_FLOAT",
  "DCM_PIN6_HIGH",
  "DCM_PIN5_LOW",
  "DCM_CONNECTED_WAITING_FOR_PIN5_HIGH",
  "DCM_CONNECTED_WAITING_FOR_PIN6_LOW",
  "DCM_CONNECTED_WAITING_FOR_PORT_OPEN"
};


INPORT    InputPortDefault =
{
  0,
  0,
  DCM_INIT,
  -1,
  0,
  0
};


OUTPORT   OutputPortDefault =
{
  0,
  0,
  0,
  0,
  0,
  DCM_INIT,
  -1,
  0,
  0,
};


static void InputPortFloat(int Port)
{
  PINLow(Port,INPUT_PORT_PIN1);
  PINFloat(Port,INPUT_PORT_PIN2);
  PINFloat(Port,INPUT_PORT_PIN5);
  PINFloat(Port,INPUT_PORT_PIN6);
  PINHigh(Port,INPUT_PORT_BUF);
}


static void OutputPortFloat(int Port)
{
  POUTLow(Port,OUTPUT_PORT_PIN1);
  POUTLow(Port,OUTPUT_PORT_PIN2);
  POUTLow(Port,OUTPUT_PORT_PIN5W);
  POUTFloat(Port,OUTPUT_PORT_PIN6);
}


UWORD     Device1GetInputPins(UBYTE Port)
{
  UWORD   Pins = 0x0000;
  UWORD   Mask = 0x0001;
  UBYTE   Tmp;

  for (Tmp = 0;Tmp < INPUT_PORT_PINS;Tmp++)
  {
    if (PINRead(Port,Tmp))
    {
      Pins |= Mask;
    }
    Mask <<= 1;
  }

  return (Pins);
}


UWORD     Device1GetOutputPins(UBYTE Port)
{
  UWORD   Pins = 0x0000;
  UWORD   Mask = 0x0001;
  UBYTE   Tmp;

  for (Tmp = 0;Tmp < OUTPUT_PORT_PINS;Tmp++)
  {
    if (POUTRead(Port,Tmp))
    {
      Pins |= Mask;
    }
    Mask <<= 1;
  }

  return (Pins);
}


static ssize_t Device1Write(struct file *File,const char *Buffer,size_t Count,loff_t *Data)
{
  int     Lng = 0;



  return (Lng);
}


/*! \page AnalogModule
 *
 *  <hr size="1"/>
 *  <b>     read </b>
 *
 *- xxxx xxxx xxxx xxxx xxxx xxxx xxxx xxxx xxxx xxxx xxxx xxxx     - xxxx is logical channel value in decimal (1st ch 0, next ch 1 ---\n
 *
 */
/*! \brief    Device1Write
 *
 *
 *
 */
static ssize_t Device1Read(struct file *File,char *Buffer,size_t Count,loff_t *Offset)
{

  int     Lng     = 0;
  int     Tmp;
  int     Point;
  UWORD   Pins;

  for (Point = 0;Point < INPUTS;Point++)
  {
    Pins  =  Device1GetInputPins(Point);
    for (Tmp = 0;Tmp < INPUT_PORT_PINS;Tmp++)
    {
      if ((Pins & 0x0001))
      {
        Buffer[Lng++]  =  '1';
      }
      else
      {
        Buffer[Lng++]  =  '0';
      }
      Pins >>= 1;
    }
    Pins  =  Device1GetOutputPins(Point);
    for (Tmp = 0;Tmp < OUTPUT_PORT_PINS;Tmp++)
    {
      if ((Pins & 0x0001))
      {
        Buffer[Lng++]  =  '1';
      }
      else
      {
        Buffer[Lng++]  =  '0';
      }
      Pins >>= 1;
    }
    Buffer[Lng++]  =  ' ';
  }

  Buffer[Lng++]  =  '\r';

  Buffer[Lng++]  =  0x1B;
  Buffer[Lng++]  =  '[';
  Buffer[Lng++]  =  'B';

  Point  =  0;
  Tmp    =  5;
  while ((Count > Tmp) && (Point < INPUTADC))
  {
    if (Point != (INPUTADC - 1))
    {
      Tmp    =  snprintf(&Buffer[Lng],6,"%04u ",pInputs[Point]);
    }
    else
    {
      Tmp    =  snprintf(&Buffer[Lng],7,"%04u\r",pInputs[Point]);
    }
    Lng   +=  Tmp;
    Count -=  Tmp;
    Point++;
  }

  Buffer[Lng++]  =  0x1B;
  Buffer[Lng++]  =  '[';
  Buffer[Lng++]  =  'A';



  return (Lng);
}

#define     SHM_LENGTH    (sizeof(AnalogDefault))
#define     NPAGES        ((SHM_LENGTH + PAGE_SIZE - 1) / PAGE_SIZE)
static void *kmalloc_ptr;

static int Device1Mmap(struct file *filp, struct vm_area_struct *vma)
{
   int ret;

   ret = remap_pfn_range(vma,vma->vm_start,virt_to_phys((void*)((unsigned long)pAnalog)) >> PAGE_SHIFT,vma->vm_end-vma->vm_start,PAGE_SHARED);

   if (ret != 0)
   {
     ret  =  -EAGAIN;
   }

   return (ret);
}


static    const struct file_operations Device1Entries =
{
  .owner        = THIS_MODULE,
  .read         = Device1Read,
  .write        = Device1Write,
  .mmap         = Device1Mmap,
};


static    struct miscdevice Device1 =
{
  MISC_DYNAMIC_MINOR,
  DEVICE1_NAME,
  &Device1Entries
};


static int Device1Init(void)
{
  int     Result = -1;
  UWORD   *pTmp;
  int     i;
  DATA8   Port;

  Result  =  misc_register(&Device1);
  if (Result)
  {
    printk("  %s device register failed\n",DEVICE1_NAME);
  }
  else
  {
    SpiInit();

    SpiUpdate(0x400F);
    SpiUpdate(0x400F);
    SpiUpdate(0x400F);
    SpiUpdate(0x400F);
    SpiUpdate(0x400F);
    SpiUpdate(0x400F);

    // allocate kernel shared memory for analog values (pAnalog)
    if ((kmalloc_ptr = kmalloc((NPAGES + 2) * PAGE_SIZE, GFP_KERNEL)) != NULL)
    {
      pTmp = (UWORD*)((((unsigned long)kmalloc_ptr) + PAGE_SIZE - 1) & PAGE_MASK);
      for (i = 0; i < NPAGES * PAGE_SIZE; i += PAGE_SIZE)
      {
        SetPageReserved(virt_to_page(((unsigned long)pTmp) + i));
      }
      pAnalog      =  (ANALOG*)pTmp;
      memset(pAnalog,0,sizeof(ANALOG));
      pInputs      =  pTmp;

      for (Port = 0;Port < INPUTS;Port++)
      {
        (*pAnalog).InDcm[Port]    =  0;
        (*pAnalog).InConn[Port]   =  0;
      }
      for (Port = 0;Port < OUTPUTS;Port++)
      {
        (*pAnalog).OutDcm[Port]   =  0;
        (*pAnalog).OutConn[Port]  =  0;
      }

      // setup analog update timer interrupt

      Time1[0]  =  ktime_set(0,200000);
      Time1[1]  =  ktime_set(0,600000);
      Time2[0]  =  ktime_set(0,200000);
      Time2[1]  =  ktime_set(0,400000);

      NextTime  =  Time1[0];

      Device1Time  =  ktime_set(0,DEVICE_UPDATE_TIME);
      hrtimer_init(&Device1Timer,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
      Device1Timer.function  =  Device1TimerInterrupt1;
      hrtimer_start(&Device1Timer,Device1Time,HRTIMER_MODE_REL);

#ifdef DEBUG
      printk("  %s device register succes\n",DEVICE1_NAME);
#endif
    }
    else
    {
      printk("  %s kmalloc failed !!\n",DEVICE1_NAME);
    }
  }

  return (Result);
}


static void Device1Exit(void)
{
  UWORD   *pTmp;
  int     i;

  hrtimer_cancel(&Device1Timer);

  SpiExit();

  pTmp     =  pInputs;
  pInputs  =  (UWORD*)&AnalogDefault;
  pAnalog  =  &AnalogDefault;

  // free shared memory
  for (i = 0; i < NPAGES * PAGE_SIZE; i+= PAGE_SIZE)
  {
    ClearPageReserved(virt_to_page(((unsigned long)pTmp) + i));
#ifdef DEBUG
    printk("  %s memory page %d unmapped\n",DEVICE1_NAME,i);
#endif
  }
  kfree(kmalloc_ptr);

  misc_deregister(&Device1);
#ifdef DEBUG
  printk("  %s device unregistered\n",DEVICE1_NAME);
#endif
}


// DEVICE2 ********************************************************************

#define   BUFFER_LNG      16

static int Device2Ioctl(struct inode *pNode, struct file *File, unsigned int Request, unsigned long Pointer)
{
  int     Result = 0;
  TSTPIN  Tstpin;
  int     Poi;
  int     Lng;
  UWORD   Pins;
  UBYTE   Port;
  UBYTE   Tmp;


  copy_from_user((void*)&Tstpin,(void*)Pointer,sizeof(TSTPIN));

  Port  =  Tstpin.Port;
  Lng   =  (int)Tstpin.Length;

  switch (Request)
  {
    case TST_PIN_OFF :
    { // Normal mode

      Device3State  =  0;
      TestMode      =  0;
      for (Port = 0;Port < INPUTS;Port++)
      {
        InputPort[Port].State       =  DCM_INIT;
      }
      for (Port = 0;Port < OUTPUTS;Port++)
      {
        OutputPort[Port].State      =  DCM_INIT;
      }
    }
    break;

    case TST_PIN_ON :
    { // Test mode

      Device3State  =  0;
      TestMode      =  1;
      for (Port = 0;Port < INPUTS;Port++)
      {
        InputPortFloat(Port);
        (*pAnalog).InDcm[Port]    =  TYPE_NONE;
        (*pAnalog).InConn[Port]   =  CONN_NONE;
      }

      for (Port = 0;Port < OUTPUTS;Port++)
      {
        OutputPortFloat(Port);
        (*pAnalog).OutDcm[Port]    =  TYPE_NONE;
        (*pAnalog).OutConn[Port]   =  CONN_NONE;
      }
    }
    break;

    case TST_PIN_READ :
    { // Read pins

      Poi   =  0;

      if (Lng)
      {
        Lng--;

        if ((Port >= 0) && (Port < INPUTS))
        {
          Pins  =  Device1GetInputPins(Port);
          for (Tmp = 0;(Poi < Lng) && (Tmp < INPUT_PORT_PINS);Tmp++)
          {
            if ((Pins & 0x0001))
            {
              Tstpin.String[Poi]  =  '1';
            }
            else
            {
              Tstpin.String[Poi]  =  '0';
            }
            Pins >>= 1;
            Poi++;
          }
        }
        if (Port >= (INPUTS * CHAIN_DEPT))
        {
          Port -=  (INPUTS * CHAIN_DEPT);
          if (Port < OUTPUTS)
          {
            Pins  =  Device1GetOutputPins(Port);
            for (Tmp = 0;(Poi < Lng) && (Tmp < OUTPUT_PORT_PINS);Tmp++)
            {
              if ((Pins & 0x0001))
              {
                Tstpin.String[Poi]  =  '1';
              }
              else
              {
                Tstpin.String[Poi]  =  '0';
              }
              Pins >>= 1;
              Poi++;
            }
          }
        }
        for (;Poi < Lng;Poi++)
        {
          Tstpin.String[Poi]  =  ' ';
        }
        Tstpin.String[Poi]  =  0;
        Poi++;
      }

      copy_to_user((void*)Pointer,(void*)&Tstpin,sizeof(TSTPIN));
    }
    break;

    case TST_PIN_WRITE :
    { // Write pins

      Poi   =  0;

      if ((Lng > 0) && (Lng < BUFFER_LNG))
      {

        if ((Port >= 0) && (Port < INPUTS))
        {
          for (Tmp = 0;(Poi < Lng) && (Tmp < INPUT_PORT_PINS);Tmp++)
          {
            if (Tstpin.String[Poi] == '0')
            {
              PINLow(Port,Tmp);
            }
            if (Tstpin.String[Poi] == '1')
            {
              PINHigh(Port,Tmp);
            }
            if ((Tstpin.String[Poi] == 'x') || (Tstpin.String[Poi] == 'X'))
            {
              PINFloat(Port,Tmp);
            }
            Poi++;
          }
        }
        if (Port >= (INPUTS * CHAIN_DEPT))
        {
          Port -=  (INPUTS * CHAIN_DEPT);
          if (Port < OUTPUTS)
          {
            Pins  =  Device1GetOutputPins(Port);
            for (Tmp = 0;(Poi < Lng) && (Tmp < OUTPUT_PORT_PINS);Tmp++)
            {
              if (Tstpin.String[Poi] == '0')
              {
                POUTLow(Port,Tmp);
              }
              if (Tstpin.String[Poi] == '1')
              {
                POUTHigh(Port,Tmp);
              }
              if ((Tstpin.String[Poi] == 'x') || (Tstpin.String[Poi] == 'X'))
              {
                POUTFloat(Port,Tmp);
              }
              Poi++;
            }
          }
        }
      }
    }
    break;
  }

  return (Result);
}


static ssize_t Device2Write(struct file *File,const char *Buffer,size_t Count,loff_t *Data)
{
  int     Lng     = 0;



  return (Lng);
}


static ssize_t Device2Read(struct file *File,char *Buffer,size_t Count,loff_t *Offset)
{
  int     Lng = 0;
  UWORD   Pins;
  UBYTE   Port;
  UBYTE   Tmp;

  if (Count >= (INPUTS * (INPUT_PORT_PINS + OUTPUT_PORT_PINS + 1) + 2))
  {
    for (Port = 0;Port < INPUTS;Port++)
    {
      Pins  =  Device1GetInputPins(Port);
      for (Tmp = 0;Tmp < INPUT_PORT_PINS;Tmp++)
      {
        if ((Pins & 0x0001))
        {
          Buffer[Lng++]  =  '1';
        }
        else
        {
          Buffer[Lng++]  =  '0';
        }
        Pins >>= 1;
      }
      Pins  =  Device1GetOutputPins(Port);
      for (Tmp = 0;Tmp < OUTPUT_PORT_PINS;Tmp++)
      {
        if ((Pins & 0x0001))
        {
          Buffer[Lng++]  =  '1';
        }
        else
        {
          Buffer[Lng++]  =  '0';
        }
        Pins >>= 1;
      }
      Buffer[Lng++]  =  ' ';
    }

    Buffer[Lng++]  =  '\r';
    Buffer[Lng++]  =  0;
  }

  return (Lng);
}


static    const struct file_operations Device2Entries =
{
  .owner        = THIS_MODULE,
  .read         = Device2Read,
  .write        = Device2Write,
  .ioctl        = Device2Ioctl
};


static    struct miscdevice Device2 =
{
  MISC_DYNAMIC_MINOR,
  DEVICE2_NAME,
  &Device2Entries
};


static int Device2Init(void)
{
  int     Result = -1;

  Result  =  misc_register(&Device2);
  if (Result)
  {
    printk("  %s device register failed\n",DEVICE2_NAME);
  }
  else
  {
#ifdef DEBUG
    printk("  %s device register succes\n",DEVICE2_NAME);
#endif
  }

  return (Result);
}


static void Device2Exit(void)
{
  misc_deregister(&Device2);
#ifdef DEBUG
  printk("  %s device unregistered\n",DEVICE2_NAME);
#endif
}


// DEVICE3 ********************************************************************

#define   DCM_TIMER_RESOLUTION          10                        // [mS]
#define   DCM_DEVICE_RESET_TIME         2000                      // [mS]
#define   DCM_FLOAT_DELAY               20                        // [mS]
#define   DCM_LOW_DELAY                 20                        // [mS]
#define   DCM_TOUCH_DELAY               20                        // [mS]
#define   DCM_CONNECT_STABLE_DELAY      IN_CONNECT_STEADY_TIME    // [mS]
#define   DCM_EVENT_STABLE_DELAY        IN_DISCONNECT_STEADY_TIME // [mS]

#ifndef DISABLE_OLD_COLOR
#define   DCM_NXT_COLOR_TIMEOUT         500                       // [mS]
#define   DCM_NXT_COLOR_INIT_DELAY      100                       // [mS]
#define   DCM_NXT_COLOR_HIGH_TIME       20                        // [mS]
#endif

static    struct hrtimer Device3Timer;
static    ktime_t        Device3Time;

static    UWORD Device3StateTimer;


#ifndef DISABLE_OLD_COLOR

static    struct hrtimer NxtColorTimer;
static    ktime_t        NxtColorTime;
#define   NXTCOLOR_TIMER_RESOLUTION      200              // [uS]






#define   NXTCOLOR_BYTES            (12 * 4 + 3 * 2)
#define   NXTCOLOR_BITS             (NXTCOLOR_BYTES * 8)

static    UBYTE   NxtColorCmd[INPUTS];
static    UBYTE   NxtColorByte[INPUTS];
static    UBYTE   NxtColorTx[INPUTS];
static    UBYTE   NxtColorClkHigh[INPUTS];

static    UBYTE   NxtColorState[INPUTS]     = { 0,0,0,0 };
static    UBYTE   NxtColorBytePnt[INPUTS];
static    UBYTE   NxtColorByteCnt[INPUTS];
static    UBYTE   NxtColorBitCnt[INPUTS];
static    UBYTE   NxtColorBuffer[INPUTS][NXTCOLOR_BYTES];

static    UWORD   NxtColorInitTimer[INPUTS];
static    UBYTE   NxtColorInitCnt[INPUTS];

static    UBYTE   NxtColorInitInUse;



static enum hrtimer_restart NxtColorCommIntr(struct hrtimer *pTimer)
{
  UBYTE   Port;

  hrtimer_forward_now(pTimer,NxtColorTime);

  for (Port = 0;Port < NO_OF_INPUT_PORTS;Port++)
  { // look at one port at a time

    if (NxtColorState[Port])
    {
      switch (NxtColorState[Port])
      {
        case 1 :
        {
          PINFloat(Port,INPUT_PORT_PIN5)
          NxtColorState[Port]++;
        }
        break;

        case 2 :
        {
          if (PINRead(Port,INPUT_PORT_PIN5))
          {
            if (NxtColorInitCnt[Port] == 0)
            {
              PINHigh(Port,INPUT_PORT_PIN5);
              NxtColorState[Port]++;
            }
            else
            {
              NxtColorInitTimer[Port]  =  0;
              NxtColorState[Port]     +=  2;
            }
          }
          else
          {
            PINHigh(Port,INPUT_PORT_PIN5);
            NxtColorState[Port]++;
          }
        }
        break;

        case 3 :
        {
          PINLow(Port,INPUT_PORT_PIN5);
          if (++NxtColorInitCnt[Port] >= 2)
          {
            NxtColorState[Port]++;
          }
          else
          {
            NxtColorState[Port]  =  1;
          }
        }
        break;

        case 4 :
        {
          PINLow(Port,INPUT_PORT_PIN5);
          if (++NxtColorInitTimer[Port] >= ((DCM_NXT_COLOR_INIT_DELAY * 1000) / NXTCOLOR_TIMER_RESOLUTION))
          {
            NxtColorState[Port]++;
          }
        }
        break;

        case 5 :
        {
          NxtColorBuffer[Port][0]   =  NxtColorCmd[Port];
          NxtColorByteCnt[Port]     =  1;
          NxtColorBytePnt[Port]     =  0;
          NxtColorTx[Port]          =  1;
          NxtColorState[Port]++;
        }
        break;

        case 6 :
        {
          if ((NxtColorBitCnt[Port] == 0)  && (NxtColorByteCnt[Port] == 0))
          {
            NxtColorByteCnt[Port]     =  NXTCOLOR_BYTES;
            NxtColorBytePnt[Port]     =  0;
            NxtColorTx[Port]          =  0;
            NxtColorState[Port]++;
          }
        }
        break;

        case 7 :
        {
          if ((NxtColorBitCnt[Port] == 0)  && (NxtColorByteCnt[Port] == 0))
          {
            NxtColorState[Port]++;
          }
        }
        break;

        default :
        {
          NxtColorState[Port]  =  0;
        }
        break;

      }

      if (NxtColorBitCnt[Port])
      {
        if (!NxtColorClkHigh[Port])
        {
          if (NxtColorTx[Port])
          {
            if (NxtColorByte[Port] & 1)
            {
              PINHigh(Port,INPUT_PORT_PIN6);
            }
            else
            {
              PINLow(Port,INPUT_PORT_PIN6);
            }
            NxtColorByte[Port] >>= 1;
          }
          else
          {
            PINFloat(Port,INPUT_PORT_PIN6);
          }
          PINHigh(Port,INPUT_PORT_PIN5);
          NxtColorClkHigh[Port]  =  1;
        }
        else
        {

          NxtColorBitCnt[Port]--;
          if (!NxtColorTx[Port])
          {
            NxtColorByte[Port] >>= 1;
            if (PINRead(Port,INPUT_PORT_PIN6))
            {
              NxtColorByte[Port] |=  0x80;
            }
            else
            {
              NxtColorByte[Port] &= ~0x80;
            }
            if (!NxtColorBitCnt[Port])
            {
              NxtColorBuffer[Port][NxtColorBytePnt[Port]]  =  NxtColorByte[Port];
              NxtColorBytePnt[Port]++;
            }
          }
          PINLow(Port,INPUT_PORT_PIN5);
          NxtColorClkHigh[Port]  =  0;
        }
      }
      else
      {
        if (NxtColorByteCnt[Port])
        {
          if (NxtColorTx[Port])
          {
            NxtColorByte[Port]  =  NxtColorBuffer[Port][NxtColorBytePnt[Port]];
            NxtColorBytePnt[Port]++;
          }
          NxtColorBitCnt[Port]  =  8;
          NxtColorByteCnt[Port]--;
        }
      }
    }
  }

  return (HRTIMER_RESTART);
}


void      NxtColorCommStart(UBYTE Port,UBYTE Cmd)
{
  NxtColorState[Port]     =  1;
  NxtColorInitCnt[Port]   =  0;
  NxtColorBytePnt[Port]   =  0;
  NxtColorByteCnt[Port]   =  0;
  NxtColorBitCnt[Port]    =  0;
  NxtColorCmd[Port]       =  Cmd;

  if (NxtColorInitInUse == 0)
  {
    NxtColorTime      =  ktime_set(0,NXTCOLOR_TIMER_RESOLUTION * 1000);
    hrtimer_init(&NxtColorTimer,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
    NxtColorTimer.function  =  NxtColorCommIntr;
    hrtimer_start(&NxtColorTimer,NxtColorTime,HRTIMER_MODE_REL);
  }
  NxtColorInitInUse |=  (1 << Port);
}


UBYTE     NxtColorCommReady(UBYTE Port)
{
  UBYTE   Result = 0;

  if (NxtColorState[Port] == 0)
  {
    Result  =  1;
  }

  return (Result);
}


void      NxtColorCommStop(UBYTE Port)
{

  NxtColorInitInUse &= ~(1 << Port);

  if (NxtColorInitInUse == 0)
  {
    hrtimer_cancel(&NxtColorTimer);
  }
  NxtColorState[Port]     =  0;
}


#endif


static enum hrtimer_restart Device3TimerInterrupt1(struct hrtimer *pTimer)
{
  UBYTE   Port;
  UBYTE   Event;
  UWORD   Tmp;
#ifndef DISABLE_OLD_COLOR
  UBYTE   *pData;
#endif

  hrtimer_forward_now(pTimer,Device3Time);

  switch (Device3State)
  {
    case 0 :
    {
      if (TestMode == 0)
      {
        Device3State  =  1;
      }
    }
    break;

    case 1 :
    {
//      IGENOff;
      Device3StateTimer  =  0;
      Device3State++;
    }
    break;

    case 2 :
    {
      if (++Device3StateTimer >= (DCM_DEVICE_RESET_TIME / DCM_TIMER_RESOLUTION))
      {
//        IGENOn;
        Device3State++;
      }
    }
    break;

    default :
    {
      for (Port = 0;Port < NO_OF_INPUT_PORTS;Port++)
      { // look at one port at a time

        switch (InputPort[Port].State)
        {
          case DCM_INIT :
          { // set input port inactive

#ifndef DISABLE_OLD_COLOR
            NxtColorActive[Port]       =  0;
            NxtColorCommStop(Port);
#endif
            InputPortFloat(Port);
            (*pAnalog).InDcm[Port]     =  TYPE_NONE;
            (*pAnalog).InConn[Port]    =  CONN_NONE;
            InputPort[Port].Timer      =  0;
            InputPort[Port].Event      =  0;
            InputPort[Port].State    =  DCM_FLOATING_DELAY;
          }
          break;

          case DCM_FLOATING_DELAY :
          { // wait for port pins to float

            if (++(InputPort[Port].Timer) >= (DCM_FLOAT_DELAY / DCM_TIMER_RESOLUTION))
            {
              InputPort[Port].Timer    =  0;
              InputPort[Port].State    =  DCM_FLOATING;
            }
          }
          break;

          case DCM_FLOATING :
          { // pins floating - check and for connection event

            Event  =  0;
            if (!(PINRead(Port,INPUT_PORT_PIN2)))
            { // pin 2 low

              Event |=  (0x01 << INPUT_PORT_PIN2);
            }
            if ((*pAnalog).InPin1[Port] < VtoC(IN1_NEAR_5V))
            { // pin 1 loaded

              Event |=  (0x01 << INPUT_PORT_VALUE);
            }
            if (!(PINRead(Port,INPUT_PORT_PIN5)))
            { // pin 5 low

              Event |=  (0x01 << INPUT_PORT_PIN5);
            }
            if ((PINRead(Port,INPUT_PORT_PIN6)))
            { // pin 6 high

              Event |=  (0x01 << INPUT_PORT_PIN6);
            }
            if (InputPort[Port].Event != Event)
            { // pins has changed - reset timer

#ifdef DEBUG
              printk("i ! %d Event = %02X Old = %02X\n",Port,Event,InputPort[Port].Event);
#endif
              InputPort[Port].Event    =  Event;
              InputPort[Port].Timer    =  0;
            }

            if (InputPort[Port].Event)
            { // some event

              if (++(InputPort[Port].Timer) >= (DCM_CONNECT_STABLE_DELAY / DCM_TIMER_RESOLUTION))
              {
                // some event is stable

                InputPort[Port].State  =  DCM_CONNECTION;
              }
            }

          }
          break;

          case DCM_CONNECTION :
          { // something is connected - try to evaluate

            if (InputPort[Port].Event & (0x01 << INPUT_PORT_PIN2))
            { // pin 2 is low

              InputPort[Port].State  =  DCM_PIN2_LOW;
            }
            else
            {
              if (InputPort[Port].Event & (0x01 << INPUT_PORT_VALUE))
              { // pin 1 is loaded

                InputPort[Port].State  =  DCM_PIN1_LOADED;
              }
              else
              {
                if (InputPort[Port].Event & (0x01 << INPUT_PORT_PIN6))
                { // pin 6 is high

                  InputPort[Port].State  =  DCM_PIN6_HIGH;
                }
                else
                {
                  if (InputPort[Port].Event & (0x01 << INPUT_PORT_PIN6))
                  { // pin 5 is low

                    InputPort[Port].State  =  DCM_PIN5_LOW;
                  }
                  else
                  { // ?

                    InputPort[Port].State  =  DCM_INIT;
                  }
                }
              }
            }
#ifndef DISABLE_FAST_DATALOG_BUFFER
            (*pAnalog).Actual[Port]     =  0;
            (*pAnalog).LogIn[Port]      =  0;
            (*pAnalog).LogOut[Port]     =  0;
#endif
          }
          break;

          case DCM_PIN2_LOW :
          {
            InputPort[Port].Connected   =  1;
            InputPortFloat(Port);
            InputPort[Port].Timer       =  0;
            InputPort[Port].State       =  DCM_CONNECTED_WAITING_FOR_PIN2_HIGH;

            if ((!(InputPort[Port].Event & (0x01 << INPUT_PORT_PIN5))) && (InputPort[Port].Event & (0x01 << INPUT_PORT_PIN6)))
            { // pin 5 and 6 is high

              if ((*pAnalog).InPin1[Port] < VtoC(IN1_NEAR_GND))
              { // nxt color sensor

                (*pAnalog).InDcm[Port]      =  TYPE_NXT_COLOR;
#ifndef DISABLE_OLD_COLOR
                (*pAnalog).InConn[Port]     =  CONN_NXT_COLOR;
                InputPort[Port].State       =  DCM_NXT_COLOR_INIT;
#else
                (*pAnalog).InConn[Port]     =  CONN_NXT_DUMB;
#endif
              }
              else
              { // nxt IIC sensor

                (*pAnalog).InDcm[Port]      =  TYPE_NXT_IIC;
                (*pAnalog).InConn[Port]     =  CONN_NXT_IIC;
              }
            }
            else
            {
              if (InputPort[Port].Event & (0x01 << INPUT_PORT_PIN5))
              { // nxt light sensor

                if (InputPort[Port].Event & (0x01 << INPUT_PORT_PIN6))
                { // nxt test sensor

                  (*pAnalog).InDcm[Port]    =  TYPE_NXT_TEST;
                  (*pAnalog).InConn[Port]   =  CONN_NXT_DUMB;
                }
                else
                {
                  (*pAnalog).InDcm[Port]    =  TYPE_NXT_LIGHT;
                  (*pAnalog).InConn[Port]   =  CONN_NXT_DUMB;
                }
              }
              else
              {
                if ((*pAnalog).InPin1[Port] < VtoC(IN1_NEAR_GND))
                { // nxt color sensor

                  (*pAnalog).InDcm[Port]      =  TYPE_NXT_COLOR;
#ifndef DISABLE_OLD_COLOR
                  (*pAnalog).InConn[Port]     =  CONN_NXT_COLOR;
                  InputPort[Port].State       =  DCM_NXT_COLOR_INIT;
#else
                  (*pAnalog).InConn[Port]     =  CONN_NXT_DUMB;
#endif
                }
                else
                {
                  if ((*pAnalog).InPin1[Port] > VtoC(IN1_NEAR_5V))
                  { // nxt touch sensor

                    (*pAnalog).InDcm[Port]    =  TYPE_NXT_TOUCH;
                    (*pAnalog).InConn[Port]   =  CONN_NXT_DUMB;
                  }
                  else
                  {
                    if (((*pAnalog).InPin1[Port] > VtoC(IN1_TOUCH_LOW)) && ((*pAnalog).InPin1[Port] < VtoC(IN1_TOUCH_HIGH)))
                    { // nxt touch sensor

                      InputPort[Port].Timer     =  0;
                      InputPort[Port].Value     =  (*pAnalog).InPin1[Port];
                      InputPort[Port].State     =  DCM_NXT_TOUCH_CHECK;
                    }
                    else
                    { // nxt sound sensor

                      (*pAnalog).InDcm[Port]    =  TYPE_NXT_SOUND;
                      (*pAnalog).InConn[Port]   =  CONN_NXT_DUMB;
                    }
                  }

                }
              }
            }
          }
          break;

          case DCM_NXT_TOUCH_CHECK :
          {
            if (++(InputPort[Port].Timer) >= (DCM_TOUCH_DELAY / DCM_TIMER_RESOLUTION))
            {
              InputPort[Port].State       =  DCM_CONNECTED_WAITING_FOR_PIN2_HIGH;
              if (((*pAnalog).InPin1[Port] > (InputPort[Port].Value - 10)) && ((*pAnalog).InPin1[Port] < (InputPort[Port].Value + 10)))
              { // nxt touch sensor

                (*pAnalog).InDcm[Port]    =  TYPE_NXT_TOUCH;
                (*pAnalog).InConn[Port]   =  CONN_NXT_DUMB;
              }
              else
              { // nxt sound sensor

                (*pAnalog).InDcm[Port]    =  TYPE_NXT_SOUND;
                (*pAnalog).InConn[Port]   =  CONN_NXT_DUMB;
              }
            }
          }
          break;

#ifndef DISABLE_OLD_COLOR

          case DCM_NXT_COLOR_INIT :
          {
            NxtcolorCmd[Port]           =  0;
            NxtColorCommStop(Port);

            InputPort[Port].Timer       =  0;
            InputPort[Port].State       =  DCM_NXT_COLOR_WAIT;
          }
          break;

          case DCM_NXT_COLOR_WAIT :
          {
            if (NxtcolorCmd[Port] == NxtcolorLatchedCmd[Port])
            {
              NxtColorCommStart(Port,InputPort[Port].Cmd);
              InputPort[Port].State       =  DCM_NXT_COLOR_BUSY;
            }
          }
          break;

          case DCM_NXT_COLOR_BUSY :
          {
            if (NxtColorCommReady(Port))
            {
              NxtcolorCmd[Port]           =  InputPort[Port].Cmd;
              InputPort[Port].Timer       =  0;
              InputPort[Port].State       =  DCM_CONNECTED_WAITING_FOR_PIN2_HIGH;

              pData   =  (UBYTE*)&((*pAnalog).NxtCol[Port]);

              for (Tmp = 0;Tmp < NXTCOLOR_BYTES;Tmp++)
              {
                *pData  =  NxtColorBuffer[Port][Tmp];
                pData++;
              }

              NxtColorCommStop(Port);

              NxtColorActive[Port]  =  1;
            }
            if (++(InputPort[Port].Timer) > (DCM_NXT_COLOR_TIMEOUT / DCM_TIMER_RESOLUTION))
            {
#ifdef DEBUG
              printk("i ! %d NXT Color sensor timeout\n",Port);
#endif
              InputPort[Port].Timer       =  0;
              InputPort[Port].State       =  DCM_CONNECTED_WAITING_FOR_PIN2_HIGH;
              NxtColorCommStop(Port);
            }
          }
          break;

#endif

          case DCM_CONNECTED_WAITING_FOR_PIN2_HIGH :
          {
            if (PINRead(Port,INPUT_PORT_PIN2))
            {
              if (++(InputPort[Port].Timer) >= (DCM_EVENT_STABLE_DELAY / DCM_TIMER_RESOLUTION))
              {
                InputPort[Port].Connected   =  0;
                InputPort[Port].State       =  DCM_INIT;
              }
            }
            else
            {
              InputPort[Port].Timer    =  0;
            }
          }
          break;

          case DCM_PIN1_LOADED :
          {
            if ((*pAnalog).InPin1[Port] > VtoC(IN1_NEAR_PIN2))
            {
              (*pAnalog).InDcm[Port]      =  TYPE_ERROR;
              (*pAnalog).InConn[Port]     =  CONN_ERROR;
            }
            else
            {
              if ((*pAnalog).InPin1[Port] < VtoC(IN1_NEAR_GND))
              {
                (*pAnalog).InDcm[Port]    =  TYPE_UNKNOWN;
                (*pAnalog).InConn[Port]   =  CONN_INPUT_UART;
              }
              else
              {
                (*pAnalog).InDcm[Port]    =  TYPE_UNKNOWN;
                (*pAnalog).InConn[Port]   =  CONN_INPUT_DUMB;
              }
            }
            InputPort[Port].Connected   =  1;
            InputPort[Port].Timer       =  0;
            InputPort[Port].State       =  DCM_CONNECTED_WAITING_FOR_PIN1_TO_FLOAT;
          }
          break;

          case DCM_CONNECTED_WAITING_FOR_PIN1_TO_FLOAT :
          {
            if ((*pAnalog).InPin1[Port] > VtoC(IN1_NEAR_5V))
            { // pin 1 floating

              if (++(InputPort[Port].Timer) >= (DCM_EVENT_STABLE_DELAY / DCM_TIMER_RESOLUTION))
              {
                InputPort[Port].Connected   =  0;
                InputPort[Port].State       =  DCM_INIT;
              }
            }
            else
            {
              InputPort[Port].Timer    =  0;
            }
          }
          break;

          case DCM_PIN6_HIGH :
          { // nxt IIC sensor

            (*pAnalog).InDcm[Port]      =  TYPE_NXT_IIC;
            (*pAnalog).InConn[Port]     =  CONN_NXT_IIC;
            InputPort[Port].Connected   =  1;
            InputPort[Port].Timer       =  0;
            InputPort[Port].State       =  DCM_CONNECTED_WAITING_FOR_PIN6_LOW;
          }
          break;

          case DCM_CONNECTED_WAITING_FOR_PIN6_LOW :
          {
            if (!(PINRead(Port,INPUT_PORT_PIN6)))
            {
              if (++(InputPort[Port].Timer) >= (DCM_EVENT_STABLE_DELAY / DCM_TIMER_RESOLUTION))
              {
                InputPort[Port].Connected   =  0;
                InputPort[Port].State       =  DCM_INIT;
              }
            }
            else
            {
              InputPort[Port].Timer    =  0;
            }
          }
          break;

          case DCM_PIN5_LOW :
          {
            (*pAnalog).InDcm[Port]      =  TYPE_ERROR;
            (*pAnalog).InConn[Port]     =  CONN_ERROR;
            InputPort[Port].Connected   =  1;
            InputPort[Port].Timer       =  0;
            InputPort[Port].State       =  DCM_CONNECTED_WAITING_FOR_PIN5_HIGH;
          }
          break;

          case DCM_CONNECTED_WAITING_FOR_PIN5_HIGH :
          {
            if (!(PINRead(Port,INPUT_PORT_PIN5)))
            {
              if (++(InputPort[Port].Timer) >= (DCM_EVENT_STABLE_DELAY / DCM_TIMER_RESOLUTION))
              {
                InputPort[Port].Connected   =  0;
                InputPort[Port].State       =  DCM_INIT;
              }
            }
            else
            {
              InputPort[Port].Timer    =  0;
            }
          }
          break;

          default :
          {
            InputPort[Port].State  =  DCM_INIT;
          }
          break;

        }
#ifdef DEBUG
        if (InputPort[Port].OldState != InputPort[Port].State)
        {
          InputPort[Port].OldState    =  InputPort[Port].State;

          printk("i   %d %s\n",Port,DcmStateText[InputPort[Port].State]);
        }
#endif

      }

    //*****************************************************************************

      for (Port = 0;Port < NO_OF_OUTPUT_PORTS;Port++)
      { // look at one port at a time

        switch (OutputPort[Port].State)
        {
          case DCM_INIT :
          { // set output port inactive

            OutputPortFloat(Port);
            (*pAnalog).OutDcm[Port]    =  TYPE_NONE;
            (*pAnalog).OutConn[Port]   =  CONN_NONE;
            OutputPort[Port].Timer     =  0;
            OutputPort[Port].Event     =  0;
            OutputPort[Port].State      =  DCM_FLOATING_DELAY;
          }
          break;

          case DCM_FLOATING_DELAY :
          { // wait for port pins to float

            if (++(OutputPort[Port].Timer) >= (DCM_FLOAT_DELAY / DCM_TIMER_RESOLUTION))
            {
              OutputPort[Port].Timer   =  0;
              OutputPort[Port].State   =  DCM_FLOATING;
            }
          }
          break;

          case DCM_FLOATING :
          { // pins floating - check and for connection event

            Event  =  0;

#ifdef FINALB
            if ((POUTRead(Port,OUTPUT_PORT_PIN6)))
            { // pin 6 low

              Event |=  (0x01 << OUTPUT_PORT_PIN6);
            }
#else
            if (!(POUTRead(Port,OUTPUT_PORT_PIN6)))
            { // pin 6 low

              Event |=  (0x01 << OUTPUT_PORT_PIN6);
            }
#endif
            if (((*pAnalog).OutPin5[Port] < VtoC(OUT5_BALANCE_LOW)) || ((*pAnalog).OutPin5[Port] > VtoC(OUT5_BALANCE_HIGH)))
            { // pin 5 out of balance

              Event |=  (0x01 << OUTPUT_PORT_VALUE);
            }

            if (OutputPort[Port].Event != Event)
            { // pins has changed - reset timer

              OutputPort[Port].Event   =  Event;
              OutputPort[Port].Timer   =  0;
            }

            if (OutputPort[Port].Event)
            { // some event

              if (++(OutputPort[Port].Timer) >= (DCM_CONNECT_STABLE_DELAY / DCM_TIMER_RESOLUTION))
              {
                // some event is stable - store value on connection 5

                OutputPort[Port].Value5Float  =  CtoV((*pAnalog).OutPin5[Port]);
                OutputPort[Port].Timer        =  0;
                POUTLow(Port,OUTPUT_PORT_PIN6);
                OutputPort[Port].State        =  DCM_WAITING_FOR_PIN6_LOW;
              }
            }
          }
          break;

          case DCM_WAITING_FOR_PIN6_LOW :
          {
            if (++(OutputPort[Port].Timer) >= (DCM_LOW_DELAY / DCM_TIMER_RESOLUTION))
            {
              OutputPort[Port].Value5Low    =  CtoV((*pAnalog).OutPin5[Port]);
              OutputPort[Port].State        =  DCM_CONNECTION;
              POUTFloat(Port,OUTPUT_PORT_PIN6);
            }
          }
          break;

          case DCM_CONNECTION :
          { // something is connected - try to evaluate

            OutputPort[Port].State      =  DCM_CONNECTED_WAITING_FOR_PORT_OPEN;
            Tmp  =  ADC_REF;
            Tmp +=  OutputPort[Port].Value5Float;
            Tmp -=  OutputPort[Port].Value5Low;

            if ((Tmp > (ADC_REF - 50)) && (Tmp < (ADC_REF + 50)))
            { // Value5Float is equal to Value5Low

              if ((OutputPort[Port].Value5Float >= OUT5_BALANCE_LOW) && (OutputPort[Port].Value5Float <= OUT5_BALANCE_HIGH) && (OutputPort[Port].Event & (0x01 << OUTPUT_PORT_PIN6)))
              { // NXT TOUCH SENSOR, NXT SOUND SENSOR or NEW UART SENSOR

                (*pAnalog).OutDcm[Port]     =  TYPE_ERROR;
                (*pAnalog).OutConn[Port]    =  CONN_ERROR;
                OutputPort[Port].Connected  =  1;
              }
              else
              {
                if (OutputPort[Port].Value5Float < OUT5_NEAR_GND)
                { // NEW DUMB SENSOR

                  (*pAnalog).OutDcm[Port]     =  TYPE_ERROR;
                  (*pAnalog).OutConn[Port]    =  CONN_ERROR;
                  OutputPort[Port].Connected  =  1;
                }
                else
                {
                  if ((OutputPort[Port].Value5Float >= OUT5_LIGHT_LOW) && (OutputPort[Port].Value5Float <= OUT5_LIGHT_HIGH))
                  { // NXT LIGHT SENSOR

                    (*pAnalog).OutDcm[Port]     =  TYPE_ERROR;
                    (*pAnalog).OutConn[Port]    =  CONN_ERROR;
                    OutputPort[Port].Connected  =  1;
                  }
                  else
                  {
                    if ((OutputPort[Port].Value5Float >= OUT5_IIC_LOW) && (OutputPort[Port].Value5Float <= OUT5_IIC_HIGH))
                    { // NXT IIC SENSOR

                      (*pAnalog).OutDcm[Port]     =  TYPE_ERROR;
                      (*pAnalog).OutConn[Port]    =  CONN_ERROR;
                      OutputPort[Port].Connected  =  1;
                    }
                    else
                    {
                      if (OutputPort[Port].Value5Float < OUT5_BALANCE_LOW)
                      {
                        if (OutputPort[Port].Value5Float > OUT5_MINITACHO_HIGH2)
                        {
                          (*pAnalog).OutDcm[Port]     =  TYPE_NEWTACHO;
                        }
                        else
                        {
                          if (OutputPort[Port].Value5Float > OUT5_MINITACHO_LOW2)
                          {
                            (*pAnalog).OutDcm[Port]   =  TYPE_MINITACHO;
                          }
                          else
                          {
                            (*pAnalog).OutDcm[Port]   =  TYPE_TACHO;
                          }
                        }
                        (*pAnalog).OutConn[Port]      =  CONN_OUTPUT_TACHO;
                        OutputPort[Port].Connected    =  1;
                      }
                      else
                      {
                        POUTHigh(Port,OUTPUT_PORT_PIN5W);
                        OutputPort[Port].State        =  DCM_WAITING_FOR_PIN5_LOW;
                      }
                    }
                  }
                }
              }

            }
            else
            { // Value5Float is NOT equal to Value5Low

              if ((OutputPort[Port].Value5Low > OUT5_NEAR_GND) && (OutputPort[Port].Value5Low < OUT5_BALANCE_LOW))
              { // NEW ACTUATOR

                (*pAnalog).OutPin5Low[Port]   =  OutputPort[Port].Value5Low;
                (*pAnalog).OutDcm[Port]       =  TYPE_UNKNOWN;
                (*pAnalog).OutConn[Port]      =  CONN_OUTPUT_DUMB;
                OutputPort[Port].Connected    =  1;
              }
              else
              {
                (*pAnalog).OutDcm[Port]       =  TYPE_ERROR;
                (*pAnalog).OutConn[Port]      =  CONN_ERROR;
                OutputPort[Port].Connected    =  1;
              }

            }
            OutputPort[Port].Timer      =  0;
#ifdef DEBUG
            if (OutputPort[Port].Connected)
            {
              printk("\r\no    %d Type = %c, Float = %u, Low = %u\r\n",Port,(char)(*pAnalog).OutDcm[Port],(unsigned int)OutputPort[Port].Value5Float,(unsigned int)OutputPort[Port].Value5Low);
            }
#endif
          }
          break;

          case DCM_WAITING_FOR_PIN5_LOW :
          {
            if (++(OutputPort[Port].Timer) >= (DCM_LOW_DELAY / DCM_TIMER_RESOLUTION))
            {
              OutputPort[Port].Value5Low    =  CtoV((*pAnalog).OutPin5[Port]);
              OutputPort[Port].State        =  DCM_CONNECTION;
              POUTLow(Port,OUTPUT_PORT_PIN5W);
              (*pAnalog).OutConn[Port]      =  CONN_OUTPUT_TACHO;
              if (OutputPort[Port].Value5Low < OUT5_MINITACHO_LOW1)
              {
                (*pAnalog).OutDcm[Port]     =  TYPE_ERROR;
                (*pAnalog).OutConn[Port]    =  CONN_ERROR;
              }
              else
              {
                if (OutputPort[Port].Value5Low < OUT5_MINITACHO_HIGH1)
                {
                  (*pAnalog).OutDcm[Port]   =  TYPE_MINITACHO;
                }
                else
                {
                  (*pAnalog).OutDcm[Port]   =  TYPE_TACHO;
                }
              }
              OutputPort[Port].Connected    =  1;
#ifdef DEBUG
              printk("\r\no   %d Type = %03u, Float = %u, Low = %u\r\n",Port,(char)(*pAnalog).OutDcm[Port],(unsigned int)OutputPort[Port].Value5Float,(unsigned int)OutputPort[Port].Value5Low);
#endif
              OutputPort[Port].State        =  DCM_CONNECTED_WAITING_FOR_PORT_OPEN;
            }
          }
          break;

          case DCM_CONNECTED_WAITING_FOR_PORT_OPEN :
          {
            if (((*pAnalog).OutPin5[Port] < VtoC(OUT5_BALANCE_LOW)) || ((*pAnalog).OutPin5[Port] > VtoC(OUT5_BALANCE_HIGH)))
            { // connection 5 out of balance

              OutputPort[Port].Timer      =  0;
            }
#ifdef FINALB
            if ((POUTRead(Port,OUTPUT_PORT_PIN6)))
            { // pin 6 low

              OutputPort[Port].Timer      =  0;
            }
#else
            if (!(POUTRead(Port,OUTPUT_PORT_PIN6)))
            { // pin 6 low

              OutputPort[Port].Timer      =  0;
            }
#endif
            if (++(OutputPort[Port].Timer) >= (DCM_EVENT_STABLE_DELAY / DCM_TIMER_RESOLUTION))
            {
              OutputPort[Port].Connected  =  0;
              OutputPort[Port].State      =  DCM_INIT;
            }
          }
          break;

          default :
          {
            OutputPort[Port].State        =  DCM_INIT;
          }
          break;

        }
#ifdef DEBUG
        if (OutputPort[Port].OldState != OutputPort[Port].State)
        {
          OutputPort[Port].OldState    =  OutputPort[Port].State;

          printk("o   %d %s\n",Port,DcmStateText[OutputPort[Port].State]);
        }
#endif

      }
    }
    break;

  }

  return (HRTIMER_RESTART);
}


static ssize_t Device3Write(struct file *File,const char *Buffer,size_t Count,loff_t *Data)
{
  char    Buf[INPUTS + 1];
  UBYTE   Port;
  UBYTE   Char;
  int     Lng     = 0;

  if (Count >= INPUTS)
  {
    Lng  =  Count;
    copy_from_user(Buf,Buffer,INPUTS);

    for (Port = 0;Port < NO_OF_INPUT_PORTS;Port++)
    {
      Char  =  Buf[Port];
      switch (Char)
      {
        case '-' :
        { // do nothing
        }
        break;

        case 'f' :
        { // float

          InputPortFloat(Port);
        }
        break;

        default :
        {
          if (InputPort[Port].Connected)
          {
            if ((Char & 0xF8) == '0')
            { // 0,1,2,3,4,5,6,7


              if (Char & 0x01)
              { // pin 1

                PINHigh(Port,INPUT_PORT_PIN1)
              }
              else
              {
                PINLow(Port,INPUT_PORT_PIN1)
              }
              if (Char & 0x02)
              { // pin 5

                PINHigh(Port,INPUT_PORT_PIN5)
              }
              else
              {
                PINLow(Port,INPUT_PORT_PIN5)
              }
            }
#ifndef DISABLE_OLD_COLOR
            else
            {
              if ((Char >= 0x0D) && (Char <= 0x11))
              { // NXT color sensor setup

                InputPort[Port].Cmd    =  Char;
                InputPort[Port].Timer  =  0;
                InputPort[Port].State  =  DCM_NXT_COLOR_INIT;

              }
            }
#endif
          }
        }
        break;

      }
    }
  }

  return (Lng);
}


static ssize_t Device3Read(struct file *File,char *Buffer,size_t Count,loff_t *Offset)
{
  int     Lng     = 0;

  if (Count > 9)
  {
    while (Lng < NO_OF_INPUT_PORTS)
    {
      Buffer[Lng]  =  (*pAnalog).InDcm[Lng];
      Lng++;
    }
    while (Lng < INPUTS)
    {
      Buffer[Lng]  =  TYPE_NONE;
      Lng++;
    }
    while (Lng < (INPUTS + NO_OF_OUTPUT_PORTS))
    {
      Buffer[Lng]  =  (*pAnalog).OutDcm[Lng - INPUTS];
      Lng++;
    }
    while (Lng < (INPUTS + OUTPUTS))
    {
      Buffer[Lng]  =  TYPE_NONE;
      Lng++;
    }
    Buffer[Lng++]  =  '\r';
    Buffer[Lng++]  =  0;
  }

  return (Lng);
}


static    const struct file_operations Device3Entries =
{
  .owner        = THIS_MODULE,
  .read         = Device3Read,
  .write        = Device3Write
};


static    struct miscdevice Device3 =
{
  MISC_DYNAMIC_MINOR,
  DEVICE3_NAME,
  &Device3Entries
};


static int Device3Init(void)
{
  int     Result = -1;
  int     Tmp;

  Result  =  misc_register(&Device3);
  if (Result)
  {
    printk("  %s device register failed\n",DEVICE3_NAME);
  }
  else
  {
    for (Tmp = 0;Tmp < NO_OF_INPUT_PORTS;Tmp++)
    {
      InputPort[Tmp]  =  InputPortDefault;
    }
    for (Tmp = 0;Tmp < NO_OF_OUTPUT_PORTS;Tmp++)
    {
      OutputPort[Tmp]  =  OutputPortDefault;
    }

    Device3State  =  0;
    TestMode      =  0;


    Device3Time  =  ktime_set(0,DCM_TIMER_RESOLUTION * 1000000);
    hrtimer_init(&Device3Timer,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
    Device3Timer.function  =  Device3TimerInterrupt1;
    hrtimer_start(&Device3Timer,Device3Time,HRTIMER_MODE_REL);

#ifdef DEBUG
    printk("  %s device register succes\n",DEVICE3_NAME);
#endif
  }

  return (Result);
}


static void Device3Exit(void)
{
  int     Tmp;

  hrtimer_cancel(&Device3Timer);
  for (Tmp = 0;Tmp < NO_OF_INPUT_PORTS;Tmp++)
  {
    InputPortFloat(Tmp);
  }
  misc_deregister(&Device3);
#ifdef DEBUG
  printk("  %s device unregistered\n",DEVICE3_NAME);
  printk("  %s memory unmapped\n",DEVICE3_NAME);
#endif
}


// MODULE *********************************************************************


#ifndef PCASM
module_param (HwId, charp, 0);
#endif

static int ModuleInit(void)
{
  Hw  =  HWID;

  if (Hw < PLATFORM_START)
  {
    Hw  =  PLATFORM_START;
  }
  if (Hw > PLATFORM_END)
  {
    Hw  =  PLATFORM_END;
  }

#ifdef DEBUG
  printk("%s init started\n",MODULE_NAME);
#endif
  if (request_mem_region(DA8XX_GPIO_BASE,0xD8,MODULE_NAME) >= 0)
  {
    GpioBase  =  (void*)ioremap(DA8XX_GPIO_BASE,0xD8);
    if (GpioBase != NULL)
    {
#ifdef DEBUG
      printk("%s gpio address mapped\n",MODULE_NAME);
#endif

      InitGpio();

//      IGENOn;
      BATENOn;

      Device1Init();
      Device2Init();
      Device3Init();


    }
  }

  return (0);
}


static void ModuleExit(void)
{
#ifdef DEBUG
  printk("%s exit started\n",MODULE_NAME);
#endif

  BATENOff;

//! \todo fix FHOLD
  POUTHigh(2,OUTPUT_PORT_PIN5W);

  Device3Exit();
  Device2Exit();
  Device1Exit();

  iounmap(GpioBase);
}


