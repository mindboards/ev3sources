LEGO MINDSTORMS EV3 source code
===============================

See [the release accouncment on BotBench][1] for more information.

## Things you will need

* A Linux (virtual) installation.  I used Ubuntu 13.04 on VMware Workstation, but I am sure other combinations also work.
* Eclipse.  I used Helios Service Release 1, but I know more recent versions will also work.
* Java JRE (for Eclipse)
* Code Sourcery Lite for ARM version 2009q1-203.  You can [download here directly][4].
* A USB to serial port dongle.  You need to splice an NXT cable and hook up dig0 (pin 5) and dig1 (pin 6) to TX and RX, not 100% which way around.  GND is pin 2 and 3, just pick one.  The brick’s console is on port 1 and has a baud rate of 115200 8N1.  I have a pre-made one with an NXT socket, it’s not actually as fancy as it sounds.
* An SD card to put your custom firmware on. It doesn’t use up a lot of space, but I’d stick with a simple 2GB one
* A pair of flat-nosed pliers, for removing the SD card
* A Netgear WNA1100 WiFi dongle.  It is currently the only WiFi dongle that is supported by the EV3’s firmware.

## Getting Started

Use Git to clone this repo:

    git clone https://github.com/mindboards/ev3sources.git

The scripts in the EV3 sources expect the source code to live in a projects folder in your home directory. You will need to create a  projects symlink to the ev3sources folder that was created when you cloned the Git repo.

I’ll publish some more elaborate HOWTOs in the next few days, for now, take a look at the scripts in ev3sources/lms2012/open_first.  You can open the lms2012 folder in Eclipse to open all of the sub projects, including the kernel modules and shared libraries.

## Contributing

To make modifications to the source code, click on the **Fork** button at the top of this page. This will create a copy of this repository under your own GitHub account. You can make changes to this repository as you wish. [See this page for more information about Forking.][2]

If you want to make a change to this shared repo, submit a **Pull Request**, which people can discuss and decide whether to apply your changes or not. [See this page for more information about Pull Requests.][3].

  [1]: http://botbench.com/blog/2013/07/31/lego-mindstorms-ev3-source-code-available/
  [2]: https://help.github.com/articles/fork-a-repo
  [3]: https://help.github.com/articles/using-pull-requests
  [4]: http://go.mentor.com/2ig4q
