MTY CL
======
A utility for generating custom tripcodes on various imageboards, utilising GPGPU Via OpenCL for faster generating.
Based on Chapuni's [MTY](http://naniya.sourceforge.jp/), released under GPL.
This project is also released under GPL.

NB
--
Version 0.42 resolves the issues with AMD drivers if you use the latest driver (13.9)
This version runs slightly slower (for AMD users) than earlier versions, so for faster searching revert to earlier drivers (such as 12.10) and use version 0.41.
Consult google on how to properly roll back AMD drivers, as AMD's uninstall does not do this properly.


Usage
-----
1. Download the latest release: [Windows](https://github.com/madsbuvi/MTY_CL/raw/master/Release/mty_cl_042.rar) [Linux](https://github.com/madsbuvi/MTY_CL/raw/master/Release/mty_cl_042_linux.rar)
2. Start mty_gui.exe
3. Enter any custom tripcodes you wish for in the left-hand column. Supports Alphabets, i.e. [AaEe]pple will look for any of Apple, apple, Epple and epple.
4. Click 'Start (NVIDIA)' or 'Start (AMD)' (linux users should start the relevant executable from the command line)
5. A sample large target is provided which will search for most words longer than 8 characters in the english language, at once. Simply delete target.txt and rename sample_target_large.txt to target.txt to use it.
6. Results will show up with proper japanese characters in the right hand column when you close the search. There is no need to manually handle encoding!
7. If you want to send a tripcode password containing japanese characters to a friend, you can handle encoding by simply using a rich text format (e.g. paste it into a document in wordpad and send the .rtf).

Releases
--------
Current version is 0.42:
Github: [Windows](https://github.com/madsbuvi/MTY_CL/raw/master/Release/mty_cl_042.rar) [Linux](https://github.com/madsbuvi/MTY_CL/raw/master/Release/mty_cl_042_linux.rar)
Mediafire:[Windows](http://www.mediafire.com/download/0ri5gk2jhj987gu/mty_cl_042.rar) [Linux](http://www.mediafire.com/download/knc3we7c1jz1pwk/mty_cl_042_linux.rar)

Previous version 0.41:
[Github](https://github.com/madsbuvi/MTY_CL/raw/master/Release/mty_cl_041.rar)
[Mediafire](http://www.mediafire.com/download/7fe6vpm6qr566aa/mty_cl_041.rar)

Original Mtycal with ui:
[Mediafire](http://www.mediafire.com/download.php?27p2lir4kedb721)

Supported architectures
=======================
AMD
---
Written primarily for GCN (Graphics Core Next), used in Souther Islands gpu's (AMD Radeon HD 77xx-79xx).
It does work on the older VLIW architectures, (AMD/ATI Radeon HD pre-77xx), but fairly slowly (only about half the speed of MTY) last it was tested.

I've also made a GUI wrapped for Chapuni's version that runs on all VLIW architectures: [mtycal](https://github.com/downloads/madsbuvi/MTY_CL/mtycal.rar).
The release of MTY that used VLIW AMD/ATI Architectures was unfortunately only released in closed source.
But it was released on his sourceforge claiming GPL, so at least redistribution should be ok.

NVIDIA
------
The 0.41-42 releases support nvidia GPUs to some extent.
Nvidias older than 780/Titan do not seem feasible to implement a fast solution to the DES algorithm on. These will run fairly slowly
compared to the newer nvidia cards and all AMD cards.
Older nvidias MAY CRASH due to slow execution, if that happens it's best to use a cpu only version like tripcode explorer.


Bugs
----
Sometimes the user interface will not display the contents of log.txt
this is because the program has generated a password that gtk+ can't decode into shift_jis.
open log.txt in notepad++ (google it) and change encoding to shift_jis and then remove any results that have
passwords that aren't rendering properly (back them up if you wish, of course).

On linux the GUI will not properly launch the amd/nvidia searchers, start them from the terminal instead.

For some users the program will crash instantly. All known cases are Ivy Bridge on Windows 8. Try running the [Debug Build](http://www.mediafire.com/download/i9fibp97ps6amll/mty_cl_042_-g.rar).
If that also crashes, run the debug build through [gdb](http://www.equation.com/servlet/equation.cmd?fa=gdb) (see [issue 7](https://github.com/madsbuvi/MTY_CL/issues/7) for instructions) and give me the output (respond to issue 7 or make a new issue).

Don't hesitate to contact me or open an issue if you have any problems with the program.


Known Issues
------
Performance is slow on nvidia cards pre-780. This is because the architecture limitations make programming the DES algorithm very dificult.
Being limited to 64 registers when i need at least ~130 to not have to spill to memory means the computations become bandwidth limited, which is an order of magnitude slower.
This is the same reason why nvidia users will notice a general system slow-down when using the application, as the gpu hog becomes more noticeable when individual runs from this program takes much longer time.

Changelog
---------
0.42. - Workaround for the AMD diver issue. Linux binaries also released.

0.41. - Fixed for amd cards and updated user interface with one button for amd, and one for nvidia.

0.40. - Addd support for nVidia cards. Primarily the 7xx series, earlier series will likely not see great performance. The issue with duplicate keys has been resolved.

0.32. - Fixed a crash

0.31. - Further speedup (1.25x). //No more duplicates.// Well it seems it does still happen, but rarely enough.

0.30. - Shift_JIS keys. Key selection makes a lot of duplicates.

0.20. - Massive speedup (2x).

0.11. - Added a graphical user interface, also released a wrapper for mtycal.

0.10. - First release. ASCII Only and relatively slow. Key selection makes some duplicates.
