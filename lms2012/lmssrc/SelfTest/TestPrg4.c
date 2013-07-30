//! \page testprg4app TestPrg4 Application
//!
//! <hr size="1"/>
//!
//! \verbatim
// 
//
//
//**********************************************************************
//        TESTPRG4
//**********************************************************************
                                                                      //
                                                                      //
#define   Timer               GV0(0)                                  //  DATA32    Timer
#define   Counter1            GV0(4)                                  //  DATA32    Counter1
#define   Counter2            GV0(8)                                  //  DATA32    Counter2
#define   DataF               GV0(12)                                 //  DATAF     DataF
#define   Run                 GV0(16)                                 //  DATA8     Run
#define   Globals             17                                      //
                                                                      //
                                                                      //
UBYTE     prg[] =                                                     //
{                                                                     //
  PROGRAMHeader(1.01,4,Globals),                                      //  TestPrg4
  VMTHREADHeader(0,16),                                               //  MAIN
  BLOCKHeader(0,1,1),                                                 //  Block1
  BLOCKHeader(0,1,1),                                                 //  Block2
  SUBCALLHeader(0,4),                                                 //  Add
                                                                      //
                                                                      //  void    MAIN(void)
                                                                      //  {
  opMOVE32_32,LC0(0),Counter1,                                        //    Counter1  =  0
  opMOVE32_32,LC0(0),Counter2,                                        //    Counter2  =  0
                                                                      //
  opMOVE8_8,LC0(1),Run,                                               //    Run       =  1
  opOBJECT_TRIG,LC0(2),                                               //    OBJECT_TRIG(Block1)
  opOBJECT_TRIG,LC0(3),                                               //    OBJECT_TRIG(Block2)
                                                                      //
  opTIMER_WAIT,LC4(100),Timer,                                        //    TIMER_WAIT(100,Timer)
  opTIMER_READY,Timer,                                                //    TIMER_READY(Timer)
                                                                      //
  opMOVE8_8,LC0(0),Run,                                               //    Run       =  0
  opOBJECT_WAIT,LC0(2),                                               //    OBJECT_WAIT(Block1)
  opOBJECT_WAIT,LC0(3),                                               //    OBJECT_WAIT(Block2)
                                                                      //
  opJR_NEQ32,Counter1,LC0(0),LC0(3),                                  //    if (Counter1 == 0)
                                                                      //    {
  opINFO,LC0(SET_ERROR),LC0(18),                                      //      INFO(SET_ERROR,18)
                                                                      //    }
  opJR_NEQ32,Counter2,LC0(0),LC0(3),                                  //    if (Counter2 == 0)
                                                                      //    {
  opINFO,LC0(SET_ERROR),LC0(18),                                      //      INFO(SET_ERROR,18)
                                                                      //    }
  opOBJECT_END,                                                       //  }
                                                                      //
                                                                      //
                                                                      //  define  Block1  {
                                                                      //                    do              
                                                                      //                    {
  opCALL,LC0(4),LC0(1),Counter1,                                      //                      Add(&Counter1)
                                                                      //                    }
  opJR_TRUE,Run,LC0(-7),                                              //                    while (Run)
  opOBJECT_END,                                                       //                  }
                                                                      //
                                                                      //
                                                                      //  define  Block2  {
                                                                      //                    do              
                                                                      //                    {
  opCALL,LC0(4),LC0(1),Counter2,                                      //                      Add(&Counter2)
  opNOP,                                                              //                      NOP()
                                                                      //                    }
  opJR_TRUE,Run,LC0(-8),                                              //                    while (Run)
  opOBJECT_END,                                                       //                  }
                                                                      //
                                                                      //
  1,IO_32,                                                            //  void    Add(DATA32 *Cnt)
                                                                      //  {
  opADD32,LV0(0),1,LV0(0),                                            //    *Cnt++
  opRETURN,                                                           //  
  opOBJECT_END,                                                       //  }
                                                                      //
};                                                                    //
                                                                      //
//
//! \endverbatim

