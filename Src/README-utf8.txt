INSTALLATION
The program does not need installation, unrar to a folder of your choice.

USAGE
Run either mty_cl.exe or mty_gui.exe.
The latter is just a wrapper for mty_cl.exe, made to simplify input/output to/from the program.
Write what tripcodes you are searching for in target.txt (or the gui), separated by lines.

target.txt does NOT support regexp, but DOES support alphabets.
I.e. making a target "[Aa]pple" (without quotes) will search for both "Apple" and "apple".
There is not currently any other way of making a case insensitive search then to make every
letter of the target an alphabet, such as [Ee][Xx][Aa][Mm][Pp][Ll][Ee].

CREDITS
This fork is written by Mads Buvik Sandvei based on Naniya/MTY written by Chapuni.
This program and its source code is distributed under the GNU general public license.
It contains work from
- http://naniya.sourceforge.jp/
	Files: All files marked as written by Chapuni
	Distributed under GNU general public license
	
- http://www.openwall.com/john/
	john-1.7.2
	Copyright (c) 2000-2001,2005,2006 by Solar Designer and others
	Files: x86-mmx.S, x86-sse.S 

- http://www.darkside.com.au/bitslice/
	By Matthew Kwan
	x64-alu.inc x64-xmm.inc by Matthew Kwan 

COMPILATION
On windows edit the makefile to point to your installation of opencl and GTK development libraries.
GTK is optional, without it do "make core" to make only the program without a GUI wrapper.
Comment out the windows section to build for linux, and vice-versa.
Use make to run the makefile script.



KNOWN ISSUES
- Duplicate results:
Occationally finds the same result twice. I'm aware of this but it is of little consequence.

- Empty log:
Sometimes the user interface will not display the contents of log.txt. This is because glib, used
for the conversion from shift_jis to utf8, throws an error and doesn't convert anything if a single
character is not valid shift_jis, and the program will sometimes generate invalid shift_jis.
For now fix this by clearing out log.txt of anything that is not correct shift_jis. Acquire notepad++
or a similar useful text editor, open log.txt, change to shift_jis encoding, and backup and wipe entries
with bad shift_jis.

- Broken gui on linux:
On linux the GUI does not actually launch the executable. You'll have to start it from the command line.

- Program crashes for some users:
In windows DEP will crash the program. Disable DEP either globally or for mty_cl.exe
The program has previously crashed instantly for ivy bridge users on windows 8. This issue arose from 
mingw generating unsafe vector code, and should be fixed.
Older AMD drivers crash during JIT compilation, update them.

- CL_INVALID_BUFFER_SIZE:
For newer cards the gpu memory requirements exceeds the default maximum allowed allocation, and the
program crashes with "CL_INVALID_BUFFER_SIZE". To get around this run the command 
	setx GPU_MAX_ALLOC_PERCENT 100
from the command line. The settings should stick, but if it does not create a .bat file that runs the
command and then mty_cl.

- For older cards from AMD the search is slower than the original MTY:
AMD cards with the now old VLIW architecture were not given priority as the original MTY still works for these.

- Most NVIDIA cards search slow or just crash:
On NVIDIA cards with "compute capability" lower than 3.5 there is no way to overcome architectural limitations.
To optimally run the search the code needs 130 registers per thread to avoid the need to use shared or global
memory, but on compute 3.0 cards and older only 64 are available leading to major register spill to global memory.
Manually spilling to shared memory turns out to be much slower than spilling everything to global memory, as
occupancy becomes too low.
If your GPU watchdog timer is set low, the code can then crash the driver as it simply takes too long time to execute.



CONTACT
mailto: madssandvei@zoho.com
create an issue at: https://github.com/madsbuvi/MTY_CL


This readme last updated: 31/07/2014