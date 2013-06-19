MTY CL
======
!!NB!! program is broken for AMD drivers 13.x

For AMD users to use this program you will need to revert to an earlier driver, such as 12.10.
Note that when reverting to an older driver version you also have to manually delete a bunch of DLLs, since the installation program won't deal with them all automatically. The program "driver fusion" should help with this.


A utility for generating custom tripcodes on various imageboards, utilising GPGPU Via OpenCL for faster generating.
Based on Chapuni's [MTY](http://naniya.sourceforge.jp/), released under GPL.
This project is also released under GPL.

Usage
-----
1. Download the [latest release](https://github.com/madsbuvi/MTY_CL/raw/master/Release/mty_cl_032.rar).
2. Start mty_gui.exe
3. Enter any custom tripcodes you wish for in the left-hand column. Supports Alphabets, i.e. [AaEe]pple will look for any of Apple, apple, Epple and epple.
4. Click 'Start (NVIDIA)' or 'Start (AMD)'.
5. A sample large target is provided which will search for most words longer than 8 characters in the english language, at once. Simply delete target.txt and rename sample_target_large.txt to target.txt to use it.
6. Results will show up with proper japanese characters in the right hand column when you close the search. There is no need to manually handle encoding!
7. If you want to send a tripcode password containing japanese characters to a friend, you can handle encoding by simply using a rich text format (e.g. paste it into a document in wordpad and send the .rtf).

Releases
--------
Current version is 0.41:
[Github](https://github.com/madsbuvi/MTY_CL/raw/master/Release/mty_cl_041.rar)
[Mediafire](http://www.mediafire.com/download/7fe6vpm6qr566aa/mty_cl_041.rar)

Original Mtycal with ui:
[Mediafire](http://www.mediafire.com/download.php?27p2lir4kedb721)

Supported architectures
-----------------------
Written primarily for GCN (Graphics Core Next), used in Souther Islands gpu's (AMD Radeon HD 77xx-79xx).
It does work on the older VLIW architectures, (AMD/ATI Radeon HD pre-77xx), but fairly slowly (only about half the speed of MTY) last it was tested.
version 0.40+ is also achieving decent speeds for nvidia cards of the compute 3.5 architecture.

I've also made a GUI wrapped for Chapuni's version that runs on all VLIW architectures: [mtycal](https://github.com/downloads/madsbuvi/MTY_CL/mtycal.rar).
The release of MTY that used VLIW AMD/ATI Architectures was unfortunately only released in closed source.
But it was released on his sourceforge claiming GPL, so at least redistribution should be ok.

Performance
-----------
In terms of potential operations per second, the GCN architecture is a step down from the VLIW architectures. AMD has chosen to sacrifice
some of the brilliant potential number of operations per second obtainable in the VLIW architectures, for a more manageable ISA (the new is scalar, rather than VLIW).
Therefore it is expected that a 7xxx card will have lower performance for some applications than the equivalent 6xxx card.
MTY CL obtains up to 125 million trips / second on my own Radeon HD 7850, which by calculation is about 45% of the potential throughput.
By comparison, Chapuni's MTY, obtains about 75 million trips / second on my Radeon HD 4870. Which by the same calculation is approximately 40% of the potential throughput.

The calculation is (obtained throughput)/((Potential operations * bitslices)/(Operations required by one bitsliced crypt(3) procedure))
Operations required by one bitsliced crypt(3) procedure is 202 000, counting only ALU operations (counting the gates in each S-box as well as the 10 XOR operations involving the keys and ciphertexts for each s-box).
Which gives (125000000)/((1761000000000*32)/(202000)) = 0.45 and (75000000)/((1200000000000*32)/(202000)) = 0.40

For nVidia cards, obtaining high performance with registers limited to only 64 does not seem feasible. I have at least not been able to figure any scheme to
obtain any speed better than what is currently obtained.
In the 7xx series, however, the compute 3.5 architecture allows me to use 256 registers, allowing much better performance. Although the register usage is sufficiently high that not enough threads can be scheduled
to obtain performance matching AMD cards for this purpose.

Bugs
----
On AMD drivers 13.x the code appears completely broken. Visual artifacts, extremely poor performance and/or garbage output depending on which driver.
Drivers before 13.1 should work. The issue has been reported to AMD...

Don't hesitate to contact me or open an issue if you have any problems with the program.

Changelog
---------
0.41. - Fixed for amd cards and updated user interface with one button for amd, and one for nvidia.

0.40. - Addd support for nVidia cards. Primarily the 7xx series, earlier series will likely not see great performance. The issue with duplicate keys has been resolved.

0.32. - Fixed a crash

0.31. - Further speedup (1.25x). //No more duplicates.// Well it seems it does still happen, but rarely enough.

0.30. - Shift_JIS keys. Key selection makes a lot of duplicates.

0.20. - Massive speedup (2x).

0.11. - Added a graphical user interface, also released a wrapper for mtycal.

0.10. - First release. ASCII Only and relatively slow. Key selection makes some duplicates.
