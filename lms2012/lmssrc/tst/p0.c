UBYTE     prg[] =
{ // p0
  // IMAGE header
  PROGRAMHeader(0,1,0),                 //      VersionInfo,NumberOfObjects,GlobalBytes
  VMTHREADHeader(0,0),                  //      OffsetToInstructions,LocalBytes
                                        //
                                        //      VMTHREAD1
                                        //      {
  opUI_WRITE,LC0(PUT_STRING),LCS,           //        UI_WRITE(STRING,"    Do nops\r\n")
    ' ',' ',' ',' ',                    //
    'D','o',' ','n','o','p','s',        //
    '\r','\n',0,                        //
  opUI_FLUSH,                           //        UI_FLUSH
                                        //        do
                                        //        {
  opNOP,                                //          nop
  opNOP,                                //          nop
  opNOP,                                //          nop
  opNOP,                                //          nop
  opNOP,                                //          nop
  opNOP,                                //          nop
  opNOP,                                //          nop
  opNOP,                                //          nop
  opNOP,                                //          nop
  opNOP,                                //          nop
                                        //        }
  opJR,LC0(-12),                        //        while (TRUE)
                                        //
  opOBJECT_END                          //      }
};

