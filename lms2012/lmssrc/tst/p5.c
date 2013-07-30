/*! \page programexample1 Line Follower
 *
 *  Example program
 *
 *  <hr size="1"/>
 *  \verbatim */

UBYTE     prg[] =                       //  p5.c
{                                       //
  PROGRAMHeader(0,1,0),                 // 0    VersionInfo,NumberOfObjects,GlobalBytes
  VMTHREADHeader(0,3),                  // 12   OffsetToInstructions,LocalBytes
                                        //
                                        // 24   VMTHREAD1
                                        //      {
  opUI_WRITE,LC0(PUT_STRING),LCS,           //        UI_WRITE(STRING,"    LineFollower\r\n")
    ' ',' ',' ',' ',                    //
    'L','i','n','e','F','o','l',        //
    'l','o','w','e','r',                //
    '\r','\n',0,                        //
  opUI_FLUSH,                           //        UI_FLUSH
                                        //
  opINPUT_READ,LC0(0),LC0(1),           //        INPUT_READ(0,0,2,LV0)       // Read light sensor
    LC1(2),LC0(1),LV0(0),               //
  opDIV8,LV0(0),LC0(2),LV0(0),          //        LV0 /=  2
  opADD8,LV0(0),LC0(15),LV0(0),         //        LV0 +=  15
                                        //
                                        //        do
                                        //        {
  opINPUT_READ,LC0(0),LC0(1),           //          INPUT_READ(0,0,2)         // Read light sensor
    LC1(2),LC0(1),LV0(1),               //
  opSUB8,LV0(1),LV0(0),LV0(1),          //          LV1    -=  LV0
  opDIV8,LV0(1),LC0(2),LV0(1),          //          LV1    /=  2
  opADD8,LC1(40),LV0(1),LV0(2),         //          LV2     =  40 + LV1
  opSUB8,LC1(40),LV0(1),LV0(3),         //          LV3     =  40 - LV1
                                        //
  opOUTPUT_POWER,LC0(0),LC0(1),LV0(3),  //          OUTPUT_POWER(0,0x1,LV3)   // Motor 1 = LV3
  opOUTPUT_POWER,LC0(0),LC0(2),LV0(2),  //          OUTPUT_POWER(0,0x2,LV2)   // Motor 2 = LV2
                                        //
  opOUTPUT_START,LC0(0),LC0(3),         //          OUTPUT_START(0,0x3)
                                        //
                                        //        }
  opJR,LC1(-39),                        //        while (TRUE)
  opOBJECT_END                          //      }
};

/* \endverbatim
*/


/*
// Connections
#define   OUTPUT_A                      LC0(0),LC0(1)
#define   OUTPUT_B                      LC0(0),LC0(2)
#define   OUTPUT_AB                     LC0(0),LC0(3)
#define   INPUT_1                       LC0(0),LC0(0)

// Types
#define   LIGHT_TYPE                    LC0(2),LC0(1)

// Labels
#define   _LOOP                         opLABEL,LC0(1)
#define   LOOP                          LAB1(1)

// Constants
#define   MID_PWR                       LC1(40)
#define   OFFSET                        LC0(15)
#define   HALF                          LC0(2)

// Globals
#define   GLOBALS                       0

// Locals 1
#define   LOCALS_1                      4
#define   Offset                        LV0(0)
#define   Light                         LV0(1)
#define   Power_A                       LV0(2)                       
#define   Power_B                       LV0(3)

IMGDATA   prg[] =                                                 //  p5.c
{
  PROGRAMHeader(0,1,GLOBALS),
  VMTHREADHeader(0,LOCALS_1),  // Object 1

  // Object  1 (VMTHREAD)

  opUI_WRITE,LC0(PUT_STRING),LCS,' ',' ',' ',' ','L','i','n','e','F','o','l','l','o','w','e','r','\r','\n',0,
  opUI_FLUSH,

  // Calibrate
  opINPUT_READ, INPUT_1, LIGHT_TYPE, Offset,
  opDIV8, Offset, HALF, Offset,
  opADD8, Offset, OFFSET, Offset,
  opOUTPUT_START, OUTPUT_AB,

  _LOOP,
  //
    // Read input
    opINPUT_READ, INPUT_1, LIGHT_TYPE, Light,
  
    // Calculate power
    opSUB8, Light, Offset, Light,
    opDIV8, Light, HALF, Light,
    opADD8, MID_PWR, Light, Power_B,
    opSUB8, MID_PWR, Light, Power_A,

    // Set outputs
    opOUTPUT_POWER, OUTPUT_A, Power_A,
    opOUTPUT_POWER, OUTPUT_B, Power_B,
  //
  opJR, LOOP,

  opOBJECT_END
};
*/
