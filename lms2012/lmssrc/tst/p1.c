//****************************************************************************
// Disassembler Listing
//****************************************************************************

UBYTE     prg[] =
{
  /*    0 */  PROGRAMHeader(0.46,2,8),
  /*   16 */  VMTHREADHeader(40,5),      // Object 1  
  /*   28 */  BLOCKHeader(65,1,1),       // Object 2  

  /* Object  1 (VMTHREAD) [40..65] */

  /*   40 */  opMOVE32_32,LC0(0),LV0(0),
  /*   43 */  opCP_EQ32,LC4(100000),LV0(0),LV0(4),
  /*   51 */  opOBJECT_TRIG,LC0(2),
  /*   53 */  opOBJECT_WAIT,LC0(2),
  /*   55 */  opJR_TRUE,LV0(4),LC0(6),
  /*   58 */  opADD32,LV0(0),LC0(1),LV0(0),
  /*   62 */  opJR,LC0(-21),
  /*   64 */  opOBJECT_END,

  /* Object  2 (BLOCK) [65..67] */

  /*   65 */  opNOP,
  /*   66 */  opOBJECT_END,
};

//****************************************************************************

