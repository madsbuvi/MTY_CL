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


Changelog
---------
0.31. - Further speedup (1.25x). No more duplicates.

0.30. - Shift_JIS keys. Key selection makes a lot of duplicates.

0.20. - Massive speedup (2x).

0.11. - Added a graphical user interface, also released a wrapper for mtycal.

0.10. - First release. ASCII Only and relatively slow. Key selection makes some duplicates.



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

How to contact me:
mailto: madsbuvi@stud.ntnu.no
create an issue at: https://github.com/madsbuvi/

compilation:
Edit makefile to point to your installation of opencl and gtk development libraries.
Use make to run the makefile script.

The following is the original readme file, distributed along with MTY from http://naniya.sourceforge.jp/:
まあ、待て屋。
"ドツボ脱却記念で寝起きもすっきり"

----プロジェクト・ナニ屋
Fri 12 Oct 2007


* これはなに?

"まあ、待て屋。" 通称待て屋。
2ちゃんねるのトリップをPCにいっしょうけんめい探させるツールです。
検索人をなかなか隠居できない私が、主に自分で使うために作りました。


* どれをつかえばいいの?

ファイルが4つ入っています。それぞれを実行してみていちばん速いのを
使っちゃっててください。

っていうだけじゃ不親切なので、簡単に解説します。

- 古い PC (Pentium III, AthlonXP)を使ってる人

  MMX版をお使いください。

- Pentium4よりも新しいパソコンを使ってる人

  XMM版をお使いください。ただし、
  Pentium Mなどのノートブックでは、MMX版の方が速かったりするかも。

- 64ビットOSをインストールしてしまった人

  64XMM を使ってめくるめく64ビットの世界へどうぞ。
  32ビット版にくらべて数割速いことも。
  Athlon64, Opteronでは、64ALU 版の方が速かったりします。
  (Opteron の新しいのとか Phenom,
   俗に言う K10 では 64XMM の方が速いかもね)


* つかいかた

カレントディレクトリに target.txt が必要です。存在しない場合、
「勝手にベンチマークモード」に突入し、ひたすら数字を垂れ流し
続けます。
実行結果は log.txt に吐かれます。

target.txt の例を sample_target.txt として同梱してあります。


以前簡単に実装してみせて以来永らく実装していなかった、連モノ検索を
復活させました。純8連以上(つまり9連も10連も)のトリップはターゲットに
入れなくても勝手に出てきます。これをやめさせる方法は用意していません。
つまり、あなたも"連モノ探すバカ(某氏語録)"になってしまいます。
そのうち、連モノ表現式の拡張を入れるので勘弁。

本版は、あらん限りのCPUを見つけ、スレッドをたくさん起動しようとします。
設計上は数十プロセッサいけますが、4プロセッサ(Core2 Quad)までしか
試していません。
使用するCPUの数を制限する方法はまだ用意していません。
コマンドプロンプトなどから以下のようなコマンドを投げることによって、
制限モードで走る、かもしれません。

C:\> start /affinity 1 mty


* メッセージについて

- エラーの行数がずれている?

  ちょっとした手抜きで、実際にまずかった箇所と報告された箇所の
  行番号が1～2行ほどずれることがあります。そのうちどうにかします。

- 「長すぎる単語」の類

  10文字の単語でも、10文字目が絶対に出ない文字である場合、
  検索せずに弾き飛ばします。

- 「木をプチ最適化してみるテスト…
    ムダを ** 個やっつけました。」

  ターゲットファイルの書き方を工夫してもムダはなかなか減りません。
  むしろ、ムダがやっつけられたので安心してください。
  この処理で思いっきり時間がかかるような使い方は想定外ですので、
  ご報告頂ければ前向きに善処します。

- 「難しい処理に挑戦中…」

  「ドツボにハマり中…」は、10/12版で出なくなりました。
  もし、「難しい処理に挑戦中」に時間がとてつもなくかかるケースが
  あったら想定外です。改良にご協力ください。


* トリップが化けます…

2chで使えそうな文字を片っ端からキーに使用するようにしているため、
2chトリップ類似仕様を実装した他サイトにて使えないトリップを吐くことが
あります。このツールは2ch専用なのだとあきらめてください。

なお、ごく希に、2chでも使えないトリップを吐くことがあります。
2ch書き込み時のNGワードチェックに引っかかると、キーが想定しないものに
置換されてしまい、どうしても再現できない、といったことが起こりえます。

ちなみに2chトリップの仕様についてですが、明確な仕様書のようなものは
現存していません。というのは、2ch運営陣も、トリップ仕様について
よくわかっていなかったりするからです。


* おことわりがき

- GNU General Public License に則って扱ってください。
  開発は SOURCEFORGE.JP にて行っています。
  http://naniya.sourceforge.jp/

- x86-mmx.S および x86-sse.S は、john-1.7.2 に
  収録されていたものをそのまま持ってきました。
  Copyright (c) 2000-2001,2005,2006 by Solar Designer and others
  詳しくは、該当ソースファイルのヘッダを参照のこと。
  → http://www.openwall.com/john/

-- ファイル x64-alu.inc x64-xmm.inc は、Matthew Kwan が生成した演算式に
  基づいています。
   → http://www.darkside.com.au/bitslice/


* Thanks to

よだきんぼ(東九州弁)の私の代わりにサポートしてくれている
トリップあげるお(　^ω^)板のみなさん、ありがとう。
爆速Core2 Quad(しかもx64)にて積極的に協力してくれる、
◆ARAMAKI/VY さん、ありがとう。

怒濤のストレステストで禿増してくれる、のたんぺ様ありがとう。

PS3版ドッグフードを喰わされている若干名の検索人さんありがとう。

トリップ検索界を「トリップはファッションだ!」へとパラダイムシフト
させてしまうきっかけをつくった、㍉こと Million_Seeker を送り出して
隠居中の L2 さん、彼のワークなくして今のプロジェクト・ナニ屋は語れません。
ありがとう。


* How to contact me

mailto:chapuni@users.sourceforge.jp
MSN Messenger: webmaster@chapuni.com


$Id: README-utf8.txt 183 2007-10-12 11:20:23Z chapuni $
*EOF
