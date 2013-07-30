/*
 * LEGO® MINDSTORMS EV3
 *
 * Copyright (C) 2010-2013 The LEGO Group
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#ifndef C_COM_H_
#define C_COM_H_

#include  <sys/types.h>
#include  <dirent.h>


#include  "lms2012.h"

/*! \page communication Communication

  This paragraph will document the communication protocol that is supposed to be used across the
  different communication platforms.

  General Protocol Overview\verbatim
  ,------,------,------,------,------,------,------,------,
  |Byte 0|Byte 1|Byte 2|Byte 3|      |      |      |Byte n|
  '------'------'------'------'------'------'------'------'

  Byte 0 – 1: Command size, Little Endian\n

  Byte 2 – 3: Message counter, Little Endian\n

  Byte 4:     Command type. The 7 lowest bit of this byte is used for identifying the command type.
              Bit 7 (MSB) is used for identifying whether the command should give a reply message or not.

  Byte 5 - n: Dependent on command type

  \endverbatim

  There are two different command types:

  System commands are thought of as commands used to controlling/utilizing some of the more overall
  system functionalities (File download,...)

  - \subpage systemcommands

  Direct commands are related to functionality which are controlled within by the virtual machine.

  - \subpage directcommands
\n
\n
\n
\n
\n
\n
  - \subpage testcases
 */


/*! \page systemcommands System Commands

  System command are defined as: a command that's not implemented as a byte code (no VM intervention).

  \verbatim
  System Command Bytes:
  ,------,------,------,------,------,------,------,------,
  |Byte 0|Byte 1|Byte 2|Byte 3|Byte 4|Byte 5|      |Byte n|
  '------'------'------'------'------'------'------'------'

  Byte 0 – 1: Command size, Little Endian\n

  Byte 2 – 3: Message counter, Little Endian\n

  Byte 4:     Command type. see following defines   */

  #define     SYSTEM_COMMAND_REPLY          0x01    //  System command, reply required
  #define     SYSTEM_COMMAND_NO_REPLY       0x81    //  System command, reply not required
                                                    /*
  Byte 5:     System Command. see following defines */

  #define     BEGIN_DOWNLOAD                0x92    //  Begin file down load
  #define     CONTINUE_DOWNLOAD             0x93    //  Continue file down load
  #define     BEGIN_UPLOAD                  0x94    //  Begin file upload
  #define     CONTINUE_UPLOAD               0x95    //  Continue file upload
  #define     BEGIN_GETFILE                 0x96    //  Begin get bytes from a file (while writing to the file)
  #define     CONTINUE_GETFILE              0x97    //  Continue get byte from a file (while writing to the file)
  #define     CLOSE_FILEHANDLE              0x98    //  Close file handle
  #define     LIST_FILES                    0x99    //  List files
  #define     CONTINUE_LIST_FILES           0x9A    //  Continue list files
  #define     CREATE_DIR                    0x9B    //  Create directory
  #define     DELETE_FILE                   0x9C    //  Delete
  #define     LIST_OPEN_HANDLES             0x9D    //  List handles
  #define     WRITEMAILBOX                  0x9E    //  Write to mailbox
  #define     BLUETOOTHPIN                  0x9F    //  Transfer trusted pin code to brick
  #define     ENTERFWUPDATE                 0xA0    //  Restart the brick in Firmware update mode
  #define     SETBUNDLEID                   0xA1    //  Set Bundle ID for mode 2
  #define     SETBUNDLESEEDID               0xA2    //  Set bundle seed ID for mode 2

/*

  Byte 6 - n: Dependent on System Command



  System Command Response Bytes:
  ,------,------,------,------,------,------,------,------,
  |Byte 0|Byte 1|Byte 2|Byte 3|      |      |      |Byte n|
  '------'------'------'------'------'------'------'------'

  Byte 0 – 1: Reply size, Little Endian\n

  Byte 2 – 3: Message counter, Little Endian\n

  Byte 4:     Reply type. see following defines     */

  #define     SYSTEM_REPLY                  0x03    //  System command reply
  #define     SYSTEM_REPLY_ERROR            0x05    //  System command reply error
/*
  Byte 5:     System command this is the response to

  Byte 6:     Reply status
*/

  // SYSTEM command return codes
  #define     SUCCESS                       0x00
  #define     UNKNOWN_HANDLE                0x01
  #define     HANDLE_NOT_READY              0x02
  #define     CORRUPT_FILE                  0x03
  #define     NO_HANDLES_AVAILABLE          0x04
  #define     NO_PERMISSION                 0x05
  #define     ILLEGAL_PATH                  0x06
  #define     FILE_EXITS                    0x07
  #define     END_OF_FILE                   0x08
  #define     SIZE_ERROR                    0x09
  #define     UNKNOWN_ERROR                 0x0A
  #define     ILLEGAL_FILENAME              0x0B
  #define     ILLEGAL_CONNECTION            0x0C

