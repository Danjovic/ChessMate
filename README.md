# ChessMate

The ChessMate is an AVR ATmega328P running at 25 MHz with chess Engine based on recursive negamax search micro-Max 4.8 by [H.G. Muller]
The hash table was removed by [Andre Adrian].


Our contribution are the following features:
- Included [Threefold repetition rule] using Compact Chess Representation of history boards ([C.C.R.]), we made it valid 
for the last 15 boards with 32 bytes per board; so the history boards occupies 480 bytes of RAM.
- Included the [Fifty-move rule] check from [fairy-Max].

Processing Speed:
For instance, the 32-bit test version of Rybka on AMD 64-bit CPU running on 2.4Ghz achieves 104 Knps (thousands of nodes 
per second); refer to [C.C.R.].


The ATmega328P ChessMate at 25 MHz approach, can reach ~1.3 Knps (1298.1 nodes/s).

A node refers to a branch of play the computer investigated. 

[H.G. Muller]: <http://home.hccnet.nl/h.g.muller/max-src2.html>
[Andre Adrian]: <http://chessprogramming.wikispaces.com/Andre+Adrian>
[C.C.R.]: <http://www.doiserbia.nb.rs/img/doi/0354-0243/2012/0354-02431200011V.pdf>
[fairy-Max]: <http://home.hccnet.nl/h.g.muller/CVfairy.html>
[Demo Video]: <https://youtu.be/5uhqcVmfm20>
[Hackaday Entry]: <https://hackaday.io/project/8705-chess-mate>
[Threefold repetition rule]: <https://en.wikipedia.org/wiki/Threefold_repetition>
[Fifty-move rule]: <https://en.wikipedia.org/wiki/Fifty-move_rule>

> First release at 15-Feb-2016

How to play:
Initially, it should appear "PLAY" at the display
- Press and hold "NEW" + the number of level
- Enter your 4 digit move then press "ENTER"
- Wait for lily thinking (it will be displayed blinking dots)
- The lily move will be displayed just after the thinking
- If you entered an erroneous digit; press "CLR", short press, to delete one digit
- If you want to delete all digits entered, press and hold "CLR"

See the [Demo Video] and the [Hackaday Entry]

