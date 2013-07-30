UBYTE     prg[] =
{ // p7
  // IMAGE header
  PROGRAMHeader(0,1,8),                 //      VersionInfo,NumberOfObjects,GlobalBytes
  VMTHREADHeader(28,0),                 //      OffsetToInstructions,LocalBytes
                                        //
                                        //      VMTHREAD1
                                        //      {
  opUI_WRITE,LC0(PUT_STRING),LCS,           //        UI_WRITE(STRING,"    DATAF multiply\r\n")
    ' ',' ',' ',' ',                    //
    'D','A','T','A','F',' ',            //
    'm','u','l','t','i','p','l','y',    //
    '\r','\n',0,                        //
  opUI_FLUSH,                           //        UI_FLUSH
                                        //        do
                                        //        {
    opMOVE8_F,LC0(3),GV0(0),            //          GV0  =  3
    opMOVE8_F,LC0(7),GV0(4),            //          GV4  =  7
                                        //
    opMULF,GV0(0),GV0(4),GV0(0),        //          GV0  =  GV0 * GV4
    opMULF,GV0(0),GV0(4),GV0(0),        //          GV0  =  GV0 * GV4
    opMULF,GV0(0),GV0(4),GV0(0),        //          GV0  =  GV0 * GV4
    opMULF,GV0(0),GV0(4),GV0(0),        //          GV0  =  GV0 * GV4
    opMULF,GV0(0),GV0(4),GV0(0),        //          GV0  =  GV0 * GV4
    opMULF,GV0(0),GV0(4),GV0(0),        //          GV0  =  GV0 * GV4
    opMULF,GV0(0),GV0(4),GV0(0),        //          GV0  =  GV0 * GV4
    opMULF,GV0(0),GV0(4),GV0(0),        //          GV0  =  GV0 * GV4
    opMULF,GV0(0),GV0(4),GV0(0),        //          GV0  =  GV0 * GV4
    opMULF,GV0(0),GV0(4),GV0(0),        //          GV0  =  GV0 * GV4
                                        //        }
    opJR,LC1(-43),                      //        while (TRUE)
    opOBJECT_END                        //      }
};

