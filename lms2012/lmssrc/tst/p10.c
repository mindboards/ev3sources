UBYTE     prg[] =
{ // p10
  // IMAGE header
  PROGRAMHeader(0,1,0),                 //      VersionInfo,NumberOfObjects,GlobalBytes
  VMTHREADHeader(28,1),                 //      OffsetToInstructions,LocalBytes
                                        //
                                        //      VMTHREAD1
                                        //      {
  opUI_WRITE,LC0(PUT_STRING),LCS,           //        UI_WRITE(STRING,"    Copy Input 1 to Output A\r\n")
    ' ',' ',' ',' ',                    //
    'C','o','p','y',' ','I','n','p',    //
    'u','t',' ','1',' ','t','o',        //
    ' ','O','u','t','p','u','t',        //
    ' ','A',                            //
    '\r','\n',0,                        //
  opUI_FLUSH,                           //        UI_FLUSH
                                        //
  opOUTPUT_START,LC0(0),LC0(1),         //        opOUTPUT_START(0,1)
                                        //
                                        //        do
                                        //        {
  opINPUT_READ,LC0(0),LC0(0),LC0(0),    //          opINPUT_READ(0,0,0,0,LV0)
  LC0(0),LV0(0),                	      //
  opOUTPUT_POWER,LC0(0),LC0(1),LV0(0),  //          opOUTPUT_POWER(0,1,LV0)
                                        //        }
  opJR,LC0(-15),                        //        while (TRUE)
                                        //
  opOBJECT_END                          //      }
};

