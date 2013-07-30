UBYTE     prg[] =
{ // p2
  // IMAGE header
  PROGRAMHeader(0,1,0),                 //      VersionInfo,NumberOfObjects,GlobalBytes
  VMTHREADHeader(28,0),                 //      OffsetToInstructions,LocalBytes
                                        //
                                        //      VMTHREAD1
                                        //      {
  opUI_WRITE,LC0(PUT_STRING),LCS,           //        UI_WRITE(STRING,"    Make 10 pulses\r\n")
    ' ',' ',' ',' ',                    //
    'M','a','k','e',' ','1','0',' ',    //
    'p','u','l','s','e','s',            //
    '\r','\n',0,                        //
                                        //
  opUI_FLUSH,                           //        UI_FLUSH
  opOUTPUT_SPEED,LC0(0),LC0(1),LC1(50), //        OUTPUT_SPEED(0,1,50)
  opOUTPUT_SPEED,LC0(0),LC0(1),LC0(0),  //        OUTPUT_SPEED(0,1,0)
  opOUTPUT_SPEED,LC0(0),LC0(1),LC1(50), //        OUTPUT_SPEED(0,1,50)
  opOUTPUT_SPEED,LC0(0),LC0(1),LC0(0),  //        OUTPUT_SPEED(0,1,0)
  opOUTPUT_SPEED,LC0(0),LC0(1),LC1(50), //        OUTPUT_SPEED(0,1,50)
  opOUTPUT_SPEED,LC0(0),LC0(1),LC0(0),  //        OUTPUT_SPEED(0,1,0)
  opOUTPUT_SPEED,LC0(0),LC0(1),LC1(50), //        OUTPUT_SPEED(0,1,50)
  opOUTPUT_SPEED,LC0(0),LC0(1),LC0(0),  //        OUTPUT_SPEED(0,1,0)
  opOUTPUT_SPEED,LC0(0),LC0(1),LC1(50), //        OUTPUT_SPEED(0,1,50)
  opOUTPUT_SPEED,LC0(0),LC0(1),LC0(0),  //        OUTPUT_SPEED(0,1,0)
  opOUTPUT_SPEED,LC0(0),LC0(1),LC1(50), //        OUTPUT_SPEED(0,1,50)
  opOUTPUT_SPEED,LC0(0),LC0(1),LC0(0),  //        OUTPUT_SPEED(0,1,0)
  opOUTPUT_SPEED,LC0(0),LC0(1),LC1(50), //        OUTPUT_SPEED(0,1,50)
  opOUTPUT_SPEED,LC0(0),LC0(1),LC0(0),  //        OUTPUT_SPEED(0,1,0)
  opOUTPUT_SPEED,LC0(0),LC0(1),LC1(50), //        OUTPUT_SPEED(0,1,50)
  opOUTPUT_SPEED,LC0(0),LC0(1),LC0(0),  //        OUTPUT_SPEED(0,1,0)
  opOUTPUT_SPEED,LC0(0),LC0(1),LC1(50), //        OUTPUT_SPEED(0,1,50)
  opOUTPUT_SPEED,LC0(0),LC0(1),LC0(0),  //        OUTPUT_SPEED(0,1,0)
  opOUTPUT_SPEED,LC0(0),LC0(1),LC1(50), //        OUTPUT_SPEED(0,1,50)
  opOUTPUT_SPEED,LC0(0),LC0(1),LC0(0),  //        OUTPUT_SPEED(0,1,0)
  opOBJECT_END                          //      }
};

