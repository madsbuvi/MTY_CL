MTY CL
======
A utility for generating custom tripcodes on various imageboards, utilising GPGPU Via OpenCL for faster generating.
Based on Chapuni's [MTY](http://naniya.sourceforge.jp/), released under GPL.
This project is also released under GPL.

NB
--
As of AMD driver 14.4 (release) all driver issues appear to be resolved for me and my R9 290

AMD users can run the NVIDIA executable until next release for a slight increase in performance (the driver issues were the reason the executables were ever split). If issues continue for you even with the 14.4 driver, let me know and i'll keep the amd executable for future versions.


Usage
-----
1. Download the latest release: [Windows](https://github.com/madsbuvi/MTY_CL/raw/master/Release/mty_cl_043.rar) [Linux](https://github.com/madsbuvi/MTY_CL/raw/master/Release/mty_cl_043.tar.gz)
2. Start mty_gui.exe
3. Enter any custom tripcodes you wish for in the left-hand column. Supports Alphabets, i.e. [AaEe]pple will look for any of Apple, apple, Epple and epple.
4. Click 'Start (NVIDIA)' or 'Start (AMD)' (linux users should start the relevant executable from the command line)
5. A sample large target is provided which will search for most words longer than 8 characters in the english language, at once. Simply delete target.txt and rename sample_target_large.txt to target.txt to use it.
6. Results will show up with proper japanese characters in the right hand column when you close the search. There should be no need to manually handle encoding. However, there is a bug related to this (Read "Bugs" below).

Releases
--------
Current version is 0.43:
Github: [Windows](https://github.com/madsbuvi/MTY_CL/raw/master/Release/mty_cl_043.rar) [Linux](https://github.com/madsbuvi/MTY_CL/raw/master/Release/mty_cl_043.tar.gz)
Mediafire:[Windows](http://www.mediafire.com/download/66yf8a8yd1xdk1f/mty_cl_043.rar) [Linux](http://www.mediafire.com/download/gf1r5hfw3tk3fdi/mty_cl_043.tar.gz)

Previous version 0.41:
[Github](https://github.com/madsbuvi/MTY_CL/raw/master/Release/mty_cl_041.rar)
[Mediafire](http://www.mediafire.com/download/7fe6vpm6qr566aa/mty_cl_041.rar)

Original Mtycal with ui:
[Mediafire](http://www.mediafire.com/download.php?27p2lir4kedb721)

Supported architectures
=======================
AMD
---
Written primarily for GCN (Graphics Core Next), used in Southern Islands gpu's (AMD Radeon HD 77xx-79xx) and newer.

It does work on the older VLIW architectures, (AMD/ATI Radeon HD pre-77xx), but fairly slowly (only about half the speed of MTY) last it was tested.

I've also made a GUI wrapped for Chapuni's version that runs on all VLIW architectures: [mtycal](https://github.com/downloads/madsbuvi/MTY_CL/mtycal.rar).

The release of MTY that used VLIW AMD/ATI Architectures was unfortunately only released in closed source, so i cannot simply include Chapuni's GPU code. However it was released on his sourceforge claiming GPL, so redistribution of his program should be ok.

NVIDIA
------
Support for NVIDIA gpus has existed since version 0.40, however only NVIDIA cards with compute capability 3.5 or higher can expect anything resembling decent performance. Currently this is only the 780 card and newer.

This is because on NVIDIA cards with "compute capability" lower than 3.5 there is no way to overcome architectural limitations. To optimally run the search the code needs 130 registers per thread to avoid the need to use shared or global memory, but on compute 3.0 cards and older only 64 are available leading to major register spill to global memory. Manually spilling to shared memory turns out to be much slower than spilling everything to global memory, as occupancy becomes too low.


Known Issues
------
* **Duplicate results:**
  Occationally finds the same result twice. I'm aware of this but it is of little consequence.
* **Blank log or blank passwords:**
  * Glib, used for the conversion from shift_jis to utf8, throws an error and doesn't convert anything if a single character is not valid shift_jis, and the program will sometimes generate passwords with invalid shift_jis. To get these passwords you will need to acquire notepad++ or a similar text editor that is better at handling bad encoding, open log.txt, and change to shift_jis encoding.
  * The CPU searcher uses a different method of choosing keys than the GPU searcher. Occasionally it will recognize a password as being bad shift_jis and choose not to print it at all, in which case the password will be blank even if you open it using notepad++. This part of the code was not written by me, and i don't intend to delve much into it so this problem will unfortunately stay for the foreseeable future. If the trip was a good find you can still try to reconstruct it using [shift_jis tables](http://en.wikipedia.org/wiki/Shift_jis)
* **Broken gui on linux:**
On linux the GUI does not actually launch the executable. You'll have to start it from the command line.
* **Program crashes for some users:**
  * In windows DEP will crash the program. Disable DEP either globally or for mty_cl.exe
  * Older AMD drivers crash during JIT compilation, update them.
  * The program has previously crashed instantly for ivy bridge users on windows 8. This issue arose from 
mingw generating unsafe vector code, and should be fixed. If not open an issue.
* **CL_INVALID_BUFFER_SIZE:**
For newer cards the gpu memory requirements exceeds the default maximum allowed allocation, and the program crashes with "CL_INVALID_BUFFER_SIZE". To get around this run the command *setx GPU_MAX_ALLOC_PERCENT 100* from the command line. The settings should stick, but if it does not create a .bat file that runs the command and then mty_cl.
* **MTY_CL is slow**:
  * For older cards from AMD the search is slower than the original MTY: AMD cards with the now old VLIW architecture were not given priority as the original MTY still works for these.
  *  On NVIDIA cards with "compute capability" lower than 3.5 there is no way to overcome architectural limitations. To optimally run the search the code needs 130 registers per thread to avoid the need to use shared or global memory, but on compute 3.0 cards and older only 64 are available leading to major register spill to global memory. Manually spilling to shared memory turns out to be much slower than spilling everything to global memory, as occupancy becomes too low. If your GPU watchdog timer is set low, the code can then crash the driver as it simply takes too long time to execute.
* **Garbage results:**
Ever since the 13.* series of AMD drivers the opencl compiler has been very buggy for my code. At the moment of writing, the latest driver (14.4) fixes all known issues with this, so make sure your drivers are the latest version. Do a full driver cleanup and then install version 14.4 or later before opening an issue about this.

Changelog
---------
* 0.50 (upcoming)
  * Returned to a single-executable as AMD driver issues are resolved.
  * The program no longer spits out an error message if it finds a platform with no devices.
  * Issues a more informative error message if opencl throws CL_INVALID_BUFFER_SIZE
  * In some cases the logfile would show as empty in the gui. This was due to glib handling broken encoding in some passwords poorly. "broken" passwords now show up as blank, and must be manually read as shift_jis from log.txt using a proper editor such as notepad++.
  * Glib would duplicate newlines every time target.txt was saved by the gui, this has been fixed.
* 0.43
  * Fixed a problem where mty_cl would exit if it found a platform with no devices, instead of ignoring these platforms like it should.
  * Fixed a problem where the latest intel chips would crash instantly.
* 0.42
  * Yet another fix for AMD drivers...
  * Linux binaries released.
* 0.41
  * Fix for AMD drivers.
  * Updated user interface with one button for amd, and one for nvidia.
* 0.40
  * Added support for NVIDIA cards. Primarily only the 7xx series will work well, earlier series will likely not see great performance.
* 0.32
  * Fixed a crash
* 0.31
  * Speedup (1.25x)
* 0.30
  * Shift_JIS keys.
* 0.20
  * Speedup (2x).
* 0.11
  * Added a graphical user interface. Also released for mtycal.
* 0.10 (First release)
  * ASCII Only.
  * GCN only.
