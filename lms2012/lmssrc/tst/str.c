/*! \page stringexample1 String Parameter Passing Program
 *
 *  This program shows how to initiate string variables,
 *  how to use some string byte codes and pass string
 *  parameters to SUBCALL's
 *
 *  <hr size="1"/>
 *  \verbatim */

// ALLOCATION ******************************************************************************************
                                                            //
enum                                                        //  Object enummeration
{                                                           //
  MAIN_ = 1,                                                //
  MYSUB_,                                                   //
  OBJECTS,                                                  //
};                                                          //
                                                            //
                                                            //
#define   GLOBALS                   0                       //  Manually sum of globals beneath
                                                            //
#define   MAIN_LOCALS               11                      //  Manually sum of locals beneath
#define   String1                   LV1(0)                  //  String 1 size is 5  characters + zero termination
#define   String2                   LV1(6)                  //  String 2 is an array handle
#define   String3                   LV1(8)                  //  String 3 is an array handle
#define   Dummy                     LV1(10)                 //  Next variable
                                                            //
#define   MYSUB_LOCALS              27                      //  manually sum of locals beneath
#define   In1                       LV1(0)                  //  String In1 size is 21 characters + zero termination
#define   In2                       LV1(22)                 //  String In2 is an array handle
#define   Out                       LV1(24)                 //  String Out is an array handle
#define   Result                    LV1(26)                 //  Result for compare
                                                            //
UBYTE     prg[] =                                           //  str.c
{                                                           //
// HEADER **********************************************************************************************
                                                            //
  PROGRAMHeader(0,(OBJECTS - 1),GLOBALS),                   //  VersionInfo,NumberOfObjects,GlobalBytes
  VMTHREADHeader(0,MAIN_LOCALS),                            //  MAIN
  SUBCALLHeader(0,MYSUB_LOCALS),                            //  MYSUB
                                                            //
// MAIN ************************************************************************************************
                                                            //
                                                            //  VMTHREAD
                                                            //  {
//  opSTRINGS,LC0(DUPLICATE),LCS,'L','E','G','O',0,String1,   //    STRING(DUPLICATE,"LEGO",String1)

  opINIT_BYTES,String1,LC0(5),LC1('L'),LC1('E'),LC1('G'),LC1('O'),LC0(0),
                                                            //
  opARRAY,LC0(CREATE8),LC0(22),String2,                     //    ARRAY(CREATE8,22,String2)
  opARRAY,LC0(CREATE8),LC0(22),String3,                     //    ARRAY(CREATE8,22,String2)

  opARRAY,LC0(INIT8),String2,LC0(0),LC0(11),LC1('M'),
    LC1('i'),LC1('n'),LC1('d'),LC1('s'),LC1('t'),
    LC1('o'),LC1('r'),LC1('m'),LC1('s'),LC0(0),

//  opINIT_BYTES,HND(String2),LC0(11),LC1('M'),LC1('i'),LC1('n'),LC1('d'),LC1('s'),LC1('t'),
//  LC1('o'),LC1('r'),LC1('m'),LC1('s'),LC0(0),
                                                            //
//  opSTRINGS,LC0(DUPLICATE),LCS,'M','i','n','d','s','t','o', //    STRING(DUPLICATE,"Mindstorms",String2)
//    'r','m','s',0,HND(String2),                             //
  opCALL,LC0(MYSUB_),LC0(3),String1,String2,String3,        //    MYSUB(3,String1,String2,String3)
                                                            //
  opUI_DRAW,LC0(FILLWINDOW),LC0(0),LC0(0),LC0(0),           //    opUI_DRAW,LC0(FILLWINDOW),LC0(0),LC0(0),LC0(0),
  opUI_DRAW,LC0(TEXT),LC0(1),LC0(24),LC1(50),HND(String3),  //    UI_DRAW(TEXT,1,24,50,String3)
  opUI_DRAW,LC0(UPDATE),                                    //    UI_DRAW(UPDATE)
  opOBJECT_END,                                             //  }
                                                            //
// MYSUB SUBCALL ***************************************************************************************
                                                            //
  0x03,IN_S,(22),IN_16,IO_16,                               //  MYSUB(In1,In2,Out)
                                                            //  {
  opSTRINGS,LC0(COMPARE),In1,LCS,'L','E','G','O',0,Result,  //    STRING(COMPARE,In1,"LEGO",Result)
  opJR_FALSE,Result,LC0(12),                                //    if (Result != 0)
                                                            //    {
  opSTRINGS,LC0(ADD),In1,LCS,' ',0,In1,                     //      STRING(ADD,In1," ",In1)
  opSTRINGS,LC0(ADD),In1,HND(In2),HND(Out),                 //      STRING(ADD,In1,In2,Out)
                                                            //    }
  opRETURN,                                                 //  }
  opOBJECT_END,                                             //
                                                            //
// END *************************************************************************************************
};

/* \endverbatim
*/
