/*! \page subparexample1 Calling Subroutine with Parameters
 *
 *  This program shows how to call byte code subroutine with parameters
 *
 *  <hr size="1"/>
 *  \verbatim */

UBYTE     prg[] =                       //  p11
{                                       //
  PROGRAMHeader(0,2,2),                 //      VersionInfo,NumberOfObjects,GlobalBytes
  VMTHREADHeader(0,2),                  //      OffsetToInstructions,LocalBytes
  SUBCALLHeader(0,2),                   //      OffsetToInstructions,LocalBytes
                                        //
                                        //      VMTHREAD1
                                        //      {
  opMOVE8_8,LC0(1),LV0(0),              //        LV0  =  1
  opCALL,LC0(2),LC0(2),LV0(0),LV0(1),   //        SUBCALL(2,2,LV0,LV1)
  opUI_WRITE,LC0(VALUE8),LV0(1),        //        UI_WRITE(VALUE8,LV1)
  opUI_WRITE,LC0(PUT_STRING),LCS,'\r',  //        UI_WRITE(PUT_STRING,"\r\n")
    '\n',0,
  opUI_FLUSH,                           //        UI_FLUSH
  opOBJECT_END,                         //      }
                                        //
  2,IN_8,OUT_8,                         //      SUBCALL2(InDATA8,OutDATA8)
                                        //      {
  opUI_WRITE,LC0(VALUE8),LV0(0),        //        UI_WRITE(VALUE8,LV0)
  opUI_WRITE,LC0(PUT_STRING),LCS,'\r',  //        UI_WRITE(PUT_STRING,"\r\n")
    '\n',0,
  opUI_FLUSH,                           //        UI_FLUSH
  opMOVE8_8,LV0(0),LV0(1),              //        LV1  =  2
  opRETURN,                             //      }
  opOBJECT_END,                         //
};

/* \endverbatim \ref subpar "Parameter Encoding"
*/
