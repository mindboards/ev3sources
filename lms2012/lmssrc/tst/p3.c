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
  opINPUT_READ,LC0(0),LC0(0),LC0(0),    //        INPUT_READ(0,0,0,0,LV0)
    LC0(0),LV0(0),              	      //
                                        //        do
                                        //        {
                                        //          do
                                        //          {
  opINPUT_READ,LC0(0),LC0(0),LC0(0),    //            INPUT_READ(0,0,0,0,LV1)
    LC0(0),LV0(1),              	      //
                                        //          }
  opJR_EQ8,LV0(0),LV0(1),LC0(-10),      //          while (LV0 == LV1)
                                        //
  opMOVE8_8,LV0(1),LV0(0),              //          LV0  =  LV1
					                              //
  opUI_WRITE,LC0(VALUE8),LV0(1),        //          UI_WRITE(VALUE8,LV1)
  opUI_WRITE,LC0(PUT_STRING),LCS,'\r',  //          UI_WRITE(PUT_STRING,"\r\n")
    '\n',0,
  opUI_FLUSH,                           //          UI_FLUSH
  opUI_WRITE,LC0(LED),LC0(1),LV0(1),	  //          UI_WRITE(LED,LED_LR,1)	      // Turn on left red
                                        //        }
  opJR,LC0(-29),                        //        while (TRUE)
  opOBJECT_END                          //      }

};

