UBYTE     prg[] =
{ // p9
  // IMAGE header
  PROGRAMHeader(0,1,2),                 //      VersionInfo,NumberOfObjects,GlobalBytes
  VMTHREADHeader(28,0),                 //      OffsetToInstructions,LocalBytes
                                        //
                                        //      VMTHREAD1
                                        //      {
  opUI_WRITE,LC0(PUT_STRING),LCS,           //        UI_WRITE(STRING,"    DATA8 multiply\r\n")
    ' ',' ',' ',' ',                    //
    'D','A','T','A','8',' ',            //
    'm','u','l','t','i','p','l','y',    //
    '\r','\n',0,                        //
  opUI_FLUSH,                           //        UI_FLUSH
                                        //        do
                                        //        {
    opMOVE8_8,LC0(3),GV0(0),            //          GV0  =  3
    opMOVE8_8,LC0(7),GV0(1),            //          GV1  =  7
                                        //
    opMUL8,GV0(0),GV0(1),GV0(0),        //          GV0  =  GV0 * GV1
    opMUL8,GV0(0),GV0(1),GV0(0),        //          GV0  =  GV0 * GV1
    opMUL8,GV0(0),GV0(1),GV0(0),        //          GV0  =  GV0 * GV1
    opMUL8,GV0(0),GV0(1),GV0(0),        //          GV0  =  GV0 * GV1
    opMUL8,GV0(0),GV0(1),GV0(0),        //          GV0  =  GV0 * GV1
    opMUL8,GV0(0),GV0(1),GV0(0),        //          GV0  =  GV0 * GV1
    opMUL8,GV0(0),GV0(1),GV0(0),        //          GV0  =  GV0 * GV1
    opMUL8,GV0(0),GV0(1),GV0(0),        //          GV0  =  GV0 * GV1
    opMUL8,GV0(0),GV0(1),GV0(0),        //          GV0  =  GV0 * GV1
    opMUL8,GV0(0),GV0(1),GV0(0),        //          GV0  =  GV0 * GV1
                                        //        }
    opJR,LC1(-43),                      //        while (TRUE)
    opOBJECT_END                        //      }
};