/*
  Byte 7 - n: Response dependent on System Command


  The example below is build around the host application (X3 software) that wants to send a file to a P-
  Brick where the VM, Green layer and Black layer reference to the firmware architecture. The
  architecture document below on the host side only consists of our current understanding of the
  implementation. At this point we have chosen to reference to the firmware architecture instead of the 7
  layer OSI model.


  ,---------------------,                                                 ,---------------------,
  |   Host Application  |                                                 |         VM          |
  '----------,----------'                                    ,--------->  '---------------------'  -----------,
             v                                               |                                                |
   Filename, Destination                             New file coming i                                        v
             |                                               ^                                        Accept or decline
             v                                               |                                                |
  ,---------------------,                                    '----------  ,---------------------,  <----------'
  |   Brick Server      |                                                 |   Green Layer       |
  '----------,----------'                                    ,--------->  '---------------------'  -----------,
             v                                               |                                                |
   Begin File D/L,                   Command size, Command type, Begin D/L, File size, Filename               v
   File size, Filename                                       ^                                      Accept or decline, Handle
             v                                               |                                                |
  ,---------------------,                                    '----------  ,---------------------,  <----------'
  |   USB Driver        |                                                 |   Black Layer       |
  '----------,----------'                                                 '---------------------'
             v                                                                       ^
  Command size, Command type, Begin D/L, File size, Filename    ------>
                                                                <------   Command size, Command type, Handle
  Command size, Command type, Continue D/L, Handle, Pay load    ------>
                                                                <------   Command size, Command type
  Command size, Command type, Continue D/L, Handle, Pay load    ------>
                                                                <------   Command size, Command type
  Command size, Command type, Continue D/L, Handle, Pay load    ------>


  Our current thoughts are as follows:
    1.  The host application initiates the communication by sending a request to the BrickServer that
        the application wants to send a file. The parameters within this call will include: File name for
        the file that should be send, and some description for the destination.
    2.  The brick server processes the request by sending a request to the USB driver which includes the
        following parameter: Specific command (Begin down load), File size and file name.
    3.  The USB driver will send out a command including: Command size, Command byte (Begin
        down load), File size and file name.
    4.  The P-bricks black layer with receive and validate the USB packages. It will transfer the
        received packages up to the green layer.
    5.  The green layer will evaluate the packages and as it is a begin down load ask the VM if it is
        okay to receive a packages now.
    6.  When the VM has replied OK to the green layer the green layer will ask the USB driver to
        reply OK and include a handle number to use within next packages for this communication.
    7.  When the OK signal is received with the USB driver (On the host side) the host will start
        transmitting additional data which include the follow parameters: Command size, Command
        type, Command byte (Continue D/L), Handle, Pay load data.

        Command size: 2 bytes (Tells the amount of bytes which belongs to this specific command).
        Command type: 1 byte
        File size: 4 Bytes
        Handle: 1 Byte


  Additional consideration
  Above scenario allows for having multiple file action running at the same time as we have a handle
  which identifies which process new data relates to. It also allows for interleaved communication
  during a potential long down load process. But our current knowledge and considerations are also that
  interleaved commands and splitting up larger data chunks slows down Bluetooth communication a lot.
  Bluetooth communication requires large data chunks to be pushed through the channel all the time
  with as less packages division as possible. Therefore the command size parameter is also included to
  enable sending larger data packages as one call.
  Above protocol architecture enables implementing the following two different scenarios which
  performs the same functionality (Sending a 60 Kbyte file to the brick):

  Scenario 1: (It is not possible to interleave during file down load) (60 K bytes data package):
  1.  Command size (0x06,0x00), Command type, Begin D/L, File size (0x60,0xEA,0x00,0x00), File Name (0x30)
  2.  Command size (0x62,0xEA), Command type, Continue D/L, Handle (0x01), Data ............

  Scenario 2 (This enables the host to interleave during file down load) (500 bytes data packages):
  1.  Command size (0x06,0x00),Command Type, Begin D/L, File size (0x60,0xEA,0x00,0x00), File Name (0x30)
  2.  Command size (0xF6,0x01), Command type, Continue D/L, Handle (0x01), Data ............
  3.  ..
  4.  ..
  5.  Command size (0xF6,0x01), Command type ,Continue D/L, Handle (0x01), Data ............



  - File Down Load
    - Destination filename path is relative from "lms2012/sys"
    - Destination folders are automatically created from filename path
    - First folder name must be: "apps", "prjs" or "tools" (see \ref UIdesign)
    - Second folder name in filename path must be equal to byte code executable name

  - File Upload (File read)
    - BEGIN_UPLOAD and CONTINUE_UPLOAD closes automatically the file handle when file has been uploaded
    - BEGIN_GETFILE and CONTINUE_GETFILE does not close the file handle when EOF has been reached
    - CONTINUE_GETFILE does also return the complete file size

  - Directory upload
    - LIST_FILES should work as long as list does not exceed 1014 bytes. CONTINUE_LISTFILES has NOT
      been implemented yet.

  - File handles
    - CLOSE_FILEHANDLE is partly implemented - hash is not working


    Examples:
*********************************************************************************************************

  File down load:
  ---------------

    Download file "../apps/tst/tst.rbf"

      BEGIN_DOWNLOAD:

          Bytes sent to brick:

            1C00xxxx0192xxxxxxxx2E2E2F617070732F7473742F7473742E72626600    (Hex)
            bbbbmmmmttssllllllllnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn

            bbbb = bytes in message, mm = message counter, tt = type of command, ss = system command,
            llllllll = file length, nn.. = filename


          Bytes received from brick:

            0600xxxx03920000    (Hex)
            bbbbmmmmttssrrhh

            bbbb = bytes in message, mm = message counter, tt = type of command, ss = system command,
            rr = return status, hh = handle to file


      CONTINUE_DOWNLOAD:

          Bytes sent to brick:

            xxxxxxxx819300xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx    (Hex)
            bbbbmmmmttsshhpppppppppppppppppppppppppppppppppppppppp

            bbbb = bytes in message, mm = message counter, tt = type of command, ss = system command, hh = handle to file (from BEGIN_DOWNLOAD), pp.. = pay load


          Bytes received from brick:

            0600xxxx03930000    (Hex)
            bbbbmmmmttssrrhh

            bbbb = bytes in message, mm = message counter, tt = type of command, ss = system command,
            rr = return status, hh = handle to file


  File Upload:
  ------------

    BEGIN_UPLOAD:

      Bytes send to the brick:

       xxxxxxxx0194xxxxxxx
       bbbbmmmmttssllllnnn...

       bbbb = bytes in message, mmmm = message counter, tt = type of command, ss = system command,
       llll = bytes to read, nnn... = filename incl. path


     Bytes received form the brick:

       xxxxxxxx039400xxxxxxxx00xxx
       bbbbmmmmttssrrllllllllhhppp...

       bbbb = bytes in massage, mmmm = message counter, tt = type of command, ss = system command,
       rr = return status, llllllll = file size, hh = file handle, ppp... = payload


    CONTINUE_UPLOAD:

      Bytes send to the brick:

      0700xxxx019500xxxx
      bbbbmmmmttsshhllll

      bbbb = bytes in the message, mmmm = message counter, tt = type of command, ss = system command,
      hh = file handle, llll = bytes to read


      Bytes send to the PC:

      xxxxxxxx03950000xxx
      bbbbmmmmttssrrhhppp...

      bbbb = bytes in the message, mmmm = message counter, tt = type of command, ss = system command,
      rr = return status,  hh = handle, pppp.. = payload



  Getting file content
  --------------------

    Used to upload datalog files - file handle is only closed when reaching EOF and file is not
    open for writing

    BEGIN_GETFILE:

      Bytes send to the brick:

      xxxxxxxx0196xxxxxxx
      bbbbmmmmttssllllnnn...

      bbbb = Bytes in massage, mmmm = message counter, tt = type of command, ss = system command,
      llll = max bytes to read, nnnn.... = path


      Bytes send to the PC:

      xxxxxxxx039600xxxxxxxx00xxx
      bbbbmmmmttssrrllllllllhhppp...

      bbbb = bytes ion massage, mmmm = message counter, tt = type of command, ss = system command,
      rr = return status, llllllll = File size, hh = Handle, ppp... = payload


    CONTINUE_GETFILE:

      Bytes send to the brick:

      0700xxxx019700xxxx
      bbbbmmmmttsshhllll

      bbbb = bytes in massage, mmmm = message counter, tt = type of command, ss = system command,
      hh = handle, llll = max bytes to read


      Bytes send to the PC:

      xxxxxxxx039700xxxxxxxx00xxx
      bbbbmmmmttssrrllllllllhhppp...

      bbbb = bytes in massage, mmmm = message counter, tt = type of command, ss = system command,
      rr = return status, llllllll = File size, hh = Handle, ppp... = payload



  Listing files and folders:
  --------------------------

    LIST_FILES:

      The new line delimited list is formatted as:

        If it is a file:
        32 chars (hex) of MD5SUM + space + 8 chars (hex) of filesize + space + filename + new line

        If it is a folder:
        foldername + / + new line


      Bytes send to the brick:

      xxxxxxxx0199xxxxxxx
      bbbbmmmmttssllllnnn...

      bbbb = bytes in message, mmmm = message counter, tt = type of message, ss = system command,
      llll = Max bytes to read, nnn.. = path name


      Bytes send to the PC:

      xxxxxxxx0399xxxxxxxxxxxxxxx
      bbbbmmmmttssrrllllllllhhnnn...

      bbbb = bytes in message, mmmm = message counter, tt = type of message, ss = system command,
      rr = return status, llllllll = List size, hh = Handle, nnn.. = new line delimited lists


    CONTINUE_LIST_FILES:

      Bytes send to the brick:

      0700xxxx019Axxxxxx
      bbbbmmmmttsshhllll

      bbbb = bytes in massage, mmmm = message counter, tt = type of command, ss = system command,
      hh = handle, llll = max bytes to read


      Bytes send to the PC:

      xxxxxxxx039Axxxxxxx
      bbbbmmmmttssrrhhppp...

      bbbb = bytes in massage, mmmm = message counter, tt = type of command, ss = system command,
      rr = return status, hh = Handle, ppp... = payload


  CLOSE_FILEHANDLE:
  -----------------

    Bytes send to the brick:

    xxxxxxxx019800xxxxxxxxxxxxxxxx
    bbbbmmmmttsshhpppppppppppppppp

    bbbb = bytes in the message, mmmm = message counter, tt = type of message, ss = system command,
    hh = handle, ppp... = hash


    Bytes send to the PC:

    0500xxxx039800
    bbbbmmmmttssrr

    bbbb = bytes in massage, mmmm = message counter, tt = type of message, ss = system command,
    rr = return status



  CREATE_DIR:
  -----------

    Bytes to send to the brick:

    xxxxxxxx019Bxxxxxx...
    bbbbmmmmttsspppppp...

    bbbb = bytes in massage, mmmm = message counter, tt = type of message, ss = system command,
    pp = null terminated full path of directory to create


    Bytes send to the PC:

    0500xxxx039Bxx
    bbbbmmmmttssrr

    bbbb = bytes in massage, mmmm = message counter, tt = type of message, ss = system command,
    rr = return status



  DELETE_FILE:
  -------------

    Bytes to send to the brick:

    xxxxxxxx019Cxxxxxx...
    bbbbmmmmttsspppppp...

    bbbb = bytes in massage, mmmm = message counter, tt = type of message, ss = system command,
    pp = null terminated full path of the file to delete


    Bytes send to the PC:

    0500xxxx039Cxx
    bbbbmmmmttssrr

    bbbb = bytes in massage, mmmm = message counter, tt = type of message, ss = system command,
    rr = return status



  LIST_OPEN_HANDLES:
  ----------_-------

    Bytes to send to the brick:

    xxxxxxxx019D
    bbbbmmmmttss

    bbbb = bytes in massage, mmmm = message counter, tt = type of message, ss = system command


    Bytes send to the PC:

    xxxxxxxx039Dxxxxxx....
    bbbbmmmmttssrrpppp....

    bbbb = bytes in massage, mmmm = message counter, tt = type of message, ss = system command,
    rr = return status, pppp = bits indicating whether handles are busy or not.



  WRITEMAILBOX:
  -------------

    Bytes sent to another brick:

    Mailbox name has to be zero terminated but name length has to be number of chars excluding
    the zero termination.

    xxxxxxxx819Exxxxxxxxxxxxxxxxxxxx
    bbbbmmmmttssllaaaaa...LLLLppp...

    bbbb = bytes in the message, mmmm = message counter, tt = type of message, ss = system command,
    ll = Name Length, aaa... = Name, LLLL = Payload length, ppp... = Payload


    Reply received from another brick:

    - Not valid


  BLUETOOTHPIN:
  --------------

    This command can only be sent by USB for safety reasons
    Bluetooth address does not contain colons
    Bluetooth MAC address is a zero terminated string type
    Bluetooth pin code is a zero terminated string type

    Bytes sent to the brick:

    0E00xxxx019F06xxxxxxxxxxxx04xxxx
    bbbbmmmmttssllaaaaaaaaaaaaLLpppp

    bbbb = bytes in the message, mmmm = message counter, tt = type of message, ss = system command,
    ll = MAC Length, aaa.. = MAC address of PC, LL = Pin length, ppp... = Pin code


    Bytes send to the PC:

    0F00xxxx039Fxx06xxxxxxxxxxxx04xxxx
    bbbbmmmmttssrrllaaaaaaaaaaaaLLpppp

    bbbb = bytes in massage, mmmm = message counter, tt = type of message, ss = system command,
    rr = return status, ll = MAC length, MAC address, Pin length, Pin


  ENTERFWUPDATE:
  --------------

    This command is used to force the brick into firmware update mode. The command will not
    send any response back to the host. The filesystem will not be updated when closing
    Linux.

    Bytes send to the brick:

    0400xxxx81A0
    bbbbmmmmttss

    bbbb = bytes in massage, mmmm = message counter, tt = type of message, ss = system command,


  SETBUNDLEID
  -------------

    Sets the default Bundle ID for mode 2. Default bundle ID is "com.lego.lms".


    xxxxxxxx01A1xxxxxx....
    bbbbmmmmttsspppppp....

    bbbb = bytes in massage, mmmm = message counter, tt = type of message, ss = system command,
    pppppp = null terminated ID string. Max. length = 24 chars including the null termination


  SETBUNDLESEEDID
  ------------------

    Sets the default Bundle seed ID for mode 2. Default bundle seed ID is "9RNK8ZF528".

    xxxxxxxx01A1xxxxxx....
    bbbbmmmmttsspppppp....

    bbbb = bytes in massage, mmmm = message counter, tt = type of message, ss = system command,
    pppppp = null terminated SEED ID string. Max. length = 11 chars including the null termination



*********************************************************************************************************
  \endverbatim
 */


