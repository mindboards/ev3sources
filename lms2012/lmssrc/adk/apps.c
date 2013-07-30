/******************************************************************************
 * MAKING APPS
 *****************************************************************************/
/*! \page makingapps Making Apps with the "lmsasm" Tool
 *
 *  <hr size="1"/>
 *
 *
 *  - \subpage helloworldlinux
 *  - \subpage helloworldwindows
 *
 *  - \subpage lmsassembler
 *
 *  - \subpage lmscodehints
 */
 
 

/******************************************************************************
 * HELLOWORLD LINUX
 *****************************************************************************/
/*! \page helloworldlinux Helloworld App (Linux PC)
 *
 *  <hr size="1"/>
 *
  \verbatim

On a Linux PC open the "Terminal"

Create a new folder in your "home" directory and go there:
mkdir ~/myapps
cd ~/myapps

Create a file called "helloworld.lms":
gedit helloworld.lms

Key in the following source code:

vmthread  MAIN
{
  UI_DRAW(FILLWINDOW,0x00,0,0)                  //  Clear screen
  UI_DRAW(TEXT,FG_COLOR,10,50,'hello world')    //  Write "hello world" at 10,50 (X,Y)
  UI_DRAW(UPDATE)                               //  Show the stuff
  UI_BUTTON(WAIT_FOR_PRESS)                     //  Wait for button press
}

Save and quit

Navigate to the "lmsasm" folder and

Compile the file with following command:
java -jar assembler.jar ~/myapps/helloworld

A file called "helloworld.rbf" should appear in your "~/myapps" folder.
Move your "myapps" folder to the root directory on a SD card.
Insert the SD card into the EV3 slot.
Navigate to the "SD_Card/myapps" folder on EV3 and start "helloworld"

  \endverbatim
 *
 */


/******************************************************************************
 * HELLOWORLD WINDOWS
 *****************************************************************************/
/*! \page helloworldwindows Helloworld App (Windows PC)
 *
 *  <hr size="1"/>
 *
  \verbatim

On a Windows PC open the "Command prompt"

Create a new folder in your "root" directory and go there:
cd \
mkdir \myapps
cd \myapps

Create a file called "helloworld.lms":
notepad helloworld.lms

Key in the following source code:

vmthread  MAIN
{
  UI_DRAW(FILLWINDOW,0x00,0,0)                  //  Clear screen
  UI_DRAW(TEXT,FG_COLOR,10,50,'hello world')    //  Write "hello world" at 10,50 (X,Y)
  UI_DRAW(UPDATE)                               //  Show the stuff
  UI_BUTTON(WAIT_FOR_PRESS)                     //  Wait for button press
}

Save and quit

Navigate to the "lmsasm" folder and

Compile the file with following command (may need adjustment for where "java.exe" is located):
"C:\Program Files (x86)\Java\jre7\bin\java.exe" -jar assembler.jar "C:\myapps\helloworld"

A file called "helloworld.rbf" should appear in your "myapps" folder.
Move your "myapps" folder to the root directory on a SD card.
Insert the SD card into the EV3 slot.
Navigate to the "SD_Card/myapps" folder on EV3 and start "helloworld"

  \endverbatim
 *
 */


/******************************************************************************
 * LMSASM TOOL
 *****************************************************************************/
/*! \page lmsassembler lmsasm tool
 *
 *  <hr size="1"/>
 *
 *  - \subpage description
 *  - \subpage programstructure
 *  - \subpage constants
 *  - \subpage variabletypes
 *
 *  - \subpage bytecodes
 *
 *  - \subpage lmscodetemplates
 *
  \verbatim


  \endverbatim
 *
 */


/******************************************************************************
 * DESCRIPTION
 *****************************************************************************/
/*! \page description Description
 *
 *  <hr size="1"/>
 *
 *
  \verbatim

The "lmsasm" is an assembler for EV3 byte code programs that takes over the housekeeping of

1. Inserting the program and object headers
2. Translating the textual instructions to byte code values
3. Allocation of variables
4. Solving label jump addresses
5. Encoding of parameters in the byte code stream
6. Calculating sub call parameters
7. Generate an executable byte code file

  \endverbatim
 *
 */


/******************************************************************************
 * PROGRAMSTRUCTURE
 *****************************************************************************/
