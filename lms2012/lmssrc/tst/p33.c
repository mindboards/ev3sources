UBYTE     prg[] =
{ // p3
  // IMAGE header
  PROGRAMHeader(0,1,0),                 //      VersionInfo,NumberOfObjects,GlobalBytes
  VMTHREADHeader(28,4),                 //      OffsetToInstructions,LocalBytes
                                        //
                                        //      VMTHREAD1
                                        //      {
  opUI_WRITE,LC0(PUT_STRING),LCS,           //        UI_WRITE(STRING,"    Print input changes\r\n")
    ' ',' ',' ',' ',                    //
    'P','r','i','n','t',' ',            //
    'i','n','p','u','t',' ',            //
    'c','h','a','n','g','e','s',        //
    '\r','\n',0,                        //
  opUI_FLUSH,                           //        UI_FLUSH
                                        //
  opINPUT_READSI,LC0(0),LC0(0),LC0(0),  //        INPUT_READ(0,0,0,0,LV0)
    LC0(0),LV0(0),              	      //
                                        //        do
                                        //        {
                                        //          do
                                        //          {
  opINPUT_READSI,LC0(0),LC0(0),LC0(0),  //            INPUT_READ(0,0,0,0,LV4)
    LC0(0),LV0(4),              	      //
                                        //          }
  opJR_EQF,LV0(0),LV0(4),LC0(-10),      //          while (LV0 == LV4)
                                        //
  opMOVEF_F,LV0(4),LV0(0),              //          LV0  =  LV4
					                              //
  opUI_WRITE,LC0(VALUEF),LV0(4),        //          UI_WRITE(VALUEF,LV4)
  opUI_WRITE,LC0(PUT_STRING),LCS,'\r',  //          UI_WRITE(PUT_STRING,"\r\n")
    '\n',0,                             //
  opUI_FLUSH,                           //          UI_FLUSH
                                        //        }
  opJR,LC0(-25),                        //        while (TRUE)
  opOBJECT_END                          //      }

};