/*! \page directcommands Direct Commands
 *  <hr size="1"/>

  Beside running user programs the VM is able to execute direct commands from the Communication Module.
  In fact direct commands are small programs that consists of regular byte codes and they are executed
  in parallel with a running user program.\n
  Special care MUST be taken when writing direct commands because the decision until now is NOT to
  restrict the use of "dangerous" codes and constructions (loops in a direct command are allowed).

  If a new direct command from the same source is going to be executed an actual running direct command is terminated.

  Because of a small header objects are limited to one VMTHREAD only - SUBCALLs and BLOCKs is of
  course not possible.\n
  This header contains information about number of global variables (for response), number of local variables
  and command size.

  Direct commands that has data response can place the data in the global variable space. The global
  variable space is equal to the communication response buffer. The composition of the direct command
  defines at which offset the result is placed (global variable 0 is placed at offset 0 in the buffer).

  Offset in the response buffer (global variables) must be aligned (float/32bits first and 8 bits last).
 \n
 \verbatim
  Direct Command Bytes:
  ,------,------,------,------,------,------,------,------,
  |Byte 0|Byte 1|Byte 2|Byte 3|Byte 4|Byte 5|      |Byte n|
  '------'------'------'------'------'------'------'------'

  Byte 0 – 1: Command size, Little Endian\n

  Byte 2 – 3: Message counter, Little Endian\n

  Byte 4:     Command type. see following defines   */

  #define     DIRECT_COMMAND_REPLY          0x00    //  Direct command, reply required
  #define     DIRECT_COMMAND_NO_REPLY       0x80    //  Direct command, reply not required

                                                    /*

  Byte 5 - 6: Number of global and local variables (compressed).

               Byte 6    Byte 5
              76543210  76543210
              --------  --------
              llllllgg  gggggggg

                    gg  gggggggg  Global variables [0..MAX_COMMAND_GLOBALS]

              llllll              Local variables  [0..MAX_COMMAND_LOCALS]

  Byte 7 - n: Byte codes



  Direct Command Response Bytes:
  ,------,------,------,------,------,------,------,------,
  |Byte 0|Byte 1|Byte 2|Byte 3|      |      |      |Byte n|
  '------'------'------'------'------'------'------'------'

  Byte 0 – 1: Reply size, Little Endian\n

  Byte 2 – 3: Message counter, Little Endian\n

  Byte 4:     Reply type. see following defines     */

  #define     DIRECT_REPLY                  0x02    //  Direct command reply
  #define     DIRECT_REPLY_ERROR            0x04    //  Direct command reply error

                                                    /*

  Byte 5 - n: Response buffer (global variable values)



\endverbatim \anchor directcommandexamples \n
<b> Direct Command Examples </b>
<hr size="1"/>
opOUTPUT_START Example \verbatim

  Start motor connected to port A with speed 20:

        Byte codes:             opOUTPUT_POWER,LC0(0),LC0(0x01),LC0(20), opOUTPUT_START,LC0(0),LC0(0x01)
                                         \    /
                                          \  /
        Hex values send:    0C00xxxx800000A4000114A60001

\endverbatim \n
<hr size="1"/>
opOUTPUT_STOP Example \verbatim

  Stop and float motor connected to port A:

        Byte codes:             opOUTPUT_STOP,LC0(0),LC0(0x01),LC0(0),
                                         \    /
                                          \  /
        Hex values send:    0900xxxx800000A3000100

\endverbatim \n
<hr size="1"/>
opINPUT_READ Example \verbatim

  Read sensor connected to port 1:

        Byte codes:             opINPUT_READ,LC0(0),LC0(0),LC0(0),LC0(0),GV0(0),
                                         \    /
                                          \  /
        Hex values send:    0B00xxxx0001009A0000000060


        Hex values received: 0400xxxx0200
                                      ^-
                                      |
     Command global variable (response buffer offset) 0=sensor value.

\endverbatim \n
<hr size="1"/>
opINPUT_DEVICE GET_NAME Example \verbatim

  Read sensor name connected to port 1:

        Byte codes:             opINPUT_DEVICE,LC0(0),LC0(0),LC0(GET_NAME),LC0(16),GV0(0),
                                         \    /
                                          \  /
        Hex values send:    0B00xxxx001000990000151060


        Hex values received: 1300xxxx024F70656E202020202020202020202000
                                      ^-------------------------------
                                      |
     Command global variable (response buffer offset) 0..15=sensor name "Open           ".

\endverbatim \n
<hr size="1"/>
opINPUT_DEVICE_LIST Example \verbatim

  Get all device types connected to input ports:

        Byte codes:             opINPUT_DEVICE_LIST,LC0(4),GV0(0),GV0(4),
                                        \            /
                                         \    -------
                                          \  /
        Hex values send:    0900xxxx00050098046064


        Hex value received: 0800xxxx027E7E7E7D00
                                      ^---------
                                      |
    Command global variable (response buffer offset) 0=port 1 type, 1=port 2 type, 2=port 3 type, 3=port4 type, 4=change flag.

\endverbatim \anchor opMEMORY_WRITE1 \n
<hr size="1"/>
opMEMORY_WRITE Example \verbatim

  Write 0x01 0x02 0x03 0x04 0x05 into global variable 4..8 of user program running in slot 1:

    opINIT_BYTES,LV0(0),LC0(5),1,2,3,4,5,
    opMEMORY_WRITE,LC0(1),LC0(0),LC0(4),LC0(5),LV0(0),

    Bytes actually sent to brick:

      1300xxxx8000142F400501020304057E0100040540
      bbbbmmmmtthhhhccccccccccccccccCCCCCCCCCCCC

      bbbb = bytes in message, mm = message counter, tt = type of command, hhhh = header, cc/CC = byte codes.

            hhhh = 10 least significant bits are number of globals, 6 most significal bits are locals


    Bytes received from brick:

      0600xxxx02
      bbbbmmmmtt

      bbbb = bytes in message, mm = message counter, tt = type of command, rr = global variables (response).

\endverbatim \anchor opMEMORY_READ1 \n
<hr size="1"/>
opMEMORY_READ Example \verbatim

  Read global variable 4..8 from user program running in slot 1:
    opMEMORY_READ,LC0(1),LC0(0),LC0(4),LC0(5),GV0(0),

    Bytes actually sent to brick:

      0B00xxxx0005007F0100040560
      bbbbmmmmtthhhhcccccccccccc

      bbbb = bytes in message, mm = message counter, tt = type of command, hhhh = header, cc = byte codes.

            hhhh = 10 least significant bits are number of globals, 6 most significal bits are locals


    Bytes received from brick:

      0800xxxx02xxxxxxxxxx
      bbbbmmmmttrrrrrrrrrr

      bbbb = bytes in message, mm = message counter, tt = type of command, rr = global variables (response).

\endverbatim \n
<hr size="1"/>
opINFO GET_ID Example \verbatim

  Read 6 bytes ID from brick:

    opINFO,LC0(GET_ID),LC0(6),GV0(0)

    Bytes actually sent to brick:

      0900xxxx0006007C000660
      bbbbmmmmtthhhhcccccccc

      bbbb = bytes in message, mm = message counter, tt = type of command, hhhh = header, cc = byte codes.

            hhhh = 10 least significant bits are number of globals, 6 most significal bits are locals


    Bytes received from brick:

      0900xxxx02xxxxxxxxxxxx
      bbbbmmmmttrrrrrrrrrrrr

      bbbb = bytes in message, mm = message counter, tt = type of command, rr = global variables (response).

\endverbatim \n
<hr size="1"/>
opPROGRAM_START Example \verbatim

  Run app byte code file (../apps/tst/tst.rbf) in user slot (1):


    opFILE,LC0(LOAD_IMAGE),LC0(USER_SLOT),LCS,'.','.','/','a','p','p','s','/','t','s','t','/','t','s','t','.','r','b','f',0,LV0(0),LV0(4),
    opPROGRAM_START,LC0(USER_SLOT),LV0(0),LV0(4),LC0(0),

    Bytes actually sent to brick:

      2400xxxx800020C00801802E2E2F617070732F7473742F7473742E7262660040440301404400
      bbbbmmmmtthhhhccccccccccccccccccccccccccccccccccccccccccccccccccccCCCCCCCCCC

      bbbb = bytes in message, mm = message counter, tt = type of command, hhhh = header, cc/CC = byte codes.

            hhhh = 10 least significant bits are number of globals, 6 most significal bits are locals

\endverbatim \n
<hr size="1"/>
opPROGRAM_STOP Example \verbatim

  Stop program in user slot (1):

    opPROGRAM_STOP,LC0(USER_SLOT),


    Bytes actually sent to brick:

      0700xxxx8000000201
      bbbbmmmmtthhhhcccc

      bbbb = bytes in message, mm = message counter, tt = type of command, hhhh = header, cc/CC = byte codes.

            hhhh = 10 least significant bits are number of globals, 6 most significal bits are locals

\endverbatim \n
 */