/*! \page programstructure Program Structure
                                                  <hr size="1"/>
<b>Program example with commonly used items</b>
                                                  \verbatim

                                                  // Constants are declared in between objects (vmthread and subcall)
define    MY_GLOBAL_CONSTANT            100       // MY_GLOBAL_CONSTANT equals 100

                                                  // Global variables are declared in between objects
DATAF     MyGlobalFloat                           // MyGlobalFloat is of the type float


vmthread  MAIN                                    // All programs must start with the "main" thread object (more vmthread's are allowed)
{
                                                  // Local variables are declared inside objects and are only valid here
  DATA8   MyLocalByte                             // MyLocalByte is of the type signed byte

Loop:                                             // Labels are local and only recognised inside the object (Symbolic names can be reused in other objects)

  CALL(MySubcall,MyLocalByte)                     // Call MySubcall with one parameter

  MOVE8_F(MyLocalByte,MyGlobalFloat)              // Assign the return parameter value from MySubcall to MyGlobalFloat

  JR(Loop)                                        // Jump unconditional to label "Loop"
}


subcall   MySubcall                               // Sub calls are all global objects
{
  IO_8    MyParameter                             // Declare sub call parameters

  DATA8   MyLocalVariable                         // Declare sub call local variables

  MOVE8_8(MY_GLOBAL_CONSTANT,MyLocalVariable)     // Initialise MyLocalVariable

  MOVE8_8(MyLocalVariable,MyParameter)            // Return the value of MyLocalVariable in parameter MyParameter
}


  \endverbatim
 *
 */


/******************************************************************************
 * CONSTANTS
 *****************************************************************************/
/*! \page constants Constants
                                                  <hr size="1"/>
<b>Constant declarations</b>
                                                  \verbatim

  define Name Value

  \endverbatim
 *
 */


/******************************************************************************
 * VARIABLE TYPES
 *****************************************************************************/
/*! \page variabletypes Variable Types
                                                  <hr size="1"/>
<b>Variable declarations</b>
                                                  \verbatim
                                                  \endverbatim
<SMALL><b>Global and local variable types:</b></SMALL>
                                                  \verbatim
    DATAF   Name                                  // float            32 bit
    DATA32  Name                                  // signed long      32 bit
    DATA16  Name                                  // signed int       16 bit
    DATA8   Name                                  // signed char       8 bit
    DATAS   Name Size                             // signed char array
                                                  \endverbatim
<SMALL><b>Fixed array types:</b></SMALL>
                                                  \verbatim
    ARRAYF  Name Size                             // float array
    ARRAY32 Name Size                             // signed long array
    ARRAY16 Name Size                             // signed int array
    ARRAY8  Name Size                             // signed char array (same as DATAS)
                                                  \endverbatim
<SMALL><b>Sub call parameter types:</b></SMALL>
                                                  \verbatim
    IN_F    Name                                  // float        input parameter
    IN_32   Name                                  // signed long  input parameter
    IN_16   Name                                  // signed int   input parameter
    IN_8    Name                                  // signed char  input parameter
    IN_S    Name Size                             // signed char array input parameter

    OUT_F   Name                                  // float        input parameter
    OUT_32  Name                                  // signed long  input parameter
    OUT_16  Name                                  // signed int   input parameter
    OUT_8   Name                                  // signed char  input parameter
    OUT_S   Name Size                             // signed char array input parameter

    IO_F    Name                                  // float        input/output parameter
    IO_32   Name                                  // signed long  input/output parameter
    IO_16   Name                                  // signed int   input/output parameter
    IO_8    Name                                  // signed char  input/output parameter
    IO_S    Name Size                             // signed char array input/output parameter
                                                  \endverbatim <hr size="1"/>
<b>Assigning values to variables</b>
                                                  \verbatim

    DATAF     MyDataF                             // Declare      DATAF MyDataF

    MOVEF_F(3.14F,MyDataF)                        // Assign       MyDataF  =  3.14


    DATA32    MyData32                            // Declare      DATA32 MyData32

    MOVE32_32(1000000,MyData32)                   // Assign       MyData32  =  1000000


    DATA16    MyData16                            // Declare      DATA16 MyData16

    MOVE16_16(10000,MyData16)                     // Assign       MyData16  =  10000


    DATA8     MyData8                             // Declare      DATA8 MyData8

    MOVE8_8(100,MyData8)                          // Assign       MyData8  =  100


    DATAS     MyDataS 16                          // Declare      DATAS MyDataS[16]

    STRINGS(DUPLICATE,'My String\r\n',MyDataS)    // Assign       strcpy(MyDataS,"My String\r\n")



      special characters used in strings:

        '\r' = CR   (0x0D)
        '\n' = LF   (0x0A)
        '\t' = TAB  (0x09)
        '\q' = '    (0x27)
                                                  \endverbatim
 *
 */


/******************************************************************************
 * LMS CODE TEMPLATES
 *****************************************************************************/
/*! \page lmscodetemplates Lms Code Templates
 *
 *  <hr size="1"/>
 *
 *  - \subpage templateprogram
 *  - \subpage templateinitdisp
 *  - \subpage templatedoonbutton
 *  - \subpage templatesubcall
 *
 *  - \subpage templateapp
 */


