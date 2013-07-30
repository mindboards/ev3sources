UBYTE     prg[] =
{ // p8
  // IMAGE header
  PROGRAMHeader(0,1,4),                 //      VersionInfo,NumberOfObjects,GlobalBytes
  VMTHREADHeader(28,0),                 //      OffsetToInstructions,LocalBytes
                                        //
                                        //      VMTHREAD1
                                        //      {
  opUI_WRITE,LC0(PUT_STRING),LCS,           //        UI_WRITE(STRING,"    DATA16 multiply\r\n")
    ' ',' ',' ',' ',                    //
    'D','A','T','A','1','6',' ',        //
    'm','u','l','t','i','p','l','y',    //
    '\r','\n',0,                        //
  opUI_FLUSH,                           //        UI_FLUSH
                                        //        do
                                        //        {
    opMOVE8_16,LC0(3),GV0(0),           //          GV0  =  3
    opMOVE8_16,LC0(7),GV0(2),           //          GV2  =  7
                                        //
    opMUL16,GV0(0),GV0(2),GV0(0),       //          GV0  =  GV0 * GV2
    opMUL16,GV0(0),GV0(2),GV0(0),       //          GV0  =  GV0 * GV2
    opMUL16,GV0(0),GV0(2),GV0(0),       //          GV0  =  GV0 * GV2
    opMUL16,GV0(0),GV0(2),GV0(0),       //          GV0  =  GV0 * GV2
    opMUL16,GV0(0),GV0(2),GV0(0),       //          GV0  =  GV0 * GV2
    opMUL16,GV0(0),GV0(2),GV0(0),       //          GV0  =  GV0 * GV2
    opMUL16,GV0(0),GV0(2),GV0(0),       //          GV0  =  GV0 * GV2
    opMUL16,GV0(0),GV0(2),GV0(0),       //          GV0  =  GV0 * GV2
    opMUL16,GV0(0),GV0(2),GV0(0),       //          GV0  =  GV0 * GV2
    opMUL16,GV0(0),GV0(2),GV0(0),       //          GV0  =  GV0 * GV2
                                        //        }
    opJR,LC1(-43),                      //        while (TRUE)
    opOBJECT_END                        //      }
};