/*! \page testcases Communication Test Cases

  To be able to follow the development of the communication system and benchmark the performance some
  test cases are defined. These test cases will give reproducible results for all involved.
  \verbatim

*********************************************************************************************************
Case 1. Direct Command to Brick without hardware intervention response:

          Move 32 bit constant to global variable (response buffer)


            Byte codes:                         opMOVE32_32,LC4(1),GV0(0)
                                                         \    /      \
                                                          \  /        \
            Hex values send to brick:       0C00xxxx0004003A830100000060
                                                              ========
                                                               /
                                                      32 bit value copied to response
                                                             /
                                                      ========
            Hex values received from brick: 0700xxxx0201000000



*********************************************************************************************************
  \endverbatim
 */

/*
 * NEW MOTOR/DAISY
Direct Command Bytes:
  ,------,------,------,------,------,------,------,------,
  |Byte 0|Byte 1|Byte 2|Byte 3|Byte 4|Byte 5|      |Byte n|
  '------'------'------'------'------'------'------'------'

  Byte 0 – 1: Command size, Little Endian\n

  Byte 2 – 3: Message counter, Little Endian\n

  Byte 4:     Command type. see following defines   */

  #define     DIR_CMD_REPLY_WITH_BUSY      	0x0F    //  Direct command, reply required
  #define     DIR_CMD_NO_REPLY_WITH_BUSY    0x8F    //  Direct command, reply not required