/******************************************************************************
 * TEMPLATE PROGRAM
 *****************************************************************************/
/*! \page templateprogram Program Template
                                                  <hr size="1"/>
                                                  \verbatim
//**********************************************************************
//        PROGRAM                                                     //
//**********************************************************************
                                                                      //
define    MyConstant                    100                           //
                                                                      //
vmthread  MAIN                                                        //  void    MAIN(void)
{                                                                     //  {
  DATA8   Run                                                         //
  DATA8   Tmp                                                         //
                                                                      //
  MOVE8_8(1,Run)                                                      //    Run     =  1
                                                                      //    do
Loop:                                                                 //    {
                                                                      //
  MOVE8_8(0,Tmp)                                                      //      Tmp   =  0
  CALL(MySubcall,Tmp)                                                 //      MySubcall(Tmp)
                                                                      //
                                                                      //    }
  JR_TRUE(Run,Loop)                                                   //    while (Run)
  UI_BUTTON(FLUSH)                                                    //    UI_BUTTON(FLUSH)
}                                                                     //  }
                                                                      //
                                                                      // \endverbatim
 *
 */


/******************************************************************************
 * TEMPLATE DO ON BUTTON
 *****************************************************************************/
/*! \page templatedoonbutton Do On Button Template
                                                  <hr size="1"/>
                                                  \verbatim
                                                                      //
  UI_BUTTON(SHORTPRESS,ENTER_BUTTON,Tmp)                              //      UI_BUTTON(SHORTPRESS,ENTER_BUTTON,Tmp)
  JR_FALSE(Tmp,NoPress)                                               //      if (Tmp)
                                                                      //      {
  MOVE8_8(0,Run)                                                      //        Run  =  0
                                                                      //
NoPress:                                                              //      }
                                                                      // \endverbatim

 */


/******************************************************************************
 * TEMPLATE INIT DISPLAY
 *****************************************************************************/
/*! \page templateinitdisp Initialise Display Template
                                                  <hr size="1"/>
                                                  \verbatim
                                                                      //
  UI_DRAW(FILLWINDOW,0x00,0,0)                                        //    UI_DRAW(FILLWINDOW,0x00,0,0)
  UI_DRAW(SELECT_FONT,SMALL_FONT)                                     //    UI_DRAW(SELECT_FONT,SMALL_FONT)
  UI_DRAW(TEXT,FG_COLOR,0,1,'My App')                                 //    UI_DRAW(TEXT,FG_COLOR,0,1,'My App')
  UI_DRAW(LINE,FG_COLOR,0,TOPLINE_HEIGHT,LCD_WIDTH,TOPLINE_HEIGHT)    //    UI_DRAW(LINE,FG_COLOR,0,TOPLINE_HEIGHT,LCD_WIDTH,TOPLINE_HEIGHT)
  UI_DRAW(SELECT_FONT,NORMAL_FONT)                                    //    UI_DRAW(SELECT_FONT,NORMAL_FONT)
  UI_DRAW(UPDATE)                                                     //    UI_DRAW(UPDATE)
                                                                      // \endverbatim

 */


/******************************************************************************
 * TEMPLATE SUBCALL
 *****************************************************************************/
/*! \page templatesubcall Subcall Template
                                                  <hr size="1"/>
                                                  \verbatim
//**********************************************************************
//        SUBCALL                                                     //
//**********************************************************************
                                                                      //
                                                                      //
subcall   MySubcall                                                   //  void MySubcall(DATA8 Par)
{                                                                     //  {
  IO_8    Par                                                         //
                                                                      //
  DATA8   Tmp                                                         //
                                                                      //
  MOVE8_8(Par,Tmp)                                                    //    Tmp  =  Par
                                                                      //
                                                                      //
}                                                                     //  }
                                                                      //
                                                                      // \endverbatim
 */


/******************************************************************************
 * TEMPLATE APP
 *****************************************************************************/
