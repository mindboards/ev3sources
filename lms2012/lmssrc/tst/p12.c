/*! \page vmthreadsexample1 Parallel Loops
 *
 *  This program shows how to make two VMTHREADs run in "parallel"
 *
 *  <hr size="1"/>
 *  \verbatim */
//
//
//  (VMTHREAD1--WRITE--START(2)--------------WRITE(One)--DELAY(1)--LOOP10)-----)
//                           \                  ^                      |
//                            \                 |                      |
//                             \                `----------------------'
//                              \
//                               (VMTHREAD2--WRITE(One)--DELAY(1)--LOOP10)-----)
//                                              ^                      |
//                                              |                      |
//                                              `----------------------'
//
UBYTE     prg[] =                                 //  p12.c
{                                                 //
  PROGRAMHeader(0,2,0),                           //  VersionInfo,NumberOfObjects,GlobalBytes
  VMTHREADHeader(40,5),                           //  OffsetToInstructions,LocalBytes
  VMTHREADHeader(106,5),                          //  OffsetToInstructions,LocalBytes
                                                  //
                                                  //  VMTHREAD1
                                                  //  {
  opUI_WRITE,LC0(PUT_STRING),LCS,' ',' ',' ',' ','P', //    UI_WRITE(STRING,"    Parallel programming\r\n");
    'a','r','a','l','l','e','l',' ','p','r','o',  //
    'g','r','a','m','m','i','n','g','\r','\n',0,  //
  opUI_FLUSH,                                     //    UI_FLUSH  
  opOBJECT_START,LC0(2),                          //    START_OBJECT(2)
                                                  //
  opMOVE8_8,LC0(10),LV0(4),                       //    LV4 = 10                                                  
                                                  //    do
                                                  //    {
  opUI_WRITE,LC0(PUT_STRING),LCS,' ',' ',' ',' ','O', //      UI_WRITE(STRING,"    One\r\n");
    'n','e','\r','\n',0,                          //
  opUI_FLUSH,                                     //      UI_FLUSH  
  opTIMER_WAIT,LC2(1000),LV0(0),                  //      DELAY(1)   
  opTIMER_READY,LV0(0),                           //   
                                                  //    }
  opSUB8,LV0(4),LC0(1),LV0(4),                    //    while(--LV4)
  opJR_NEQ8,LV0(4),LC0(0),LC0(-29),               //
  opOBJECT_END,                                   //  }
                                                  //
                                                  //  VMTHREAD2
                                                  //  {
  opMOVE8_8,LC0(10),LV0(4),                       //    LV4 = 10                                                  
                                                  //    do
                                                  //    {
  opUI_WRITE,LC0(PUT_STRING),LCS,' ',' ',' ',' ','T', //      UI_WRITE(STRING,"    Two\r\n");
    'w','o','\r','\n',0,                          //
  opUI_FLUSH,                                     //      UI_FLUSH  
  opTIMER_WAIT,LC2(1000),LV0(0),                  //      DELAY(1)   
  opTIMER_READY,LV0(0),                           //   
                                                  //    }
  opSUB8,LV0(4),LC0(1),LV0(4),                    //    while(--LV4)
  opJR_NEQ8,LV0(4),LC0(0),LC0(-29),               //
  opOBJECT_END,                                   //  }
                                                  //
};

/* \endverbatim
*/