enum
{
  USBDEV,
  USBHOST,
  BTSLAVE,
  BTMASTER1,
  BTMASTER2,
  BTMASTER3,
  BTMASTER4,
  BTMASTER5,
  BTMASTER6,
  BTMASTER7,
  WIFI,
  NO_OF_CHS
};

#define   MAX_MSG_SIZE                  1024
#define   NO_OF_MAILBOXES               30
#define   MAILBOX_CONTENT_SIZE          250
#define   USB_CMD_IN_REP_SIZE           1024
#define   USB_CMD_OUT_REP_SIZE          1024


typedef   UWORD     CMDSIZE;
typedef   UWORD     MSGCNT;


typedef   struct                        //!< Common command struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCnt;
  UBYTE   Cmd;
  UBYTE   PayLoad[];                    //!< Pay load is DIRCMD or SYSCMD
}
COMCMD;


typedef   struct                        //!< Common reply struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCnt;
  UBYTE   Cmd;
  UBYTE   PayLoad[];
}
COMRPL;


typedef   struct                        //!< Direct command struct
{
  UBYTE   Globals;
  UBYTE   Locals;
  UBYTE   Code[];
}
DIRCMD;


typedef   struct                        //!< System command begin down load command
{
  UBYTE   Sys;
  UBYTE   LengthLsb;
  UBYTE   LengthNsb1;
  UBYTE   LengthNsb2;
  UBYTE   LengthMsb;
  UBYTE   Name[];
}
SYSCMDB;