/*! \page templateapp App Template
                                                  <hr size="1"/>
                                                  \verbatim
//**********************************************************************
//        PROGRAM                                                     //
//**********************************************************************
                                                                      //
define    MyConstant                    100                           //
                                                                      //
vmthread  MAIN                                                        //  void    MAIN(void)
{                                                                     //  {
  DATA8   Run                                                         //
  DATA8   Tmp                                                         //
                                                                      //
                                                                      //
  UI_DRAW(FILLWINDOW,0x00,0,0)                                        //    UI_DRAW(FILLWINDOW,0x00,0,0)
  UI_DRAW(SELECT_FONT,SMALL_FONT)                                     //    UI_DRAW(SELECT_FONT,SMALL_FONT)
  UI_DRAW(TEXT,FG_COLOR,0,1,'My App')                                 //    UI_DRAW(TEXT,FG_COLOR,0,1,'My App')
  UI_DRAW(LINE,FG_COLOR,0,TOPLINE_HEIGHT,LCD_WIDTH,TOPLINE_HEIGHT)    //    UI_DRAW(LINE,FG_COLOR,0,TOPLINE_HEIGHT,LCD_WIDTH,TOPLINE_HEIGHT)
  UI_DRAW(SELECT_FONT,NORMAL_FONT)                                    //    UI_DRAW(SELECT_FONT,NORMAL_FONT)
  UI_DRAW(UPDATE)                                                     //    UI_DRAW(UPDATE)
                                                                      //
  MOVE8_8(1,Run)                                                      //    Run     =  1
                                                                      //    do
Loop:                                                                 //    {
                                                                      //
  MOVE8_8(0,Tmp)                                                      //      Tmp   =  0
  CALL(MySubcall,Tmp)                                                 //      MySubcall(Tmp)
                                                                      //
                                                                      //
  UI_BUTTON(SHORTPRESS,ENTER_BUTTON,Tmp)                              //      UI_BUTTON(SHORTPRESS,ENTER_BUTTON,Tmp)
  JR_FALSE(Tmp,NoPress)                                               //      if (Tmp)
                                                                      //      {
  MOVE8_8(0,Run)                                                      //        Run  =  0
                                                                      //
NoPress:                                                              //      }
                                                                      //
                                                                      //    }
  JR_TRUE(Run,Loop)                                                   //    while (Run)
  UI_BUTTON(FLUSH)                                                    //    UI_BUTTON(FLUSH)
}                                                                     //  }
                                                                      //
                                                                      //
//**********************************************************************
//        SUBCALL                                                     //
//**********************************************************************
                                                                      //
                                                                      //
subcall   MySubcall                                                   //  void MySubcall(DATA8 Par)
{                                                                     //  {
  IO_8    Par                                                         //
                                                                      //
  DATA8   Tmp                                                         //
                                                                      //
  MOVE8_8(Par,Tmp)                                                    //    Tmp  =  Par
                                                                      //
                                                                      //
}                                                                     //  }
                                                                      //
                                                                      // \endverbatim
 *
 */


/******************************************************************************
 * LMS CODE HINTS
 *****************************************************************************/
/*! \page lmscodehints Lms Code Hints
 *
 *  <hr size="1"/>
 *
 *  - \subpage hintperformance
 *  - \subpage hinttimer
 */


/******************************************************************************
 * HINT PERFORMANCE
 *****************************************************************************/
/*! \page hintperformance Performance
                                                  <hr size="1"/>
<b>Speed improvement hints</b>
                                                  \verbatim

  Use in line code instead of sub calls.
  Use DATA8 instead of DATAF where possible.
  Use fixed arrays instead of the dynamic array handles.
  Use the compound byte codes instead of byte code macros.
  Use as few vmThreads as possible

                                                  \endverbatim
 *
 */


/******************************************************************************
 * HINT TIMER
 *****************************************************************************/
/*! \page hinttimer Timer
\subpage cTimer
                                                  <hr size="1"/>
<b>Delay execution (500mS)</b>
                                                  \verbatim

  DATA32  MyTimer                                 //  Declare your unique timer place holder

  TIMER_WAIT(500,MyTimer)                         //  Initialise your timer
  TIMER_READY(MyTimer)                            //  Wait for the time to expire - use VM power in other vmthreads
                                                  \endverbatim <hr size="1"/>
<b>Check for timer timeout (500mS)</b>
                                                  \verbatim

  DATA32  MyTimer                                 //  Declare your unique timer place holder
  DATA32  TmpTime                                 //  Need a working variable

  TIMER_READ(MyTimer)                             //  Initialise your timer
                                                  //
Loop:                                             //
                                                  //  Do the task until timeout
                                                  //
  TIMER_READ(TmpTime)                             //  Get the actual time
  SUB32(TmpTime,MyTimer,TmpTime)                  //  Calculate elapsed time
  JR_LT32(TmpTime,500,Loop)                       //  Compare it to the timeout time (500mS) and loop until expired
                                                  \endverbatim <hr size="1"/>
<b>Check for small timer timeout (100uS)</b>
                                                  \verbatim

  DATA32  MyTimer                                 //  Declare your unique timer place holder
  DATA32  TmpTime                                 //  Need a working variable

  TIMER_READ_US(MyTimer)                          //  Initialise your timer
                                                  //
Loop:                                             //
                                                  //  Do the task until timeout
                                                  //
  TIMER_READ_US(TmpTime)                          //  Get the actual time
  SUB32(TmpTime,MyTimer,TmpTime)                  //  Calculate elapsed time
  JR_LT32(TmpTime,100,Loop)                       //  Compare it to the timeout time (100uS) and loop until expired
                                                  \endverbatim
 *
 */


