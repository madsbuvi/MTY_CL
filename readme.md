MTY CL
======

A utility for generating custom tripcodes on various imageboards, utilising GPGPU Via OpenCL for faster generating.
Based on Chapuni's [MTY](http://naniya.sourceforge.jp/), released under GPL.
This project is also released under GPL.

Usage
-----
1. Download the [latest release](https://github.com/downloads/madsbuvi/MTY_CL/mty_cl_031.rar).
2. Start mty_gui.exe
3. Enter any custom tripcodes you wish for in the left-hand column. Supports Alphabets, i.e. [AaEe]pple will look for any of Apple, apple, Epple and epple.
4. Click 'Start Search'.
5. A sample large target is provided which will search for most words longer than 8 characters in the english language, at once. Simply delete target.txt and rename sample_target_large.txt to target.txt to use it.
6. Results will show up with proper japanese characters in the right hand column when you close the search. There is no need to manually handle encoding!
7. If you want to send a tripcode password containing japanese characters to a friend, you can handle encoding by simply using a rich text format (e.g. paste it into a document in wordpad and send the .rtf).

Releases
--------
Current version is 0.31: [link](https://github.com/downloads/madsbuvi/MTY_CL/mty_cl_031.rar)


Supported architectures
-----------------------
Currently only GCN (Graphics Core Next), used in Souther Islands gpu's (AMD Radeon HD 77xx-79xx).
It does work on the older VLIW architectures, (AMD/ATI Radeon HD pre-77xx), but terribly slowly.

However, i've also made a GUI wrapped for Chapuni's version that runs on all VLIW architectures: [mtycal](https://github.com/downloads/madsbuvi/MTY_CL/mtycal.rar).
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

Changelog
---------
0.31. - Further speedup (1.25x). //No more duplicates.// Well it seems it does still happen, but rarely enough.

0.30. - Shift_JIS keys. Key selection makes a lot of duplicates.

0.20. - Massive speedup (2x).

0.11. - Added a graphical user interface, also released a wrapper for mtycal.

0.10. - First release. ASCII Only and relatively slow. Key selection makes some duplicates.