typedef   struct                        //!< System command begin Upload command
{
  UBYTE   Sys;
  UBYTE   LengthLsb;
  UBYTE   LengthMsb;
  UBYTE   Name[];
}
SYSCMDBUPL;

typedef   struct                        //!< System command Continue Upload command
{
  UBYTE   Sys;
  UBYTE   Handle;
  UBYTE   LengthLsb;
  UBYTE   LengthMsb;
}
SYSCMDCUPL;

typedef   struct                        //!< System command continue down load command
{
  UBYTE   Sys;
  UBYTE   Handle;
  UBYTE   PayLoad[];
}
SYSCMDC;






typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   BytesToReadLsb;
  UBYTE   BytesToReadMsb;
  UBYTE   Path[];
}BEGIN_LIST;
#define   SIZEOF_BEGINLIST              6

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Status;
  UBYTE   ListSizeLsb;
  UBYTE   ListSizeNsb1;
  UBYTE   ListSizeNsb2;
  UBYTE   ListSizeMsb;
  UBYTE   Handle;
  UBYTE   PayLoad[];
}RPLY_BEGIN_LIST;
#define   SIZEOF_RPLYBEGINLIST          12

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Handle;
  UBYTE   BytesToReadLsb;
  UBYTE   BytesToReadMsb;
}CONTINUE_LIST;
#define   SIZEOF_CONTINUELIST           7


typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Status;
  UBYTE   Handle;
  UBYTE   PayLoad[];
}RPLY_CONTINUE_LIST;
#define   SIZEOF_RPLYCONTINUELIST       8

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   BytesToReadLsb;
  UBYTE   BytesToReadMsb;
  UBYTE   Path[];
}BEGIN_GET_FILE;
#define   SIZEOF_BEGINGETFILE           6

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Status;
  UBYTE   FileSizeLsb;
  UBYTE   FileSizeNsb1;
  UBYTE   FileSizeNsb2;
  UBYTE   FileSizeMsb;
  UBYTE   Handle;
  UBYTE   PayLoad[];
}RPLY_BEGIN_GET_FILE;
#define   SIZEOF_RPLYBEGINGETFILE       12

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Handle;
  UBYTE   BytesToReadLsb;
  UBYTE   BytesToReadMsb;
}CONTINUE_GET_FILE;
#define   SIZEOF_CONTINUEGETFILE        7

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Status;
  UBYTE   FileSizeLsb;
  UBYTE   FileSizeNsb1;
  UBYTE   FileSizeNsb2;
  UBYTE   FileSizeMsb;
  UBYTE   Handle;
  UBYTE   PayLoad[];
}RPLY_CONTINUE_GET_FILE;
#define   SIZEOF_RPLYCONTINUEGETFILE    12

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   BytesToReadLsb;
  UBYTE   BytesToReadMsb;
  UBYTE   Path[];
}BEGIN_READ;
#define   SIZEOF_BEGINREAD              6

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Status;
  UBYTE   FileSizeLsb;
  UBYTE   FileSizeNsb1;
  UBYTE   FileSizeNsb2;
  UBYTE   FileSizeMsb;
  UBYTE   Handle;
  UBYTE   PayLoad[];
}RPLY_BEGIN_READ;
#define   SIZEOF_RPLYBEGINREAD          12

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Handle;
  UBYTE   BytesToReadLsb;
  UBYTE   BytesToReadMsb;
}CONTINUE_READ;
#define   SIZEOF_CONTINUEREAD           7

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Status;
  UBYTE   Handle;
  UBYTE   PayLoad[];
}RPLY_CONTINUE_READ;
#define   SIZEOF_RPLYCONTINUEREAD       8

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
}LIST_HANDLES;
#define   SIZEOF_LISTHANDLES            4

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Status;
  UBYTE   PayLoad[];
}RPLY_LIST_HANDLES;
#define   SIZEOF_RPLYLISTHANDLES        7

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Name[];
}REMOVE_FILE;
#define   SIZEOF_REMOVEFILE             4

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Status;
}RPLY_REMOVE_FILE;
#define   SIZEOF_RPLYREMOVEFILE         7

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Dir[];
}MAKE_DIR;
#define   SIZEOF_MAKEDIR                4

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Status;
}RPLY_MAKE_DIR;
#define   SIZEOF_RPLYMAKEDIR            7

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Handle;
}CLOSE_HANDLE;
#define   SIZEOF_CLOSEHANDLE            5

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Status;
  UBYTE   Handle;
}RPLY_CLOSE_HANDLE;
#define   SIZEOF_RPLYCLOSEHANDLE        8

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   FileSizeLsb;
  UBYTE   FileSizeNsb1;
  UBYTE   FileSizeNsb2;
  UBYTE   FileSizeMsb;
  UBYTE   Path[];
//  UBYTE   PayLoad[];
}BEGIN_DL;
#define   SIZEOF_BEGINDL                10

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Status;
  UBYTE   Handle;
}RPLY_BEGIN_DL;
#define   SIZEOF_RPLYBEGINDL            8

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Handle;
  UBYTE   PayLoad[];
}CONTINUE_DL;
#define   SIZEOF_CONTINUEDL             7

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Status;
  UBYTE   Handle;
}RPLY_CONTINUE_DL;
#define   SIZEOF_RPLYCONTINUEDL         8

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   NameSize;
  UBYTE   Name[];
}WRITE_MAILBOX;
#define   SIZEOF_WRITEMAILBOX           7

typedef   struct
{
  UBYTE   SizeLsb;
  UBYTE   SizeMsb;
  UBYTE   Payload[];
}WRITE_MAILBOX_PAYLOAD;
#define   SIZEOF_WRITETOMAILBOXPAYLOAD  2

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   MacSize;
  UBYTE   Mac[13];
  UBYTE   PinSize;
  UBYTE   Pin[7];
}BLUETOOTH_PIN;
#define   SIZEOF_BLUETOOTHPIN           7

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Status;
  UBYTE   MacSize;
  UBYTE   Mac[13];
  UBYTE   PinSize;
  UBYTE   Pin[7];
}RPLY_BLUETOOTH_PIN;
#define   SIZEOF_RPLYBLUETOOTHPIN       7

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   BundleId[];
}BUNDLE_ID;
#define   SIZEOF_BUNDLEID               6

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Status;
}RPLY_BUNDLE_ID;
#define   SIZEOF_RPLYBUNDLEID           7

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   BundleSeedId[];
}BUNDLE_SEED_ID;
#define   SIZEOF_BUNDLESEEDID           6

