/*! \page blockexample1 Scheduling Blocks
 *
 *  This program shows how BLOCKs are scheduled
 *
 *  <hr size="1"/>
 *  \verbatim */
//
//
//  (VMTHREAD1--DELAY(2)--TRIG(2)---------------------TRIG(3)----------------WAIT(3)--WAIT(2)------)
//                              \                           \                    /        /
//                               \                           (BLOCK3--DELAY(2)--)        /
//                                \                         /                           /
//                                 (BLOCK2--DELAY(2)--TRIG(3)----------DELAY(6)--------)


UBYTE     prg[] =                                                         //  p6.c
{
  PROGRAMHeader(0,3,0),                                                   //      VersionInfo,NumberOfObjects,GlobalBytes
  VMTHREADHeader(52,12),                                                  //      OffsetToInstructions,LocalBytes
  BLOCKHeader(192,1,1),                                                   //      OffsetToInstructions,OwnerObjectId,TriggerCount
  BLOCKHeader(266,1,2),                                                   //      OffsetToInstructions,OwnerObjectId,TriggerCount
                                                                          //
                                                                          //      VMTHREAD1
                                                                          //      {
  opUI_WRITE,LC0(PUT_STRING),LCS,'B','L','O','C','K','1','\r','\n',0,         //        UI_WRITE(STRING,"BLOCK1\r\n")
  opUI_FLUSH,                                                             //        UI_FLUSH
                                                                          //
  opUI_WRITE,LC0(PUT_STRING),LCS,'D','E','L','A','Y','(','2',')','\r','\n',0, //        UI_WRITE(STRING,"DELAY(2)\r\n")
  opUI_FLUSH,                                                             //        UI_FLUSH
  opTIMER_WAIT,LC2(2000),LV0(0),                                          //        DELAY(2)   
  opTIMER_READY,LV0(0),                                                   //   
                                                                          //
  opUI_WRITE,LC0(PUT_STRING),LCS,'T','R','I','G','(','2',')','\r','\n',0,     //        UI_WRITE(STRING,"TRIG(2)\r\n")
  opUI_FLUSH,                                                             //        UI_FLUSH
  opOBJECT_TRIG,LC0(2),                                                   //        TRIG(2)
                                                                          //
  opUI_WRITE,LC0(PUT_STRING),LCS,'T','R','I','G','(','3',')','\r','\n',0,     //        UI_WRITE(STRING,"TRIG(3)\r\n")   
  opUI_FLUSH,                                                             //        UI_FLUSH
  opOBJECT_TRIG,LC0(3),                                                   //        TRIG(3)
                                                                          //
  opUI_WRITE,LC0(PUT_STRING),LCS,'W','A','I','T','(','3',')','\r','\n',0,     //        UI_WRITE(STRING,"WAIT(3)\r\n")
  opUI_FLUSH,                                                             //        UI_FLUSH
  opOBJECT_WAIT,LC0(3),                                                   //        WAIT(3)
                                                                          //
  opUI_WRITE,LC0(PUT_STRING),LCS,'D','O','N','E','(','3',')','\r','\n',0,     //        UI_WRITE(STRING,"DONE(3)\r\n")
                                                                          //
  opUI_WRITE,LC0(PUT_STRING),LCS,'W','A','I','T','(','2',')','\r','\n',0,     //        UI_WRITE(STRING,"WAIT(3)\r\n")   
  opUI_FLUSH,                                                             //        UI_FLUSH
  opOBJECT_WAIT,LC0(3),                                                   //        WAIT(2)
                                                                          //
  opUI_WRITE,LC0(PUT_STRING),LCS,'D','O','N','E','(','2',')','\r','\n',0,     //        UI_WRITE(STRING,"DONE(2)\r\n")
  opUI_WRITE,LC0(PUT_STRING),LCS,'D','O','N','E','(','1',')','\r','\n',0,     //        UI_WRITE(STRING,"DONE(1)\r\n")
  opUI_FLUSH,                                                             //        UI_FLUSH
  opOBJECT_END,                                                           //      }
                                                                          //
                                                                          //      BLOCK2
                                                                          //      {
  opUI_WRITE,LC0(PUT_STRING),LCS,'B','L','O','C','K','2','\r','\n',0,         //        UI_WRITE(STRING,"BLOCK2\r\n")   
  opUI_FLUSH,                                                             //        UI_FLUSH
                                                                          //
  opUI_WRITE,LC0(PUT_STRING),LCS,'D','E','L','A','Y','(','2',')','\r','\n',0, //        UI_WRITE(STRING,"DELAY(2)\r\n")
  opUI_FLUSH,                                                             //        UI_FLUSH
  opTIMER_WAIT,LC2(2000),LV0(0),                                          //        DELAY(2)   
  opTIMER_READY,LV0(0),                                                   //   
                                                                          //
  opUI_WRITE,LC0(PUT_STRING),LCS,'T','R','I','G','(','3',')','\r','\n',0,     //        UI_WRITE(STRING,"TRIG(3)\r\n")   
  opUI_FLUSH,                                                             //        UI_FLUSH
  opOBJECT_TRIG,LC0(3),                                                   //        TRIG(3)
                                                                          //
  opUI_WRITE,LC0(PUT_STRING),LCS,'D','E','L','A','Y','(','6',')','\r','\n',0, //        UI_WRITE(STRING,"DELAY(6)\r\n")
  opUI_FLUSH,                                                             //        UI_FLUSH
  opTIMER_WAIT,LC2(6000),LV0(0),                                          //        DELAY(2)   
  opTIMER_READY,LV0(0),                                                   //   
  opOBJECT_END,                                                           //      }
                                                                          //
                                                                          //      BLOCK3
                                                                          //      {
  opUI_WRITE,LC0(PUT_STRING),LCS,'B','L','O','C','K','3','\r','\n',0,         //        UI_WRITE(STRING,"BLOCK3\r\n")
  opUI_FLUSH,                                                             //        UI_FLUSH
                                                                          //
  opUI_WRITE,LC0(PUT_STRING),LCS,'D','E','L','A','Y','(','2',')','\r','\n',0, //        UI_WRITE(STRING,"DELAY(2)\r\n")
  opUI_FLUSH,                                                             //        UI_FLUSH
  opTIMER_WAIT,LC2(2000),LV0(0),                                          //        DELAY(2)   
  opTIMER_READY,LV0(0),                                                   //   
  opOBJECT_END                                                            //      }
                                                                          //   
};

/* \endverbatim
*/