typedef   struct
{
  CMDSIZE CmdSize;
  MSGCNT  MsgCount;
  UBYTE   CmdType;
  UBYTE   Cmd;
  UBYTE   Status;
}RPLY_BUNDLE_SEED_ID;
#define   SIZEOF_RPLYBUNDLESEEDID       7


// Constants related to State
enum
{
  TXIDLE,
  TXFILEUPLOAD,
  TXGETFILE,
  TXLISTFILES,
  TXFOLDER,
  TXFILE,
  RXIDLE,
  RXFILEDL
};


// Constants related to sub state
enum
{
  SUBSTATE_IDLE,
  FILE_IN_PROGRESS_WAIT_FOR_REPLY,
  FILE_COMPLETE_WAIT_FOR_REPLY
};


//Constants related to ReplyState
enum
{
  DIR_CMD_REPLY     =  0x01,
  DIR_CMD_NOREPLY   =  0x02,
  SYS_CMD_REPLY     =  0x04,
  SYS_CMD_NOREPLY   =  0x08
};

RESULT    cComInit(void);

RESULT    cComOpen(void);

RESULT    cComClose(void);

RESULT    cComExit(void);

void      cComRead(void);

void      cComWrite(void);

void      cComGet(void);

void      cComSet(void);

void      cComUpdate(void);

void      cComTxUpdate(UBYTE ChNo);

void      cComRemove(void);

DATA8     cComGetUsbStatus(void);
UBYTE     cComGetBtStatus(void);
UBYTE     cComGetWifiStatus(void);

void      cComReady(void);
void      cComTest(void);

void      cComReadData(void);
void      cComWriteData(void);

void      cComWriteFile(void);

void      cComOpenMailBox(void);
void      cComWriteMailBox(void);
void      cComReadMailBox(void);
void      cComTestMailBox(void);
void      cComReadyMailBox(void);
void      cComCloseMailBox(void);

void      cComGetBrickName(DATA8 Length, DATA8 *pBrickName);
DATA8     cComGetEvent(void);

// DAISY chain
// Write type data to chain
RESULT    cComSetDeviceInfo(DATA8 Length,UBYTE *pInfo);

// Read type data from chain
RESULT    cComGetDeviceInfo(DATA8 Length,UBYTE *pInfo);

// Write mode to chain
RESULT    cComSetDeviceType(DATA8 Layer,DATA8 Port,DATA8 Type,DATA8 Mode);

// Read device data from chain
RESULT    cComGetDeviceData(DATA8 Layer,DATA8 Port,DATA8 Length,DATA8 *pType,DATA8 *pMode,DATA8 *pData);


typedef   struct
{
  char    Name[vmFILENAMESIZE];         //!< File name
  struct  dirent   **namelist;
  ULONG   Size;                         //!< File size
  int     File;

  ULONG   Length;                       //!< Total down load length
  ULONG   Pointer;                      //!<
  UWORD   State;
}FIL;


typedef   struct
{
  UBYTE     Buf[1024];
  ULONG     MsgLen;
  ULONG     BlockLen;
  ULONG     SendBytes;
  ULONG     BufSize;
  FIL       *pFile;
  UBYTE     FileHandle;
  UBYTE     RemoteFileHandle;
  UBYTE     State;
  UBYTE     SubState;
  UBYTE     Writing;
  DIR       *pDir;
  UBYTE     Folder[vmFILENAMESIZE];
}TXBUF;


typedef   struct
{
  UBYTE     Buf[1024];
  ULONG     MsgLen;
  ULONG     RxBytes;
  ULONG     BufSize;
  FIL       *pFile;
  UBYTE     FileHandle;
  UBYTE     State;
}RXBUF;


typedef   struct
{
  UBYTE   Name[50];
  UBYTE   Status;
  UBYTE   FifoSize;
  UBYTE   Type;
  UWORD   DataSize;
  UWORD   ReadCnt;
  UWORD   WriteCnt;
  ULONG   Content[(MAILBOX_CONTENT_SIZE/4) + 1];
}MAILBOX;


typedef struct
{
  //*****************************************************************************
  // Com Global variables
  //*****************************************************************************
  IMGDATA   Image[sizeof(IMGHEAD) + sizeof(OBJHEAD) + USB_CMD_IN_REP_SIZE - sizeof(DIRCMD)]; // must be aligned
  DATA32    Alignment;
  UBYTE     Globals[MAX_COMMAND_GLOBALS];
  UBYTE     CommandReady;

  UWORD     (*ReadChannel[NO_OF_CHS])(UBYTE *, UWORD);
  UWORD     (*WriteChannel[NO_OF_CHS])(UBYTE *, UWORD);

  FIL       Files[MAX_FILE_HANDLES];

  TXBUF     TxBuf[NO_OF_CHS];
  RXBUF     RxBuf[NO_OF_CHS];

  MAILBOX   MailBox[NO_OF_MAILBOXES];

  int       Cmdfd;
  UBYTE		  VmReady;
  UBYTE     ComResult;
  UBYTE     ActiveComCh;   // Temporary fix until com channel functionality is in place, Ch interleaving not possible
  UBYTE     ReplyStatus;

  UBYTE     BrickName[vmBRICKNAMESIZE];

}COM_GLOBALS;

#if       (HARDWARE == SIMULATION)
  extern COM_GLOBALS * gComInstance;
  #define ComInstance (*gComInstance)

  void setComInstance(COM_GLOBALS * _Instance);
  COM_GLOBALS * getComInstance();
#else
  extern COM_GLOBALS ComInstance;
#endif

#endif /* C_COM_H_ */
